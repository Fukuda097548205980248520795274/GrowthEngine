#include "GrabStrikeAttack.h"
#include "Entity/Character/Character.h"

/// @brief コンストラクタ
GrabStrikeAttack::GrabStrikeAttack(Character* character, const GrabStrikeAttackInitData& initData)
	: Attack(character)
{
	// モーションと攻撃時間を設定
	hAttackMotion_ = initData.hAttackAnimation;
	hTargetAnimation_ = initData.hTargetAnimation;
	moveSpeed_ = initData.moveSpeed;
	moveStartTime_ = initData.moveStartTime;
	moveEndTime_ = initData.moveEndTime;
	attackTime_ = initData.attackTime;
	damageReaction_ = initData.damageReaction;
	isRelease_ = initData.isRelease;
	knockback_ = initData.knockback;
	knockbackDirection_ = initData.knockbackDirection;
	chargeTime_ = initData.chargeTime;
	chargeCompleteTime_ = initData.chargeCompleteTime;
	chargeFinishAttackTime_ = initData.chargeFinishAttackTime;
	isChargeAttack_ = initData.isChargeAttack;

	// 攻撃の種類を掴み打撃に設定
	attackType_ = AttackType::GrabStrike;

	// ヒット定義をコピー
	for (const auto& def : initData.hits)
		hits_.push_back(def);
}

/// @brief デストラクタ
GrabStrikeAttack::~GrabStrikeAttack()
{
	// 攻撃中であれば、攻撃を終了する
	if (owner_ && owner_->GetCurrentAttack() == this)
		owner_->SetCurrentAttack(nullptr);
}

/// @brief 実行
void GrabStrikeAttack::Exec()
{
	// ブレイクポイントのチェック
	BreakpointOnExec();

	// 基底クラスの実行
	Attack::Exec();

	// 掴んでいる相手を取得
	grabbedTarget_ = owner_->GetGrabTarget();

	// もし掴んでいる相手がいない場合は、攻撃を終了する
	if (!grabbedTarget_)
	{
		Exit();
		return;
	}

	// チャージ攻撃の状態を初期化する
	isChargeFinished_ = false;
	canChargeAttack_ = isChargeAttack_;
	chargeTimer_ = 0.0f;

	// タイマーとフラグをリセット
	attackTimer_ = 0.0f;
	isReleased_ = false;
}

/// @brief 更新処理
void GrabStrikeAttack::Update()
{
	// ブレイクポイントのチェック
	BreakpointOnUpdate();

	// 掴んでいる相手を取得
	grabbedTarget_ = owner_->GetGrabTarget();

	// もし掴んでいる相手がいない場合は、攻撃を終了する
	if (!grabbedTarget_ || grabbedTarget_->IsDead() || grabbedTarget_->IsFinished())
	{
		Exit();

		// Character側の掴み状態を解除する処理を呼ぶ
		owner_->SetGrabTarget(nullptr);
		return;
	}

	// 0.0f秒以下の攻撃タイマーの場合、アニメーションを設定する
	if (attackTimer_ <= 0.0f)
	{
		owner_->SetAnimation(hAttackMotion_, true, false);
		grabbedTarget_->SetAnimation(hTargetAnimation_, true, false);
	}

	// 攻撃タイマーが攻撃時間に達したら、次の攻撃への移行を確認する
	if (attackTimer_ >= attackTime_)
	{
		// バッファされた攻撃入力を取得する
		AttackInputType bufferedInput = owner_->GetBufferedAttackInput();

		if (bufferedInput == AttackInputType::InputX && nextInputXAttack_)
		{
			// ライト攻撃への移行は、ヘビー攻撃への移行よりも優先されると仮定する（両方入力されている場合はヘビー攻撃に移行する）
			owner_->ConsumeBufferedAttackInput();
			this->Exit();
			nextInputXAttack_->Exec();
			nextInputXAttack_->SetChargeInputType(bufferedInput);
			return;
		}
		else if (bufferedInput == AttackInputType::InputY && nextInputYAttack_)
		{
			// ヘビー攻撃への移行は、ライト攻撃への移行よりも優先されると仮定する（両方入力されている場合はヘビー攻撃に移行する）
			owner_->ConsumeBufferedAttackInput();
			this->Exit();
			nextInputYAttack_->Exec();
			nextInputYAttack_->SetChargeInputType(bufferedInput);
			return;
		}
		else if (bufferedInput == AttackInputType::InputB && nextInputBAttack_)
		{
			// バッファされた攻撃入力を消費する
			owner_->ConsumeBufferedAttackInput();
			this->Exit();
			nextInputBAttack_->Exec();
			nextInputBAttack_->SetChargeInputType(bufferedInput);
			return;
		}
	}

	// チャージ攻撃が可能な場合、チャージ攻撃の入力タイプが一致するかどうかを確認する
	if (canChargeAttack_)
	{
		AttackInputType bufferedChargeInput = owner_->GetBufferedChargeAttackInput();

		// チャージ攻撃の入力タイプが一致する場合は、チャージ攻撃を継続する
		if (bufferedChargeInput != chargeInputType_)
		{
			canChargeAttack_ = false;
		}
	}


	// タイマーを記録する
	prevTimer_ = attackTimer_;


	// レイジモードの攻撃速度を取得する
	float rageModeSpeed = owner_->RageModeAttackSpeed();

	// デルタタイムを取得する（レイジモードの攻撃速度を考慮する）
	float dt = engine_->GetDeltaTime() * engine_->GetTimeScale() * rageModeSpeed;

	// チャージ攻撃が可能であれば、チャージタイマーを更新する
	if (canChargeAttack_)
	{
		chargeTimer_ += dt;

		// チャージ攻撃の割合を計算する（0.0fから1.0fの範囲で正規化）
		float t = GetChargeTimeRate();

		float easing = 1.0f - std::pow(1.0f - t, 3.0f); // イージング関数を使用して、チャージ攻撃の割合を補間する

		// チャージ攻撃の割合に応じて、攻撃タイマーを補間する
		attackTimer_ = std::lerp(0.0f, chargeFinishAttackTime_, easing);

		// チャージ攻撃が完了したかどうかを判定する
		if (t > chargeCompleteTime_ && !isChargeFinished_)
		{
			isChargeFinished_ = true;
			SoundManager::GetInstance()->SeChargeComplete();
		}

		// チャージタイマーがチャージ時間を超えた場合、チャージ攻撃を終了する
		if (chargeTimer_ >= chargeTime_)
		{
			canChargeAttack_ = false;
		}
	}
	else
	{
		// 攻撃タイマーを更新する
		attackTimer_ += dt;
	}


	// ヒットのタイミングになったら、掴んでいる相手にダメージを与える
	for (auto& state : hits_)
	{
		if (attackTimer_ >= state.hitTime && prevTimer_ <= state.hitTime)
		{
			std::optional<Vector3> hitPosition = owner_->GetBonePosition(state.hitJoint);
			grabbedTarget_->OnGrabDamage(state.damage, damageReaction_, owner_, hitPosition);
		}
	}

	// 掴んでいる相手を離すタイミングになったら、掴み状態を解除する
	if (isRelease_ && !isReleased_ && attackTimer_ >= attackTime_)
	{
		if (grabbedTarget_)
		{
			// まず、攻撃者の前方ベクトルを取得する
			Vector3 forward = owner_->GetDirection();
			forward.y = 0.0f;
			forward = (forward.Length() > 0.0f) ? forward.Normalize() : owner_->GetDirection();

			// ワールドの上方向（Y軸）を定義する
			Vector3 worldUp(0.0f, 1.0f, 0.0f);

			// 前方とワールドの上方向から右方向を計算する
			Vector3 right;
			right.x = worldUp.y * forward.z - worldUp.z * forward.y;
			right.y = worldUp.z * forward.x - worldUp.x * forward.z;
			right.z = worldUp.x * forward.y - worldUp.y * forward.x;
			right = right.Normalize();

			Vector3 up;
			up.x = forward.y * right.z - forward.z * right.y;
			up.y = forward.z * right.x - forward.x * right.z;
			up.z = forward.x * right.y - forward.y * right.x;
			up = up.Normalize();

			// 攻撃の定義に基づいて、ノックバックの方向を計算する
			Vector3 knockBackDirection;
			knockBackDirection.x = right.x * knockbackDirection_.x + up.x * knockbackDirection_.y + forward.x * knockbackDirection_.z;
			knockBackDirection.y = right.y * knockbackDirection_.x + up.y * knockbackDirection_.y + forward.y * knockbackDirection_.z;
			knockBackDirection.z = right.z * knockbackDirection_.x + up.z * knockbackDirection_.y + forward.z * knockbackDirection_.z;
			knockBackDirection = knockBackDirection.Normalize();

			// 相手を飛ばす用のダメージ処理を呼ぶ
			grabbedTarget_->OnDamage(0, damageReaction_, knockback_, knockBackDirection, owner_->GetWorldPosition(),
				nullptr, std::nullopt, false, true);

			// Character側の掴み状態を解除する処理を呼ぶ
			owner_->SetGrabTarget(nullptr);
		}

		isReleased_ = true; // 解除済みフラグを立てる

		// 攻撃を終了する
		Attack::Update();
		return;
	}

	// 攻撃の特定の時間帯は、攻撃者が回避または無力化されているかどうかを確認する
	if (owner_->IsJustAvoided() || owner_->IsIncapacitated())
	{
		this->Exit();

		// Character側の掴み状態を解除する処理を呼ぶ
		owner_->SetGrabTarget(nullptr);
		return;
	}

	// 攻撃時間が経過したら、攻撃を終了する
	if (attackTimer_ >= attackTime_ && !owner_->IsPlayer())
	{
		// タイマーをリセットする
		attackTimer_ = 0.0f;
	}
}

/// @brief リセット
void GrabStrikeAttack::Reset()
{
	// ブレイクポイントのチェック
	BreakpointOnReset();

	Attack::Reset();
	attackTimer_ = 0.0f;
	isReleased_ = false;
	grabbedTarget_ = nullptr;
}

/// @brief 終了、中断
void GrabStrikeAttack::Exit()
{
	// ブレイクポイントのチェック
	BreakpointOnExit();

	// もし掴んでいる相手がいる状態で攻撃が終了した場合は、確実に手を離す
	if (isRelease_ && !isReleased_ && grabbedTarget_)
		owner_->SetGrabTarget(nullptr);

	Attack::Exit();
}