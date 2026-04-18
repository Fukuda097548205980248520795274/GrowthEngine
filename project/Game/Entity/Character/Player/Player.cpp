#include "Player.h"

#include <cmath>

namespace
{
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
    // 通常時の移動速度[m/s]
	constexpr float kNormalMoveSpeed = 6.0f;
	constexpr float kStanceMoveSpeed = kNormalMoveSpeed * 0.5f;
	constexpr float kDashSpeedMultiplier = 2.0f;

	// カメラのY回転を取得する
	float cameraYaw = 0.0f;
	if (Engine::Camera3DData::Param* cameraParam = GrowthEngine::GetInstance()->GetCamera3DParam("MainCamera"))
	{
		cameraYaw = cameraParam->transform.rotate.y;
	}

	// 構え入力中は構えフラグを立て、離したらフラグを下ろす
	const bool isGamepadStance = (inputStance_ && inputStance_->IsInput());
	const bool isKeyStance = (keyStance_ && keyStance_->IsInput());
	isStance_ = (isGamepadStance || isKeyStance);

	// 構え中は移動速度を半分にする
	// Character側で速度補間しているため、通常速度↔構え速度の切り替えも補間される
    float moveSpeed = isStance_ ? kStanceMoveSpeed : kNormalMoveSpeed;

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
	bool hasMoveInput = false;
	Vector2 moveInputDirection = Vector2(0.0f, 0.0f);
	if (keyMoveDirection.Length() > 0.0f)
	{
		hasMoveInput = true;
		moveInputDirection = keyMoveDirection.Normalize();
	}
	else if (inputMove_ && inputMove_->param_ && inputMove_->IsInput())
	{
		// 左スティックの入力を取得する
		const Vector2 stick = GrowthEngine::GetInstance()->GetGamepadLeftStick(inputMove_->param_->controller);
		if (stick.Length() > 0.0f)
		{
			hasMoveInput = true;
			moveInputDirection = stick.Normalize();
		}
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

	// ダッシュ中は移動速度を2倍にする
	if (isDash_)
	{
		moveSpeed *= kDashSpeedMultiplier;
	}

	if (hasMoveInput)
	{
		// カメラ基準の入力方向をワールド方向へ変換する
		const Vector2 worldMoveDirection = ToWorldMoveDirectionFromCamera(moveInputDirection, cameraYaw);
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

/// @brief 描画処理
void Player::Draw()
{
	assert(model_);

	// モデルを描画する
	model_->Draw();
}