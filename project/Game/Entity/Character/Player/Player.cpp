#include "Player.h"
#include "Player.h"

#include <cmath>

namespace
{
	// 通常時の移動速度[m/s]
	constexpr float kNormalMoveSpeed = 6.0f;

	// 構え時の移動速度倍率
	constexpr float kStanceMoveSpeedMultiplier = 0.5f;

	// ダッシュ時の移動速度倍率
	constexpr float kDashSpeedMultiplier = 2.0f;

	/// @brief カメラ基準の入力方向をワールド方向へ変換する
	/// @param cameraLocalDirection 
	/// @param cameraYaw 
	/// @return 
	Vector2 ToWorldMoveDirectionFromCamera(const Vector2& cameraLocalDirection, float cameraYaw)
	{
		// カメラ前方向(XZ平面)
		const Vector2 forward = Vector2(std::sin(cameraYaw), std::cos(cameraYaw));

		// カメラ右方向(XZ平面)
		const Vector2 right = Vector2(forward.y, -forward.x);

		// カメラ基準入力をワールド方向へ変換する
		return right * cameraLocalDirection.x + forward * cameraLocalDirection.y;
	}
}

/// @brief コンストラクタ
/// @param initData 
Player::Player(const InitData& initData) : Character(initData)
{
	// タグを指定する
	characterTag_ = CharacterTag::PlayerSide;
}

/// @brief 初期化
void Player::Initialize()
{
	assert(model_);


	// モデルをワールドトランスフォームの子にする
	model_->SetParent(worldTransform_.get());


	// 移動入力の生成
	inputMove_ = std::make_unique<InputGamepadLeftStick>("Player_Move", InputState::Press, 0, Vector2(0.0f, 0.0f), 0.5f);

	// ダッシュ入力の生成
	inputDash_ = std::make_unique<InputGamepadButton>("Player_Dash", InputState::Trigger, 0, XINPUT_GAMEPAD_A);

	// 回避入力の生成
	inputAvoid_ = std::make_unique<InputGamepadButton>("Player_Avoid", InputState::Trigger, 0, XINPUT_GAMEPAD_A);

	// 弱攻撃入力の生成
	inputLightAttack_ = std::make_unique<InputGamepadButton>("Player_LightAttack", InputState::Trigger, 0, XINPUT_GAMEPAD_X);

	// 強攻撃入力の生成
	inputHeavyAttack_ = std::make_unique<InputGamepadButton>("Player_HeavyAttack", InputState::Trigger, 0, XINPUT_GAMEPAD_Y);

	// 構え入力の生成
	inputStance_ = std::make_unique<InputGamepadButton>("Player_Stance", InputState::Press, 0, XINPUT_GAMEPAD_RIGHT_SHOULDER);

	// キーの前移動入力の生成
	keyFrontMove_ = std::make_unique<InputKey>("Player_KeyFrontMove", InputState::Press, DIK_W);

	// キーの後移動入力の生成
	keyBackMove_ = std::make_unique<InputKey>("Player_KeyBackMove", InputState::Press, DIK_S);

	// キーの左移動入力の生成
	keyLeftMove_ = std::make_unique<InputKey>("Player_KeyLeftMove", InputState::Press, DIK_A);

	// キーの右移動入力の生成
	keyRightMove_ = std::make_unique<InputKey>("Player_KeyRightMove", InputState::Press, DIK_D);

	// キーの構え入力の生成
	keyStance_ = std::make_unique<InputKey>("Player_KeyStance", InputState::Press, DIK_SPACE);
}

/// @brief 更新処理
void Player::Update()
{
	// 回避中は回避更新のみ行い、他の操作は受け付けない
	if (isAvoid_)
	{
        // 回避中でも回避ボタン入力があれば次の回避を予約する
		bool hasMoveInput = false;
		const Vector2 moveInputDirection = GetMoveInputDirection(hasMoveInput);
		if (isStance_ && inputAvoid_ && inputAvoid_->IsInput())
		{
			ReserveNextAvoid(moveInputDirection, hasMoveInput, GetCameraYaw());
		}

		Character::Update();
		return;
	}

    // 構え状態を更新する
	UpdateStanceState();

	// 移動入力方向を取得する
	bool hasMoveInput = false;
	const Vector2 moveInputDirection = GetMoveInputDirection(hasMoveInput);

	// 構え中に回避ボタンを押したら回避を開始する
	if (isStance_ && inputAvoid_ && inputAvoid_->IsInput())
	{
       StartAvoid(moveInputDirection, hasMoveInput, GetCameraYaw());
		Character::Update();
		return;
	}

	// ダッシュ状態を更新する
	UpdateDashState(hasMoveInput);

	// 状態に応じた移動速度を計算する
	const float moveSpeed = GetCurrentMoveSpeed();

	if (hasMoveInput)
	{
		// カメラ基準の入力方向をワールド方向へ変換する
       const Vector2 worldMoveDirection = ToWorldMoveDirectionFromCamera(moveInputDirection, GetCameraYaw());
		SetMoveInputXZ(worldMoveDirection.Normalize(), moveSpeed);
	}
	else
	{
		// 入力がない場合は移動を停止する
		SetMoveInputXZ(Vector2(0.0f, 0.0f), moveSpeed);
	}

	// 基底クラスの更新
	Character::Update();
}

/// @brief 構え状態を更新する
void Player::UpdateStanceState()
{
	// 構え入力中は構えフラグを立て、離したらフラグを下ろす
	const bool isGamepadStance = (inputStance_ && inputStance_->IsInput());
	const bool isKeyStance = (keyStance_ && keyStance_->IsInput());
	isStance_ = (isGamepadStance || isKeyStance);
}

/// @brief 移動入力方向を取得する
/// @param hasMoveInput
/// @return
Vector2 Player::GetMoveInputDirection(bool& hasMoveInput) const
{
	// WASDキーの入力方向を作成する
	Vector2 keyMoveDirection = Vector2(0.0f, 0.0f);
	if (keyFrontMove_ && keyFrontMove_->IsInput())
	{
		keyMoveDirection.y += 1.0f;
	}
	if (keyBackMove_ && keyBackMove_->IsInput())
	{
		keyMoveDirection.y -= 1.0f;
	}
	if (keyLeftMove_ && keyLeftMove_->IsInput())
	{
		keyMoveDirection.x -= 1.0f;
	}
	if (keyRightMove_ && keyRightMove_->IsInput())
	{
		keyMoveDirection.x += 1.0f;
	}

	// 移動入力方向を求める（キー入力を優先）
	hasMoveInput = false;
	if (keyMoveDirection.Length() > 0.0f)
	{
		hasMoveInput = true;
		return keyMoveDirection.Normalize();
	}

	if (inputMove_ && inputMove_->param_ && inputMove_->IsInput())
	{
		// 左スティックの入力を取得する
		const Vector2 stick = GrowthEngine::GetInstance()->GetGamepadLeftStick(inputMove_->param_->controller);
		if (stick.Length() > 0.0f)
		{
			hasMoveInput = true;
			return stick.Normalize();
		}
	}

	return Vector2(0.0f, 0.0f);
}

/// @brief ダッシュ状態を更新する
/// @param hasMoveInput
void Player::UpdateDashState(bool hasMoveInput)
{
	// 構え中はダッシュできない
	// ダッシュ中に構えた場合もダッシュを解除する
	if (isStance_)
	{
		isDash_ = false;
		return;
	}

	// ダッシュボタンを押したらダッシュフラグを立てる
	if (inputDash_ && inputDash_->IsInput() && hasMoveInput)
	{
		isDash_ = true;
	}

	// ダッシュ中に移動入力がなくなったらダッシュを終了する
	if (isDash_ && !hasMoveInput)
	{
		isDash_ = false;
	}
}

/// @brief 現在の移動速度を取得する
/// @return
float Player::GetCurrentMoveSpeed() const
{
	// 構え中は移動速度を半分にする
	// Character側で速度補間しているため、通常速度↔構え速度の切り替えも補間される
	float moveSpeed = isStance_ ? (kNormalMoveSpeed * kStanceMoveSpeedMultiplier) : kNormalMoveSpeed;

	// ダッシュ中は移動速度を2倍にする
	if (isDash_)
	{
		moveSpeed *= kDashSpeedMultiplier;
	}

	return moveSpeed;
}

/// @brief カメラのY回転を取得する
/// @return
float Player::GetCameraYaw() const
{
	if (Engine::Camera3DData::Param* cameraParam = GrowthEngine::GetInstance()->GetCamera3DParam("MainCamera"))
	{
		return cameraParam->transform.rotate.y;
	}

	return 0.0f;
}

/// @brief 描画処理
void Player::Draw()
{
	assert(model_);

	// モデルを描画する
	model_->Draw();
}