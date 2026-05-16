#include "Player.h"
#include "Action/Attack/SpinThrowAttack/SpinThrowAttack.h"

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

	// つかみ入力の生成
	inputGrab_ = std::make_unique<InputGamepadButton>("Player_Grab", InputState::Trigger, 0, XINPUT_GAMEPAD_B);

	// 構え入力の生成
	inputStance_ = std::make_unique<InputGamepadButton>("Player_Stance", InputState::Press, 0, XINPUT_GAMEPAD_RIGHT_SHOULDER);

	// 防御入力の生成
	inputGuard_ = std::make_unique<InputGamepadButton>("Player_Guard", InputState::Press, 0, XINPUT_GAMEPAD_LEFT_SHOULDER);

	// 掴まれ解き入力の生成
	inputEscapeMash_ = std::make_unique<InputGamepadButton>("Player_EscapeMash", InputState::Trigger, 0, XINPUT_GAMEPAD_A);

	// ダウン後起き上がり入力の生成
	inputGetUp_ = std::make_unique<InputGamepadButton>("Player_GetUp", InputState::Trigger, 0, XINPUT_GAMEPAD_A);


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


	// 1段目の攻撃
	CombAttackInitData attack1Data;
	attack1Data.hAttackMotion = MotionManager::GetInstance()->GetMotion(MotionType::Attack, "Player_Combo_1");
	attack1Data.attackTime = 0.5f;
	attack1Data.moveSpeed = 9.0f;
	attack1Data.moveStartTime = 0.01f;
	attack1Data.moveEndTime = 0.07f;
	attack1Data.cancelStartTime = 0.2f;
	attack1Data.cancelEndTime = 0.5f;
	attack1Data.jointType = JointType::HandR;
	attack1Data.hitboxStartTime = 0.1f;
	attack1Data.hitboxEndTime = 0.4f;
	attack1Data.damage = 1;
	attack1Data.damageReaction = DamageReaction::LightStagger;
	attack1Data.knockback = 0.1f;
	attack1Data.knockbackDirection = Vector3(0.0f, 0.0f, 1.0f);

	// 2段目の攻撃
	CombAttackInitData attack2Data;
	attack2Data.hAttackMotion = MotionManager::GetInstance()->GetMotion(MotionType::Attack, "Player_Combo_2");
	attack2Data.attackTime = 0.5f;
	attack2Data.moveSpeed = 9.0f;
	attack2Data.moveStartTime = 0.01f;
	attack2Data.moveEndTime = 0.07f;
	attack2Data.cancelStartTime = 0.2f;
	attack2Data.cancelEndTime = 0.5f;
	attack2Data.jointType = JointType::HandL;
	attack2Data.hitboxStartTime = 0.1f;
	attack2Data.hitboxEndTime = 0.4f;
	attack2Data.damage = 1;
	attack2Data.damageReaction = DamageReaction::LightStagger;
	attack2Data.knockback = 0.1f;
	attack2Data.knockbackDirection = Vector3(0.0f, 0.0f, 1.0f);

	// 3段目の攻撃
	CombAttackInitData attack3Data;
	attack3Data.hAttackMotion = MotionManager::GetInstance()->GetMotion(MotionType::Attack, "Player_Combo_3");
	attack3Data.attackTime = 0.5f;
	attack3Data.moveSpeed = 9.0f;
	attack3Data.moveStartTime = 0.01f;
	attack3Data.moveEndTime = 0.07f;
	attack3Data.cancelStartTime = 0.2f;
	attack3Data.cancelEndTime = 0.5f;
	attack3Data.jointType = JointType::HandR;
	attack3Data.hitboxStartTime = 0.1f;
	attack3Data.hitboxEndTime = 0.4f;
	attack3Data.damage = 1;
	attack3Data.damageReaction = DamageReaction::LightStagger;
	attack3Data.knockback = 0.1f;
	attack3Data.knockbackDirection = Vector3(0.0f, 1.0f, 1.0f);

	// 4段目の攻撃
	CombAttackInitData attack4Data;
	attack4Data.hAttackMotion = MotionManager::GetInstance()->GetMotion(MotionType::Attack, "Player_Combo_4");
	attack4Data.attackTime = 0.5f;
	attack4Data.moveSpeed = 9.0f;
	attack4Data.moveStartTime = 0.01f;
	attack4Data.moveEndTime = 0.07f;
	attack4Data.cancelStartTime = 0.2f;
	attack4Data.cancelEndTime = 0.5f;
	attack4Data.jointType = JointType::FootL;
	attack4Data.hitboxStartTime = 0.1f;
	attack4Data.hitboxEndTime = 0.4f;
	attack4Data.damage = 1;
	attack4Data.damageReaction = DamageReaction::DownFalling;
	attack4Data.knockback = 0.5f;
	attack4Data.knockbackDirection = Vector3(1.0f, 1.0f, 1.0f);

	comboAttacks_.push_back(std::make_unique<ComboAttack>(this, attack1Data));
	comboAttacks_.push_back(std::make_unique<ComboAttack>(this, attack2Data));
	comboAttacks_.push_back(std::make_unique<ComboAttack>(this, attack3Data));
	comboAttacks_.push_back(std::make_unique<ComboAttack>(this, attack4Data));

	auto comboLight1 = comboAttacks_[0].get();
	auto comboLight2 = comboAttacks_[1].get();
	auto comboLight3 = comboAttacks_[2].get();
	auto comboLight4 = comboAttacks_[3].get();

	comboLight1->SetNextLightAttack(comboLight2);
	comboLight2->SetNextLightAttack(comboLight3);
	comboLight3->SetNextLightAttack(comboLight4);


	// 掴み攻撃
	GrabAttackInitData grabData;
	grabData.hAttackMotion = motionManager_->GetMotion(MotionType::Attack, "Player_Combo_1");
	grabData.attackTime = 1.0f;
	grabData.moveSpeed = 3.0f;
	grabData.moveStartTime = 0.1f;
	grabData.moveEndTime = 0.3f;
	grabData.jointType = JointType::HandR;
	grabData.hitboxStartTime = 0.15f;
	grabData.hitboxEndTime = 0.35f;
	grabData.grabTime = 3.0f;

	grabAttack_ = std::make_unique<GrabAttack>(this, grabData);
}

/// @brief 更新処理
void Player::Update()
{
	// 更新処理開始前のリセット
	StartUpdate();

	// 怯み状態、つかみ状態、つかまれ状態、ダウン状態なら移動や攻撃の更新は行わず、つかまれ解き入力の受付やダウンからの起き上がり条件のチェックのみ行う
	if (IsDamageReaction() || IsGrabbed() || IsDown())
	{
		// つかまれている状態なら、つかまれ解き入力を受け付けて、入力があればつかまれ解きの処理を行う
		if (IsGrabbed())
		{
			bool isStruggleInput = false;
			if (inputEscapeMash_ && inputEscapeMash_->IsInput()) isStruggleInput = true;

			if (isStruggleInput)
			{
				grabbedTimer_ += 0.2f;
			}
		}

		Character::Update();
		return;
	}

	// 攻撃の更新処理
	UpdateAttack();

	// アクションの更新処理
	ActionUpdate();

	// 防御状態を更新する
	UpdateGuardState();

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

/// @brief 描画処理
void Player::Draw()
{
	assert(model_);

	// モデルを描画する
	model_->Draw();
}

/// @brief 攻撃処理を更新する
void Player::UpdateAttack()
{
	// デルタタイムの取得
	const float deltaTime = GrowthEngine::GetInstance()->GetDeltaTime();

	// 怯み状態、または「つかまれている状態」なら攻撃の更新は行わない
	if (IsDamageReaction() || IsGrabbing() || IsGrabbed() || IsDown())
		return;

	// つかみ攻撃の入力があって、現在攻撃中でない場合はつかみ攻撃を実行する
	if (inputGrab_ && inputGrab_->IsInput() && !IsAttack())
	{
		// つかみ攻撃を実行
		grabAttack_->Exec();
		return;
	}

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
	// 怯み状態、または「つかまれている状態」、または攻撃中、またはダウン状態、または地面にいない状態なら構え状態にならない
	if(IsGrabbing() || IsGrabbed() || IsDown() || !IsGrounded())
	{
		isStance_ = false;
		return;
	}

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
	// 怯み状態、または構え状態、または「つかまれている状態」ならダッシュ状態にならない
	// ダッシュ中に構えた場合もダッシュを解除する
	if (isStance_ || IsGrabbing() || IsDamageReaction() || IsGrabbed() || IsDown() || !IsGrounded())
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
	// 怯み状態、または構え状態、または「つかまれている状態」、または攻撃中は移動速度が0になる
	if (isGuard_ || IsGrabbed() || IsDown() || IsDamageReaction() || IsAttack())
		return 0.0f;

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

/// @brief ダウン後起き上がり条件を満たしているかどうか
/// @return 
bool Player::CheckGetUpCondition()
{
	// 起き上がりボタンが入力されたら true を返す
	if (inputGetUp_ && inputGetUp_->IsInput())
	{
		return true;
	}

	return false;
}

/// @brief 防御状態を更新する
void Player::UpdateGuardState()
{
	// 怯み状態、または構え状態、または「つかまれている状態」、または攻撃中、またはダウン状態、または地面にいない状態なら防御状態にならない
	if(IsDamageReaction() || IsGrabbing() || IsGrabbed() || IsDown() || IsAttack() || !IsGrounded())
	{
		isGuard_ = false;
		return;
	}

	// 防御入力中は防御フラグを立て、離したらフラグを下ろす
	if(inputGuard_ && inputGuard_->IsInput())
	{
		isGuard_ = true;
	}
	else
	{
		isGuard_ = false;
	}
}

/// @brief 回転掴み攻撃の更新処理
void Player::SpinThrowAttackUpdate()
{
	if (!IsSpinThrowAttacking()) return;

	auto spinThrowAttack = static_cast<SpinThrowAttack*>(currentAttack_);

	// プレイヤーの移動入力を取得
	bool hasMoveInput = false;
	Vector2 inputDir = GetMoveInputDirection(hasMoveInput);

	// 入力がある場合のみ、その方向に回転移動・投げ方向を更新する
	if (hasMoveInput)
	{
		// カメラ基準の入力をワールド方向(XZ平面の3Dベクトル)へ変換する
		Vector2 worldDir2D = ToWorldMoveDirectionFromCamera(inputDir, GetCameraYaw());
		Vector3 worldDir3D(worldDir2D.x, 0.0f, worldDir2D.y);

		// 攻撃クラスに入力方向を伝える
		spinThrowAttack->SetInputDirection(worldDir3D);
	}
	else
	{
		// 入力がない場合は「今向いている方向」にするため、基底クラス（Character）の処理を呼び出す
		// これにより、スティックを離したときはその場で現在の向きのまま回り、その方向に投げ飛ばします
		Character::SpinThrowAttackUpdate();
	}
}