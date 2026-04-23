#include "Player.h"

#include <cmath>

namespace
{
	// 通常時の移動速度[m/s]
	constexpr float kNormalMoveSpeed = 3.0f;

	// 構え時の移動速度倍率
	constexpr float kStanceMoveSpeedMultiplier = 1.0f;

	// ダッシュ時の移動速度倍率
	constexpr float kDashSpeedMultiplier = 3.0f;

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


	comboAttacks_.push_back(std::make_unique<ComboAttack>(this, MotionManager::GetInstance()->GetMotion(MotionType::Attack, 0), 0.5f, 9.0f, 0.01f, 0.07f, 0.2f, 0.5f));
	comboAttacks_.push_back(std::make_unique<ComboAttack>(this, MotionManager::GetInstance()->GetMotion(MotionType::Attack, 1), 0.5f, 9.0f, 0.01f, 0.07f, 0.2f, 0.5f));
	comboAttacks_.push_back(std::make_unique<ComboAttack>(this, MotionManager::GetInstance()->GetMotion(MotionType::Attack, 2), 0.5f, 9.0f, 0.01f, 0.07f, 0.2f, 0.5f));
	comboAttacks_.push_back(std::make_unique<ComboAttack>(this, MotionManager::GetInstance()->GetMotion(MotionType::Attack, 3), 0.5f, 9.0f, 0.01f, 0.07f, 0.2f, 0.5f));

	auto comboLight1 = comboAttacks_[0].get();
	auto comboLight2 = comboAttacks_[1].get();
	auto comboLight3 = comboAttacks_[2].get();
	auto comboLight4 = comboAttacks_[3].get();

	comboLight1->SetNextLightAttack(comboLight2);
	comboLight2->SetNextLightAttack(comboLight3);
	comboLight3->SetNextLightAttack(comboLight4);
}

/// @brief 更新処理
void Player::Update()
{
	// 攻撃の更新処理
	UpdateAttack();

	// アクションの更新処理
	ActionUpdate();

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

/// @brief 攻撃処理を更新する
void Player::UpdateAttack()
{
	// デルタタイムの取得
	const float deltaTime = GrowthEngine::GetInstance()->GetDeltaTime();

	// 攻撃入力のバッファ時間を減らす
	if (attackInputBufferTime_ > 0.0f)
	{
		attackInputBufferTime_ -= deltaTime;

		// バッファ時間が0以下になったらバッファされた攻撃入力を消す
		if (attackInputBufferTime_ <= 0.0f)
			bufferedAttackInput_ = AttackInputType::None;
	}

	// 攻撃ボタンの入力を受け付け、条件を満たす場合のみバッファに保存
	if (inputLightAttack_ && inputLightAttack_->IsInput())
	{
		// 攻撃していない(待機・移動中) または、現在の攻撃から「弱」へ派生できる場合のみ
		if (!IsAttack() || currentAttack_->HasNextAttack(AttackInputType::Light))
		{
			bufferedAttackInput_ = AttackInputType::Light;
			attackInputBufferTime_ = 0.2f; // バッファ有効時間
		}
	} 
	else if (inputHeavyAttack_ && inputHeavyAttack_->IsInput())
	{
		// 攻撃していない または、現在の攻撃から「強」へ派生できる場合のみ
		if (!IsAttack() || currentAttack_->HasNextAttack(AttackInputType::Heavy))
		{
			bufferedAttackInput_ = AttackInputType::Heavy;
			attackInputBufferTime_ = 0.2f;
		}
	}

	// 現在攻撃中でなく、かつバッファされた攻撃入力がある場合は攻撃を開始する
	if (!IsAttack() && bufferedAttackInput_ != AttackInputType::None)
	{
		if (bufferedAttackInput_ == AttackInputType::Light)
		{
			// 初段の弱攻撃アクションのポインタを渡して実行
			// (※事前に初期化で生成しておいた ComboAttack インスタンスを使用)
			auto comboLight1 = comboAttacks_[0].get();
			comboLight1->Exec();
		}
		ConsumeBufferedAttackInput();
	}
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