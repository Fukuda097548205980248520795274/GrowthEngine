#include "Player.h"
#include "Entity/Weapon/Weapon.h"
#include "HUD/HP/HP.h"

#include "ComboTree/ComboTreeEditor/ComboTreeFactory/ComboTreeFactory.h"
#include "comboTree/ComboTreeEditor/ComboTreeEditor.h"

#include "CharacterStateMachine/CharacterState/CharacterStateAvoid/CharacterStateAvoid.h"

#include <cmath>

namespace
{
	// 通常時の移動速度[m/s]
	constexpr float kNormalMoveSpeed = 3.0f;

	// 構え時の移動速度倍率
	constexpr float kStanceMoveSpeedMultiplier = 1.0f;

	// ダッシュ時の移動速度倍率
	constexpr float kDashSpeedMultiplier = 3.0f;
}

/// @brief コンストラクタ
/// @param initData 
Player::Player() : Character()
{
	// キャラクターのリストに追加する
	characters_.push_back(this);

	// タグを指定する
	characterTag_ = CharacterTag::Player;

	// 戦闘スタイルを指定する
	currentStyle_ = FightStyle::Tempest;
}

/// @brief 初期化
void Player::Initialize(const CharacterInitData& initData, Weapon* baton)
{
	assert(baton);

	// 初期化データを設定する
	SetInitData(initData);

	// ステートを初期化する
	stateMachine_->ChangeState("None");

	// 入力コントローラーを作成する
	inputController_ = std::make_unique<PlayerInputController>();
	inputController_->Initialize();

	// 引数を受け取る
	baton_ = baton;

	// 武器の有効状態をスタイルに応じて設定する
	if (currentStyle_ == FightStyle::Hammer)
	{
		hStanceMotion_ = motionManager_->GetMotion(MotionType::Stance, "Standard");
		baton_->SetActive(true);
		GrabWeapon(baton_);
	}
	else if (currentStyle_ == FightStyle::Tempest)
	{
		hStanceMotion_ = motionManager_->GetMotion(MotionType::Stance, "Fighter");
		baton_->SetActive(false);
		ReleaseWeapon();
	}

	// スタイルチェンジ開始時の処理
	OnStyleChanged(currentStyle_);
}

/// @brief 更新処理
void Player::Update()
{
	// 更新処理開始前のリセット
	StartUpdate();

	// カットシーン中は移動を停止して、基底クラスの更新処理のみ行う
	if (Character::IsCutsceneActive() || !updateEnabled_)
	{
		// ステートをNoneに変更する
		stateMachine_->ChangeState("None");
		MoveStop();

		Character::Update();
		return;
	}

	// ゲームが終了している場合は、基底クラスの更新処理のみ行う
	if (Character::IsGameFinished() || Character::IsGameIntro())
	{
		Character::Update();
		return;
	}

	// 動けない状態かどうか
	bool isIncapacitatedState = IsIncapacitated();

	// カメラによるターゲットの更新
	UpdateTargetByCamera();
	isOperationCamera_ = false;

	// コンボツリーの変更が予約されている場合、攻撃が完全に終了している状態であればコンボツリーを変更する
	if (isChangeComboTree_)
	{
		// 攻撃が完全に終了している状態
		if (!currentAttack_)
		{
			currentComboTreeX_ = nextComboTreeX_;
			currentComboTreeY_ = nextComboTreeY_;
			currentComboTreeB_ = nextComboTreeB_;

			// 予約をクリア
			nextComboTreeX_ = nullptr;
			nextComboTreeY_ = nullptr;
			nextComboTreeB_ = nullptr;

			isChangeComboTree_ = false;
		}
	}

	// 攻撃の更新処理
	UpdateAttack();

	// 動ける状態なら、攻撃やスタイルチェンジなどの入力を受け付けて、状態の更新や移動処理を行う
	if (!isIncapacitatedState)
	{
		// スタイルチェンジ入力があればスタイルチェンジ処理を行う
		StyleChange();

		// レイジモードを開始する
		if (inputController_->IsRageModeRequested())
			RageModeInput();

		// プレイヤーは予備動作はないので、攻撃中が攻撃動作中と同じ扱いになる
		isInAttackSequence_ = IsAttack();

		// 防御状態を更新する
		UpdateGuardState();

		// 回避中は回避更新のみ行い、他の操作は受け付けない
		if (IsAvoid())
		{
			// アクションの更新処理
			ActionUpdate();
			Character::Update();
			return;
		}

		// 構え状態を更新する
		UpdateStanceState();

		// 移動入力方向を取得する
		bool hasMoveInput = false;
		const Vector2 kMoveInputDirection = inputController_->GetMoveDirection(hasMoveInput);

		// 構え中に回避ボタンを押したら回避を開始する
		if (isStance_ && inputController_->IsAvoidRequested())
		{
			// 入力方向に応じた回避方向を計算して回避を開始する
			Vector2 avoidDirection = GetAvoidDirection(kMoveInputDirection, hasMoveInput, GetCameraYaw());
			StartAvoid(Vector3(avoidDirection.x, 0.0f, avoidDirection.y), 1.5f, 0.3f);

			Character::Update();
			return;
		}

		// ダッシュ状態を更新する
		UpdateDashState(hasMoveInput);

		// 状態に応じた移動速度を計算する
		const float kMoveSpeed = GetCurrentMoveSpeed();

		if (hasMoveInput)
		{
			// カメラ基準の入力方向をワールド方向へ変換する
			const Vector2 kWorldMoveDirection = ToWorldMoveDirectionFromCamera(kMoveInputDirection, GetCameraYaw());
			SetMoveInputXZ(kWorldMoveDirection.Normalize(), kMoveSpeed);
		}
		else
		{
			// 入力がない場合は移動を停止する
			SetMoveInputXZ(Vector2(0.0f, 0.0f), kMoveSpeed);
		}
	}

	// プレイヤーは予備動作はないので、攻撃中が攻撃動作中と同じ扱いになる
	isInAttackSequence_ = IsAttack();

	// 攻撃中であれば、攻撃がヒットしたかどうかを判定してコンボフラグを更新する
	if(isInAttackSequence_)
	{ 
		if(IsHitAttack())
			isCombo_ = true;
	}
	else
	{
		// 攻撃が完全に終了している状態なので、コンボフラグをリセットする
		isCombo_ = false;
	}

	// アクションの更新処理
	ActionUpdate();

	// 基底クラスの更新
	Character::Update();
}

/// @brief 描画処理
void Player::Draw()
{
	// モデルを描画する
	if(model_)model_->Draw();

	// 攻撃エフェクトの描画
	if (attackTrail_)attackTrail_->Draw();
}

/// @brief 更新処理開始前のリセット
void Player::StartUpdate()
{
	// 攻撃入力をしたかどうかのフラグを更新する
	isPrevInputLightAttack_ = isInputLightAttack_;
	isInputLightAttack_ = false;

	// レイジモード入力をしたかどうかのフラグを更新する
	isPrevInputRageMode_ = isInputRageMode_;
	isInputRageMode_ = false;

	// 基底クラスの更新処理開始前のリセット
	Character::StartUpdate();
}

/// @brief 攻撃処理を更新する
void Player::UpdateAttack()
{
	// デルタタイムの取得
	const float kDt = GrowthEngine::GetInstance()->GetDeltaTime() * GrowthEngine::GetInstance()->GetTimeScale();

	// 現在の攻撃が「つかみ武器」攻撃で、かつ武器を持っていない場合は、最も近い武器を探して持つ
	if (currentAttack_ && currentAttack_->IsGrabWeapon() && !weapon_)
	{
		GrabWeapon(FindClosestWeapon());
	}

	// 攻撃入力のバッファ時間を減らす
	if (attackInputBufferTime_ > 0.0f)
	{
		attackInputBufferTime_ -= kDt;

		// バッファ時間が0以下になったらバッファされた攻撃入力を消す
		if (attackInputBufferTime_ <= 0.0f)
			bufferedAttackInput_ = AttackInputType::None;
	}

	// 攻撃ボタンの入力を受け付け、条件を満たす場合のみバッファに保存
	if (inputController_->IsInputXAttackRequested())
	{
		// 攻撃していない(待機・移動中) または、現在の攻撃から「弱」へ派生できる場合のみ
		if (!IsAttack() || currentAttack_->HasNextAttack(AttackInputType::InputX))
		{
			// 弱攻撃入力をしたことを記録する
			isInputLightAttack_ = true;

			bufferedAttackInput_ = AttackInputType::InputX;
			attackInputBufferTime_ = 0.2f; // バッファ有効時間
		}
	} 
	else if (inputController_->IsInputYAttackRequested())
	{
		// 攻撃していない または、現在の攻撃から「強」へ派生できる場合のみ
		if (!IsAttack() || currentAttack_->HasNextAttack(AttackInputType::InputY))
		{
			bufferedAttackInput_ = AttackInputType::InputY;
			attackInputBufferTime_ = 0.2f;
		}
	}
	else if (inputController_->IsInputBAttackRequested())
	{
		// 攻撃していない または、現在の攻撃から「特殊」へ派生できる場合のみ
		if (!IsAttack() || currentAttack_->HasNextAttack(AttackInputType::InputB))
		{
			bufferedAttackInput_ = AttackInputType::InputB;
			attackInputBufferTime_ = 0.2f;
		}
	}

	// 現在攻撃中でなく、かつバッファされた攻撃入力がある場合は攻撃を開始する
	if (!IsAttack() && bufferedAttackInput_ != AttackInputType::None)
	{
		// 入力に応じて別々のコンボツリーを実行する
		if (bufferedAttackInput_ == AttackInputType::InputX && currentComboTreeX_)
		{
			currentComboTreeX_->Exec();
		}
		else if (bufferedAttackInput_ == AttackInputType::InputY && currentComboTreeY_)
		{
			currentComboTreeY_->Exec();
		}
		else if (bufferedAttackInput_ == AttackInputType::InputB && currentComboTreeB_)
		{
			currentComboTreeB_->Exec();
		}

		// バッファされた攻撃入力を消す
		ConsumeBufferedAttackInput();
	}
}

/// @brief 構え状態を更新する
void Player::UpdateStanceState()
{
	// 怯み状態、動けない状態は構え状態にならない
	if(IsGrabbing() || IsIncapacitated() || IsDash() || !GetLockOnTarget())
	{
		isStance_ = false;
		return;
	}

	isStance_ = true;
}

/// @brief ダッシュ状態を更新する
/// @param hasMoveInput
void Player::UpdateDashState(bool hasMoveInput)
{
	// 怯み状態、動けない状態はダッシュ状態にならない
	if (IsGrabbing() || IsIncapacitated() || IsAttack())
	{
		return;
	}

	// ダッシュ入力があって、移動入力もあって、ダッシュ中でない場合はダッシュを開始する
	if (inputController_->IsDashRequested() && hasMoveInput && !IsDash())
	{
		stateMachine_->ChangeState("Dash");

		// ダッシュ開始時にスローモーションを開始する
		GrowthEngine::GetInstance()->StartSlowMotion(0.1f, 0.1f);
	}
}

/// @brief 現在の移動速度を取得する
/// @return
float Player::GetCurrentMoveSpeed() const
{
	// 怯み状態、または構え状態、または「つかまれている状態」、または攻撃中、またはダウン中、または空中にいる状態、またはスタイルチェンジ中なら移動速度は0
	if (IsGuard() || IsIncapacitated())
		return 0.0f;

	// 構え中は移動速度を半分にする
	// Character側で速度補間しているため、通常速度↔構え速度の切り替えも補間される
	float moveSpeed = isStance_ ? (kNormalMoveSpeed * kStanceMoveSpeedMultiplier) : kNormalMoveSpeed;

	// ダッシュ中は移動速度を2倍にする
	if (IsDash())
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

/// @brief コンボツリーの変更をリクエストする
/// @param combTreeX 
/// @param comboTreeY 
/// @param comboTreeB 
void Player::RequestComboTreeChange(ComboTree* comboTreeX, ComboTree* comboTreeY, ComboTree* comboTreeB)
{
	// 攻撃中なら次のコンボツリーとして保存し、攻撃中でなければ現在のコンボツリーとして保存する
	if (currentAttack_)
	{
		nextComboTreeX_ = comboTreeX;
		nextComboTreeY_ = comboTreeY;
		nextComboTreeB_ = comboTreeB;

		// コンボツリーの変更フラグを立てる
		isChangeComboTree_ = true;
	}
	else
	{
		// 攻撃中でない場合は現在のコンボツリーとして保存する
		currentComboTreeX_ = comboTreeX;
		currentComboTreeY_ = comboTreeY;
		currentComboTreeB_ = comboTreeB;

		nextComboTreeX_ = nullptr;
		nextComboTreeY_ = nullptr;
		nextComboTreeB_ = nullptr;

		isChangeComboTree_ = false;
	}
}

/// @brief コンボツリーを初期化する
/// @param comboTreeConfig 
/// @param comboTreeEditor 
void Player::InitComboTree(const ComboTreeConfig& comboTreeConfig)
{
	stateMachine_->GetState("None")->SetComboTree(
		ComboTreeFactory::CreateTree(comboTreeConfig.noneStateCT.xName_, this),
		ComboTreeFactory::CreateTree(comboTreeConfig.noneStateCT.yName_, this),
		ComboTreeFactory::CreateTree(comboTreeConfig.noneStateCT.bName_, this));

	stateMachine_->GetState("Dash")->SetComboTree(
		ComboTreeFactory::CreateTree(comboTreeConfig.dashStateCT.xName_, this),
		ComboTreeFactory::CreateTree(comboTreeConfig.dashStateCT.yName_, this),
		ComboTreeFactory::CreateTree(comboTreeConfig.dashStateCT.bName_, this));

	stateMachine_->GetState("Grabbed")->SetComboTree(
		ComboTreeFactory::CreateTree(comboTreeConfig.grabbedStateCT.xName_, this),
		ComboTreeFactory::CreateTree(comboTreeConfig.grabbedStateCT.yName_, this),
		ComboTreeFactory::CreateTree(comboTreeConfig.grabbedStateCT.bName_, this));

	stateMachine_->GetState("Grabbing")->SetComboTree(
		ComboTreeFactory::CreateTree(comboTreeConfig.grabbingStateCT.xName_, this),
		ComboTreeFactory::CreateTree(comboTreeConfig.grabbingStateCT.yName_, this),
		ComboTreeFactory::CreateTree(comboTreeConfig.grabbingStateCT.bName_, this));

	stateMachine_->GetState("Guard")->SetComboTree(
		ComboTreeFactory::CreateTree(comboTreeConfig.guardStateCT.xName_, this),
		ComboTreeFactory::CreateTree(comboTreeConfig.guardStateCT.yName_, this),
		ComboTreeFactory::CreateTree(comboTreeConfig.guardStateCT.bName_, this));

	stateMachine_->GetState("LightDamage")->SetComboTree(
		ComboTreeFactory::CreateTree(comboTreeConfig.lightDamageStateCT.xName_, this),
		ComboTreeFactory::CreateTree(comboTreeConfig.lightDamageStateCT.yName_, this),
		ComboTreeFactory::CreateTree(comboTreeConfig.lightDamageStateCT.bName_, this));

	stateMachine_->GetState("HeavyDamage")->SetComboTree(
		ComboTreeFactory::CreateTree(comboTreeConfig.heavyDamageStateCT.xName_, this),
		ComboTreeFactory::CreateTree(comboTreeConfig.heavyDamageStateCT.yName_, this),
		ComboTreeFactory::CreateTree(comboTreeConfig.heavyDamageStateCT.bName_, this));

	stateMachine_->GetState("DownFalling")->SetComboTree(
		ComboTreeFactory::CreateTree(comboTreeConfig.downFallingStateCT.xName_, this),
		ComboTreeFactory::CreateTree(comboTreeConfig.downFallingStateCT.yName_, this),
		ComboTreeFactory::CreateTree(comboTreeConfig.downFallingStateCT.bName_, this));

	stateMachine_->GetState("DownLying")->SetComboTree(
		ComboTreeFactory::CreateTree(comboTreeConfig.downLyingStateCT.xName_, this),
		ComboTreeFactory::CreateTree(comboTreeConfig.downLyingStateCT.yName_, this),
		ComboTreeFactory::CreateTree(comboTreeConfig.downLyingStateCT.bName_, this));

	stateMachine_->GetState("DownGettingUp")->SetComboTree(
		ComboTreeFactory::CreateTree(comboTreeConfig.downGettingUpStateCT.xName_, this),
		ComboTreeFactory::CreateTree(comboTreeConfig.downGettingUpStateCT.yName_, this),
		ComboTreeFactory::CreateTree(comboTreeConfig.downGettingUpStateCT.bName_, this));

	stateMachine_->GetState("DownStagger")->SetComboTree(
		ComboTreeFactory::CreateTree(comboTreeConfig.downStaggerStateCT.xName_, this),
		ComboTreeFactory::CreateTree(comboTreeConfig.downStaggerStateCT.yName_, this),
		ComboTreeFactory::CreateTree(comboTreeConfig.downStaggerStateCT.bName_, this));

	stateMachine_->GetState("BlownAway")->SetComboTree(
		ComboTreeFactory::CreateTree(comboTreeConfig.blownAwayStateCT.xName_, this),
		ComboTreeFactory::CreateTree(comboTreeConfig.blownAwayStateCT.yName_, this),
		ComboTreeFactory::CreateTree(comboTreeConfig.blownAwayStateCT.bName_, this));

	stateMachine_->GetState("BlownFalling")->SetComboTree(
		ComboTreeFactory::CreateTree(comboTreeConfig.blownFallingStateCT.xName_, this),
		ComboTreeFactory::CreateTree(comboTreeConfig.blownFallingStateCT.yName_, this),
		ComboTreeFactory::CreateTree(comboTreeConfig.blownFallingStateCT.bName_, this));

	stateMachine_->GetState("Repel")->SetComboTree(
		ComboTreeFactory::CreateTree(comboTreeConfig.repelStateCT.xName_, this),
		ComboTreeFactory::CreateTree(comboTreeConfig.repelStateCT.yName_, this),
		ComboTreeFactory::CreateTree(comboTreeConfig.repelStateCT.bName_, this));

	stateMachine_->GetState("Deflect")->SetComboTree(
		ComboTreeFactory::CreateTree(comboTreeConfig.deflectStateCT.xName_, this),
		ComboTreeFactory::CreateTree(comboTreeConfig.deflectStateCT.yName_, this),
		ComboTreeFactory::CreateTree(comboTreeConfig.deflectStateCT.bName_, this));

	stateMachine_->GetState("Repelled")->SetComboTree(
		ComboTreeFactory::CreateTree(comboTreeConfig.repelledStateCT.xName_, this),
		ComboTreeFactory::CreateTree(comboTreeConfig.repelledStateCT.yName_, this),
		ComboTreeFactory::CreateTree(comboTreeConfig.repelledStateCT.bName_, this));

	stateMachine_->GetState("Deflected")->SetComboTree(
		ComboTreeFactory::CreateTree(comboTreeConfig.deflectedStateCT.xName_, this),
		ComboTreeFactory::CreateTree(comboTreeConfig.deflectedStateCT.yName_, this),
		ComboTreeFactory::CreateTree(comboTreeConfig.deflectedStateCT.bName_, this));

	stateMachine_->GetState("Avoid")->SetComboTree(
		ComboTreeFactory::CreateTree(comboTreeConfig.avoidStateCT.xName_, this),
		ComboTreeFactory::CreateTree(comboTreeConfig.avoidStateCT.yName_, this),
		ComboTreeFactory::CreateTree(comboTreeConfig.avoidStateCT.bName_, this));

	stateMachine_->GetState("Dead")->SetComboTree(
		ComboTreeFactory::CreateTree(comboTreeConfig.deadStateCT.xName_, this),
		ComboTreeFactory::CreateTree(comboTreeConfig.deadStateCT.yName_, this),
		ComboTreeFactory::CreateTree(comboTreeConfig.deadStateCT.bName_, this));
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

		// 現在持っているのが「警棒」以外の武器だった場合
		if (weapon_ != nullptr && weapon_ != baton_)
		{
			ReleaseWeapon();
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
		hStanceMotion_ = motionManager_->GetMotion(MotionType::Stance, "Standard");
		if (attackTrail_)attackTrail_->param_->color = Vector4(0.5f, 0.5f, 1.0f, 1.0f);
		break;

		// 撃鉄スタイル
	case FightStyle::Hammer:
		hStanceMotion_ = motionManager_->GetMotion(MotionType::Stance, "Fighter");
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
	if (inputController_->IsStyleChangeRequested())
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

/// @brief レイジモード入力処理
void Player::RageModeInput()
{
	// 怯み状態、または構え状態、または「つかまれている状態」、または攻撃中、またはダウン中、または空中にいる状態、またはスタイルチェンジ中ならレイジモード入力を受け付けない
	if (IsIncapacitated())return;

	// レイジゲージが最大値に達している場合はレイジモード入力を受け付ける
	if (IsRageGageThresholdExceeded())
	{
		isInputRageMode_ = true;
	}

	// 基底クラスのレイジモード入力処理を呼び出す
	Character::RageModeInput();
}

/// @brief ロックオンしているターゲットを検索する
void Player::SearchLockOnTarget()
{
	// 最も視線方向に近い相手を探す
	float bestDistance = std::numeric_limits<float>::max();
	float bestDot = -1.0f;

	// 攻撃中の相手を見つけたかどうかのフラグ
	bool hasFoundAttackingTarget = false;

	const Vector3 kSelfPosition = GetWorldPosition();

	// ロックオン対象の側を決定する
	const bool kIsSelfPlayerSide = IsPlayerSide();

	for (Character* character : characters_)
	{
		// 無効または自分自身は除外する
		if (!character || character == this)continue;

		// 自分と同じ側の相手は除外する
		if (kIsSelfPlayerSide == character->IsPlayerSide()) continue;

		// 死んでいる相手は除外する
		if (character->IsDead())continue;

		// 自分から相手へのベクトルを計算する
		Vector3 toTarget = character->GetWorldPosition() - kSelfPosition;
		toTarget.y = 0.0f;

		// 距離の二乗を計算する
		const float kDistanceSq = toTarget.x * toTarget.x + toTarget.z * toTarget.z;
		if (kDistanceSq <= 0.0f)
			continue;


		// 自分の移動方向を取得する
		Vector3 direction = movement_->GetDirection();

		// 目の前にいる相手のみリストに登録する
		const Vector3 kToTargetDirection = toTarget.Normalize();
		if (Dot(direction, kToTargetDirection) <= 0.0f)
			continue;

		// 距離と相手ポインタを登録する
		const float kDistance = std::sqrt(kDistanceSq);

		// 視線方向との内積を計算する
		const float kViewDot = Dot(direction, kToTargetDirection);

		// プレイヤーに攻撃を仕掛けてきているかどうか
		bool isAttacking = false;
		if (character->GetLockOnTarget() && character->GetLockOnTarget() == this && character->IsInAttackSequence())
			isAttacking = true;

		// ターゲットを更新すべきかどうかの判定フラグ
		bool isBetterTarget = false;

		// 攻撃中の相手を優先する
		if (isAttacking && !hasFoundAttackingTarget)
		{
			isBetterTarget = true;
		}
		else if (isAttacking == hasFoundAttackingTarget)
		{
			// 攻撃中の相手が見つかっている場合は、距離が近い相手を優先する
			if (kDistance < bestDistance)
			{
				isBetterTarget = true;
			}
			else if (kDistance == bestDistance && kViewDot > bestDot)
			{
				isBetterTarget = true;
			}
		}

		// より良いターゲットが見つかった場合の更新処理
		if (isBetterTarget)
		{
			bestDistance = kDistance;
			bestDot = kViewDot;
			hasFoundAttackingTarget = isAttacking; // 攻撃中の敵を見つけた状態を保存
			lockOnTarget_ = character;
		}
	}
}

/// @brief 最も近い武器を検索する
/// @return 
Weapon* Player::FindClosestWeapon()
{
	Weapon* closestWeapon = nullptr;

	// 半径3メートルの2乗
	float minDistanceSq = 3.0f * 3.0f;

	// 自身のワールド座標を取得する
	Vector3 myPosition = GetWorldPosition();

	// フィールド上のすべての武器リストを取得する
	const auto& allWeapons = Weapon::GetWeapons();

	for (Weapon* weapon : allWeapons)
	{
		// 有効でない武器や壊れた武器、すでに誰かに所持されている武器は除外する
		if (!weapon->IsActive() || weapon->IsBreak() || weapon->IsEquipped()) continue;

		// 武器のワールド座標を取得する
		Vector3 weaponPos = weapon->GetWorldPosition();

		// プレイヤーと武器の距離の2乗を計算する
		Vector3 diff = weaponPos - myPosition;
		float distanceSq = diff.LengthSq();

		// 3メートル以内かつ、現在の最短距離より近ければ候補を更新する
		if (distanceSq <= minDistanceSq)
		{
			minDistanceSq = distanceSq;
			closestWeapon = weapon;
		}
	}

	return closestWeapon;
}

/// @brief 防御状態を更新する
void Player::UpdateGuardState()
{
	// 既に防御中なら何もしない
	if (IsGuard() || IsGrabbing() || IsAttack() || IsIncapacitated())return;

	// 防御入力中は防御フラグを立て、離したらフラグを下ろす
	if(inputController_->IsGuardRequested())
		ExecuteGuard();
}