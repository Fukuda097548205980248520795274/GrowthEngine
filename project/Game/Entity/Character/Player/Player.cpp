#include "Player.h"
#include "Entity/Weapon/Weapon.h"
#include "HUD/HP/HP.h"

#include "ComboTree/ComboTreeEditor/ComboTreeFactory/ComboTreeFactory.h"

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
void Player::Initialize(const CharacterInitData& initData, std::unique_ptr<ComboTree> comboTree, Weapon* baton)
{
	assert(baton);

	// 初期化データを設定する
	SetInitData(initData);

	// 入力コントローラーを作成する
	inputController_ = std::make_unique<PlayerInputController>();
	inputController_->Initialize();

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

	// スタイルチェンジ開始時の処理
	OnStyleChanged(currentStyle_);

	// コンボツリーを作成する
	comboTree_ = std::move(comboTree);
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

		// レイジモードを開始する
		if (inputController_->IsRageModeRequested())
			RageModeInput();

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
			const Vector2 moveInputDirection = inputController_->GetMoveDirection(hasMoveInput);
			if (isStance_ && inputController_->IsAvoidRequested())
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
		const Vector2 moveInputDirection = inputController_->GetMoveDirection(hasMoveInput);

		// 構え中に回避ボタンを押したら回避を開始する
		if (isStance_ && inputController_->IsAvoidRequested())
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
			if (inputController_->IsEscapeMashRequested()) isStruggleInput = true;

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

	// 基底クラスの更新処理開始前のリセット
	Character::StartUpdate();
}

/// @brief 攻撃処理を更新する
void Player::UpdateAttack()
{
	// デルタタイムの取得
	const float deltaTime = GrowthEngine::GetInstance()->GetDeltaTime() * GrowthEngine::GetInstance()->GetTimeScale();

	// 怯み状態、または「つかまれている状態」なら攻撃の更新は行わない
	if (IsDamageReaction() || IsGrabbing() || IsGrabbed() || IsDown() || IsStyleChanging())
		return;

	// 攻撃入力のバッファ時間を減らす
	if (attackInputBufferTime_ > 0.0f)
	{
		attackInputBufferTime_ -= deltaTime;

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
		if (bufferedAttackInput_ == AttackInputType::InputX)
		{
			// 1段目の攻撃を実行
			comboTree_->Exec();
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
	if (inputController_->IsDashRequested() && hasMoveInput && !isDash_)
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

	// プレイヤーはダウン後、すぐに起き上がるようにする
	damageReactionTimer_ = 0.0f;

	return true;
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
	if(inputController_->IsGuardRequested())
	{
		SetGuard(true);
	}
	else
	{
		SetGuard(false);
	}
}