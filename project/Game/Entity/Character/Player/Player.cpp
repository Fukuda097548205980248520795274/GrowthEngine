#include "Player.h"
#include "Entity/Weapon/Weapon.h"

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
	characterTag_ = CharacterTag::Player;

	// 戦闘スタイルを指定する
	currentStyle_ = FightStyle::Tempest;
}

/// @brief 初期化
void Player::Initialize(Weapon* baton)
{
	assert(model_);
	assert(baton);

	// 引数を受け取る
	baton_ = baton;

	// 武器の有効状態をスタイルに応じて設定する
	if (currentStyle_ == FightStyle::Hammer)
	{
		hStanceMotion_ = motionManager_->GetMotion(MotionType::Stance, "Gekitetu");
		baton_->SetActive(true);
		GrabWeapon(baton_);

		// 撃鉄スタイルのBGMを再生する
		soundManager_->bgmStyleGekitetu_->Play();
	}
	else if (currentStyle_ == FightStyle::Tempest)
	{
		hStanceMotion_ = motionManager_->GetMotion(MotionType::Stance, "Senran");
		baton_->SetActive(false);
		ReleaseWeapon();

		// 旋嵐スタイルのBGMを再生する
		soundManager_->bgmStyleSenran_->Play();
	}


	// モデルをワールドトランスフォームの子にする
	model_->SetParent(worldTransform_.get());


	// 移動入力の生成
	inputMove_ = std::make_unique<InputGamepadLeftStick>("Player_Move", InputState::Press, 0, Vector2(0.0f, 0.0f), 0.5f);

	// ダッシュ入力の生成
	inputDash_ = std::make_unique<InputGamepadButton>("Player_Dash", InputState::Trigger, 0, XINPUT_GAMEPAD_RIGHT_SHOULDER);

	// 回避入力の生成
	inputAvoid_ = std::make_unique<InputGamepadButton>("Player_Avoid", InputState::Trigger, 0, XINPUT_GAMEPAD_A);

	// 弱攻撃入力の生成
	inputLightAttack_ = std::make_unique<InputGamepadButton>("Player_LightAttack", InputState::Trigger, 0, XINPUT_GAMEPAD_X);

	// 強攻撃入力の生成
	inputHeavyAttack_ = std::make_unique<InputGamepadButton>("Player_HeavyAttack", InputState::Trigger, 0, XINPUT_GAMEPAD_Y);

	// つかみ入力の生成
	inputGrab_ = std::make_unique<InputGamepadButton>("Player_Grab", InputState::Trigger, 0, XINPUT_GAMEPAD_B);

	// 防御入力の生成
	inputGuard_ = std::make_unique<InputGamepadButton>("Player_Guard", InputState::Press, 0, XINPUT_GAMEPAD_LEFT_SHOULDER);

	// 掴まれ解き入力の生成
	inputEscapeMash_ = std::make_unique<InputGamepadButton>("Player_EscapeMash", InputState::Trigger, 0, XINPUT_GAMEPAD_A);

	// ダウン後起き上がり入力の生成
	inputGetUp_ = std::make_unique<InputGamepadButton>("Player_GetUp", InputState::Trigger, 0, XINPUT_GAMEPAD_A);

	// スタイルチェンジ入力の生成
	inputStyleChange_ = std::make_unique<InputGamepadButton>("Player_StyleChange", InputState::Trigger, 0, XINPUT_GAMEPAD_DPAD_DOWN);


	// キーの前移動入力の生成
	keyFrontMove_ = std::make_unique<InputKey>("Player_KeyFrontMove", InputState::Press, DIK_W);

	// キーの後移動入力の生成
	keyBackMove_ = std::make_unique<InputKey>("Player_KeyBackMove", InputState::Press, DIK_S);

	// キーの左移動入力の生成
	keyLeftMove_ = std::make_unique<InputKey>("Player_KeyLeftMove", InputState::Press, DIK_A);

	// キーの右移動入力の生成
	keyRightMove_ = std::make_unique<InputKey>("Player_KeyRightMove", InputState::Press, DIK_D);


	// 1段目の攻撃
	CombAttackInitData attack1Data;
	attack1Data.hAttackMotion = MotionManager::GetInstance()->GetMotion(MotionType::Attack, "Player_Combo_1");
	attack1Data.attackTime = 0.5f;
	attack1Data.moveSpeed = 9.0f;
	attack1Data.moveStartTime = 0.01f;
	attack1Data.moveEndTime = 0.07f;
	attack1Data.cancelStartTime = 0.2f;
	attack1Data.cancelEndTime = 0.5f;
	attack1Data.hitDefinitions.resize(1);
	attack1Data.hitDefinitions[0].jointType = JointType::HandR;
	attack1Data.hitDefinitions[0].startTime = 0.1f;
	attack1Data.hitDefinitions[0].endTime = 0.4f;
	attack1Data.hitDefinitions[0].damage = 1;
	attack1Data.hitDefinitions[0].damageReaction = DamageReaction::LightStagger;
	attack1Data.hitDefinitions[0].knockback = 1.0f;
	attack1Data.hitDefinitions[0].knockbackDirection = Vector3(0.0f, 0.0f, 1.0f);

	// 2段目の攻撃
	CombAttackInitData attack2Data;
	attack2Data.hAttackMotion = MotionManager::GetInstance()->GetMotion(MotionType::Attack, "Player_Combo_2");
	attack2Data.attackTime = 0.5f;
	attack2Data.moveSpeed = 9.0f;
	attack2Data.moveStartTime = 0.01f;
	attack2Data.moveEndTime = 0.07f;
	attack2Data.cancelStartTime = 0.2f;
	attack2Data.cancelEndTime = 0.5f;
	attack2Data.hitDefinitions.resize(1);
	attack2Data.hitDefinitions[0].jointType = JointType::HandL;
	attack2Data.hitDefinitions[0].startTime = 0.1f;
	attack2Data.hitDefinitions[0].endTime = 0.4f;
	attack2Data.hitDefinitions[0].damage = 1;
	attack2Data.hitDefinitions[0].damageReaction = DamageReaction::LightStagger;
	attack2Data.hitDefinitions[0].knockback = 1.0f;
	attack2Data.hitDefinitions[0].knockbackDirection = Vector3(0.0f, 0.0f, 1.0f);

	// 3段目の攻撃
	CombAttackInitData attack3Data;
	attack3Data.hAttackMotion = MotionManager::GetInstance()->GetMotion(MotionType::Attack, "Player_Combo_3");
	attack3Data.attackTime = 0.5f;
	attack3Data.moveSpeed = 9.0f;
	attack3Data.moveStartTime = 0.01f;
	attack3Data.moveEndTime = 0.07f;
	attack3Data.cancelStartTime = 0.2f;
	attack3Data.cancelEndTime = 0.5f;
	attack3Data.hitDefinitions.resize(1);
	attack3Data.hitDefinitions[0].jointType = JointType::HandR;
	attack3Data.hitDefinitions[0].startTime = 0.1f;
	attack3Data.hitDefinitions[0].endTime = 0.4f;
	attack3Data.hitDefinitions[0].damage = 1;
	attack3Data.hitDefinitions[0].damageReaction = DamageReaction::LightStagger;
	attack3Data.hitDefinitions[0].knockback = 1.0f;
	attack3Data.hitDefinitions[0].knockbackDirection = Vector3(0.0f, 1.0f, 1.0f);

	// 4段目の攻撃
	CombAttackInitData attack4Data;
	attack4Data.hAttackMotion = MotionManager::GetInstance()->GetMotion(MotionType::Attack, "Player_Combo_4");
	attack4Data.attackTime = 0.5f;
	attack4Data.moveSpeed = 9.0f;
	attack4Data.moveStartTime = 0.01f;
	attack4Data.moveEndTime = 0.07f;
	attack4Data.cancelStartTime = 0.2f;
	attack4Data.cancelEndTime = 0.5f;
	attack4Data.hitDefinitions.resize(1);
	attack4Data.hitDefinitions[0].jointType = JointType::FootL;
	attack4Data.hitDefinitions[0].startTime = 0.1f;
	attack4Data.hitDefinitions[0].endTime = 0.4f;
	attack4Data.hitDefinitions[0].damage = 1;
	attack4Data.hitDefinitions[0].damageReaction = DamageReaction::Down;
	attack4Data.hitDefinitions[0].knockback = 8.0f;
	attack4Data.hitDefinitions[0].knockbackDirection = Vector3(0.0f, 1.0f, 1.0f);

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

	// スタイルチェンジ開始時の処理
	OnStyleChanged(currentStyle_);
}

/// @brief 更新処理
void Player::Update()
{
	// 更新が無効なら何もしない
	if (!updateEnabled_)return;

	// 更新処理開始前のリセット
	StartUpdate();

	if(IsJustAvoided())
	{
		int a = 0;
	}

	// 動けない状態かどうか
	bool isIncapacitatedState = IsIncapacitated();

	// カメラによるターゲットの更新
	UpdateTargetByCamera();
	isOperationCamera_ = false;

	// 動ける状態なら、攻撃やスタイルチェンジなどの入力を受け付けて、状態の更新や移動処理を行う
	if (!isIncapacitatedState)
	{
		// スタイルチェンジ入力があればスタイルチェンジ処理を行う
		StyleChange();

		// 攻撃の更新処理
		UpdateAttack();

		// プレイヤーは予備動作はないので、攻撃中が攻撃動作中と同じ扱いになる
		isInAttackSequence_ = IsAttack();

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

			// アクションの更新処理
			ActionUpdate();
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
			// 入力方向に応じた回避方向を計算して回避を開始する
			Vector2 avoidDirection = GetAvoidDirection(moveInputDirection, hasMoveInput, GetCameraYaw());
			StartAvoid(Vector3(avoidDirection.x, 0.0f, avoidDirection.y), 1.5f, 0.3f);

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
	}

	// プレイヤーは予備動作はないので、攻撃中が攻撃動作中と同じ扱いになる
	isInAttackSequence_ = IsAttack();

	// アクションの更新処理
	ActionUpdate();

	// 動けない状態なら、攻撃やスタイルチェンジなどの入力は受け付けず、状態の更新と描画のみ行う
	if (isIncapacitatedState)
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

	// 攻撃エフェクトの描画
	if (attackTrail_)attackTrail_->Draw();
}

/// @brief 攻撃処理を更新する
void Player::UpdateAttack()
{
	// デルタタイムの取得
	const float deltaTime = GrowthEngine::GetInstance()->GetDeltaTime() * GrowthEngine::GetInstance()->GetTimeScale();

	// 怯み状態、または「つかまれている状態」なら攻撃の更新は行わない
	if (IsDamageReaction() || IsGrabbing() || IsGrabbed() || IsDown() || IsStyleChanging())
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
			// 1段目の攻撃を実行
			auto comboLight1 = comboAttacks_[0].get();
			comboLight1->Exec();
		}

		// バッファされた攻撃入力を消す
		ConsumeBufferedAttackInput();
	}
}

/// @brief 構え状態を更新する
void Player::UpdateStanceState()
{
	// 怯み状態、動けない状態は構え状態にならない
	if(IsGrabbing() || IsIncapacitated() || isDash_ || !GetLockOnTarget())
	{
		isStance_ = false;
		return;
	}

	isStance_ = true;
}

/// @brief 連続回避を試行する
/// @param moveInputDirection
/// @param hasMoveInput
/// @param cameraYaw
void Player::ReserveNextAvoid(const Vector2& moveInputDirection, bool hasMoveInput, float cameraYaw)
{
	// 回避中でない場合は何もしない
	if (!isAvoid_)
	{
		return;
	}

	// 最大連続回避回数に達している場合は連続回避できない
	if (currentAvoidCount_ >= maxConsecutiveAvoidCount_)
	{
		return;
	}

	// 現在位置から次の連続回避を即時開始する
	++currentAvoidCount_;

	Vector2 avoidDirection = GetAvoidDirection(moveInputDirection, hasMoveInput, cameraYaw);
	StartAvoid(Vector3(avoidDirection.x, 0.0f, avoidDirection.y), 1.5f, 0.3f);
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
	// 怯み状態、または構え状態、または「つかまれている状態」、または攻撃中、またはダウン中、または空中にいる状態、またはスタイルチェンジ中ならダッシュにならない
	// ダッシュ中に構えた場合もダッシュを解除する
	if (IsGrabbing() || IsIncapacitated())
	{
		isDash_ = false;
		return;
	}

	// ダッシュ入力があって、移動入力もあって、ダッシュ中でない場合はダッシュを開始する
	if (inputDash_ && inputDash_->IsInput() && hasMoveInput && !isDash_)
	{
		isDash_ = true;

		// ダッシュ開始時にスローモーションを開始する
		GrowthEngine::GetInstance()->StartSlowMotion(0.1f, 0.1f);
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
	// 怯み状態、または構え状態、または「つかまれている状態」、または攻撃中、またはダウン中、または空中にいる状態、またはスタイルチェンジ中なら移動速度は0
	if (isGuard_ || IsIncapacitated())
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
	// 地面に接地していない場合は起き上がれない
	if (!IsGrounded())return false;

	// 起き上がりボタンが入力されたら true を返す
	if (inputGetUp_ && inputGetUp_->IsInput())
	{
		return true;
	}

	return false;
}

/// @brief スタイルチェンジ開始時の処理
void Player::StyleChangeStart()
{
	// スタイルチェンジ開始時にスローモーションを開始する
	GrowthEngine::GetInstance()->StartSlowMotion(0.25f, 0.5f);

	switch (nextStyle_)
	{
		// 旋嵐スタイル（武器なし・他の武器を拾える）
	case FightStyle::Tempest:

		// 旋嵐スタイルのBGMを再生する
		soundManager_->bgmStyleSenran_->Play();

		// 撃鉄bgmが流れていたら止める
		if (soundManager_->bgmStyleGekitetu_->IsPlaying())
			soundManager_->bgmStyleGekitetu_->Stop();

		// 現在持っているのが「警棒」だった場合
		if (weapon_ != nullptr && weapon_ == baton_)
		{
			// 警棒の描画や当たり判定を無効にする
			baton_->SetActive(false);

			// 警棒を手放す
			ReleaseWeapon();
		}

		break;

		// 撃鉄スタイル（警棒を装備）
	case FightStyle::Hammer:

		// 撃鉄スタイルのBGMを再生する
		soundManager_->bgmStyleGekitetu_->Play();

		// 旋嵐bgmが流れていたら止める
		if (soundManager_->bgmStyleSenran_->IsPlaying())
			soundManager_->bgmStyleSenran_->Stop();

		// もし旋嵐スタイル中にフィールドの「別の武器」を拾って持っていたら、落とす
		if (weapon_ != nullptr && weapon_ != baton_)
		{
			ReleaseWeapon(); // ※キャラクタークラスの既存関数で手放す
		}

		// 警棒を再度装備する
		GrabWeapon(baton_);

		// 警棒の描画や当たり判定を有効にする
		if (baton_)
		{
			baton_->SetActive(true);
		}
		break;
	}

	// 既定のスタイルチェンジ処理
	Character::StyleChangeStart();
}

/// @brief スタイルが変化したときの処理
/// @param newStyle 
void Player::OnStyleChanged(FightStyle newStyle)
{
	switch (newStyle)
	{
		// 旋嵐スタイル
	case FightStyle::Tempest:
		hStanceMotion_ = motionManager_->GetMotion(MotionType::Stance, "Senran");
		if (attackTrail_)attackTrail_->param_->color = Vector4(0.5f, 0.5f, 1.0f, 1.0f);
		break;

		// 撃鉄スタイル
	case FightStyle::Hammer:
		hStanceMotion_ = motionManager_->GetMotion(MotionType::Stance, "Gekitetu");
		if (attackTrail_)attackTrail_->param_->color = Vector4(1.0f, 0.5f, 0.5f, 1.0f);
		break;
	}

	// 既定のスタイルチェンジ処理
	Character::OnStyleChanged(newStyle);
}

/// @brief スタイルチェンジ処理
void Player::StyleChange()
{
	// スタイルチェンジ入力があればスタイルを切り替える
	if (inputStyleChange_ && inputStyleChange_->IsInput())
	{
		if (currentStyle_ == FightStyle::Tempest) { StartStyleChange(FightStyle::Hammer); }
		if (currentStyle_ == FightStyle::Hammer) { StartStyleChange(FightStyle::Tempest); }
	}
}

/// @brief カメラによるターゲットの更新
void Player::UpdateTargetByCamera()
{
	// カメラ操作中でない、または攻撃中、またはつかまれている状態、または動けない状態ならターゲットの更新を行わない
	if (!isOperationCamera_ || IsGrabbing() || IsGrabbed() || IsIncapacitated())return;

	// カメラのY回転を取得する
	float currentYaw = GetCameraYaw();

	// 前フレームからのカメラのY回転の変化量を計算する
	float deltaYaw = std::abs(currentYaw - prevCameraYaw_);

	// 前フレームのカメラY回転を保存する
	prevCameraYaw_ = currentYaw;

	// カメラのY回転に基づいて、ターゲットの前方向を計算する
	Vector3 cameraForward = Vector3(std::sin(currentYaw), 0.0f, std::cos(currentYaw));

	// ターゲット候補の中から、カメラ前方向に最も近いターゲットを選択する
	Character* bestTarget = nullptr;
	float maxDot = -1.0f;

	// 自身の位置を取得する
	Vector3 pos = GetWorldPosition();

	for (Character* target : Character::characters_)
	{
		// ターゲットが自分自身、または死んでいる、または同じ陣営ならスキップする
		if (target == this || target->IsDead())continue;
		if (IsPlayerSide() == target->IsPlayerSide())continue;

		// ターゲットの方向
		Vector3 toTarget = target->GetWorldPosition() - pos;
		toTarget.y = 0.0f; // Y軸の高さは無視する

		// ターゲット方向を正規化
		toTarget = toTarget.Normalize();

		// カメラ前方向とターゲット方向の内積を計算する
		float dot = cameraForward.x * toTarget.x + cameraForward.z * toTarget.z;
		if (dot > 0.0f && dot > maxDot)
		{
			maxDot = dot;
			bestTarget = target;
		}
	}

	// 最もカメラ前方向に近いターゲットをロックオンする
	if (bestTarget != nullptr && bestTarget != lockOnTarget_)
		lockOnTarget_ = bestTarget;
}

/// @brief 防御状態を更新する
void Player::UpdateGuardState()
{
	// 怯み状態、または構え状態、または「つかまれている状態」、または攻撃中、またはダウン中、または空中にいる状態、またはスタイルチェンジ中なら防御状態にならない
	if(IsGrabbing() || IsAttack() || IsIncapacitated())
	{
		SetGuard(false);
		return;
	}

	// 防御入力中は防御フラグを立て、離したらフラグを下ろす
	if(inputGuard_ && inputGuard_->IsInput())
	{
		SetGuard(true);
	}
	else
	{
		SetGuard(false);
	}
}