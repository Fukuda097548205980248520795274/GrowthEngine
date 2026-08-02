#include "ComboAttack.h"
#include "Entity/Character/Character.h"
#include "Entity/Weapon/Weapon.h"

#include "EffectManager/EffectManager.h"

/// @brief コンストラクタ
/// @param character 
/// @param initData 
ComboAttack::ComboAttack(Character* character, const CombAttackInitData& initData)
	: Attack(character)
{
	// 初期化データをメンバ変数にコピーする
	hAttackMotion_ = initData.hAttackMotion;
	attackTime_ = initData.attackTime;
	moveSpeed_ = initData.moveSpeed;
	moveStartTime_ = initData.moveStartTime;
	moveEndTime_ = initData.moveEndTime;
	cancelStartTime_ = initData.cancelStartTime;
	cancelEndTime_ = initData.cancelEndTime;
	isGrabWeapon_ = initData.isGrabWeapon;
	grabWeaponStartTime_ = initData.grabWeaponStartTime;
	grabWeaponEndTime_ = initData.grabWeaponEndTime;

	// 攻撃の種類をコンボに設定する
	attackType_ = AttackType::Combo;

	// 当たり判定の定義からHitboxStateを作成してリストに追加する
	for (const auto& def : initData.hitDefinitions)
	{
		HitboxState state;
		state.def = def;
		hitStates_.push_back(state);
	}
}

/// @brief デストラクタ
ComboAttack::~ComboAttack()
{
	// 攻撃中であれば、攻撃を終了する
	if (owner_ && owner_->GetCurrentAttack() == this)
		owner_->SetCurrentAttack(nullptr);

	// すべての判定をリセット・削除する
	for (auto& state : hitStates_)
	{
		state.hitCharacters.clear();
		state.DeleteHitbox();
	}
}

/// @brief 実行
void ComboAttack::Exec()
{
	// ブレイクポイントのチェック
	BreakpointOnExec();

	// 基底の実行
	Attack::Exec();

	// アニメーションを設定する
	owner_->SetAnimation(hAttackMotion_, true , false);

	// 状態なしに遷移する
	auto stateMachine = owner_->GetStateMachine();
	stateMachine->ChangeState("None");

	// 攻撃タイマーを初期化する
	attackTimer_ = 0.0f;

	// 移動速度を初期化する
	currentMoveSpeed_ = moveSpeed_;

	// すべての判定をリセット・削除する
	for (auto& state : hitStates_)
	{
		state.hitCharacters.clear();
		state.DeleteHitbox();
	}

	// 攻撃用のトレイルをクリアする
	owner_->TrailClear();
}

/// @brief 更新処理
void ComboAttack::Update()
{
	// ブレイクポイントのチェック
	BreakpointOnUpdate();

	// コンボキャンセル受付時間内であれば、次の攻撃への入力をチェックする
	if (attackTimer_ >= cancelStartTime_ && attackTimer_ <= cancelEndTime_)
	{
		if (owner_)
		{
			// バッファされた攻撃入力を取得する
			AttackInputType bufferedInput = owner_->GetBufferedAttackInput();

			if (bufferedInput == AttackInputType::InputX && nextInputXAttack_)
			{
				// ライト攻撃への移行は、ヘビー攻撃への移行よりも優先されると仮定する（両方入力されている場合はヘビー攻撃に移行する）
				owner_->ConsumeBufferedAttackInput();
				this->Exit();
				nextInputXAttack_->Exec();
				return;
			}
			else if (bufferedInput == AttackInputType::InputY && nextInputYAttack_)
			{
				// ヘビー攻撃への移行は、ライト攻撃への移行よりも優先されると仮定する（両方入力されている場合はヘビー攻撃に移行する）
				owner_->ConsumeBufferedAttackInput();
				this->Exit();
				nextInputYAttack_->Exec();
				return;
			}
			else if (bufferedInput == AttackInputType::InputB && nextInputBAttack_)
			{
				// バッファされた攻撃入力を消費する
				owner_->ConsumeBufferedAttackInput();
				this->Exit();
				nextInputBAttack_->Exec();
				return;
			}
		}
	}


	// 攻撃の特定の時間帯は、攻撃者が回避、掴み、または無力化されているかどうかを確認する
	if (owner_->IsJustAvoided() || owner_->IsGrabbing() || owner_->IsIncapacitated() || owner_->IsDash())
	{
		this->Exit();
		return;
	}


	// レイジモードの攻撃速度を取得する
	float rageModeSpeed = owner_->RageModeAttackSpeed();

	// 攻撃タイマーを更新する
	attackTimer_ += engine_->GetDeltaTime() * engine_->GetTimeScale() * rageModeSpeed;

	// 各当たり判定の状態を更新する
	for (auto& state : hitStates_)
	{
		// ジョイントタイプがNoneの場合は当たり判定を出さない
		if (state.def.jointType == JointType::None) continue;

		// 武器のジョイントタイプの場合は、攻撃者が武器を持っているかどうかを確認する
		if (state.def.jointType == JointType::Weapon && owner_->GetWeapon() == nullptr)continue;

		if (attackTimer_ >= state.def.startTime && attackTimer_ <= state.def.endTime)
		{
			// 当たり判定がまだ存在しない場合は作成する
			if (state.hitbox.collider_ == nullptr)
			{
				state.hitbox.collider_ = owner_->GetHitboxGroup()->CreateInstance();

				// 攻撃の種類に応じて、攻撃のSEを再生する
				SoundManager* soundManager = SoundManager::GetInstance();
				if (state.def.damageReaction == DamageReaction::LightStagger)
				{
					soundManager->SeLightAttack();
				} 
				else if (state.def.damageReaction == DamageReaction::HeavyStagger || state.def.damageReaction == DamageReaction::Down)
				{
					soundManager->SeHeavyAttack();
				}
			}

			// 攻撃用のトレイルがある場合は、トレイルの位置も更新する
			Vector3 bonePosition = owner_->GetBonePosition(state.def.jointType);
			Vector3 boneParentPosition = owner_->GetBonePosition(MotionManager::GetInstance()->GetParentJoint(state.def.jointType));
			owner_->SetTrailPos(bonePosition, boneParentPosition);

			// 攻撃のエフェクトを再生する
			EffectManager::GetInstance()->AttackImpact000(bonePosition);

			// 当たり判定の位置とサイズを攻撃者のボーンに基づいて更新する
			auto sphere = static_cast<Collision3DInstanceSphere*>(state.hitbox.collider_);
			if (state.def.jointType == JointType::Weapon)
			{
				// 武器の当たり判定は、武器の位置とサイズを使用する
				sphere->param_->center = owner_->GetWeapon()->GetWorldPosition();
			}
			else
			{
				// その他の当たり判定は、攻撃者の指定されたボーンの位置を使用する
				sphere->param_->center = bonePosition;
			}

			sphere->param_->radius = state.def.radius;


			// 当たり判定がヒットしているかどうかをチェックするためのラムダ関数を定義する
			auto IsSphereHit = [](Engine::BaseCollision3DInstance* hitbox, Engine::BaseCollision3DInstance* hurtbox) -> bool
				{
					if (!hitbox || !hurtbox) return false;

					auto s1 = static_cast<Collision3DInstanceSphere*>(hitbox);
					auto s2 = static_cast<Collision3DInstanceSphere*>(hurtbox);

					// 中心点同士の距離を求める
					Vector3 diff = s1->param_->center - s2->param_->center;

					// 距離が、2つの球の半径の合計値以下なら当たっている
					return diff.Length() <= (s1->param_->radius + s2->param_->radius);
				};

			// ターゲットのリストを取得する
			for (Character* target : Character::GetCharacters())
			{
				// ターゲットが攻撃者自身である場合、同じサイドのキャラクターである場合、またはすでに倒れている場合はスキップする
				if (target == owner_ || (target->IsPlayerSide() == owner_->IsPlayerSide() && target->IsEnemySide() == owner_->IsEnemySide()) || target->IsDead()) continue;

				// すでにこの攻撃でヒットしているターゲットはスキップする
				if (std::find(state.hitCharacters.begin(), state.hitCharacters.end(), target) != state.hitCharacters.end())
					continue;

				// 当たり判定がヒットした場合の処理
				if (IsSphereHit(state.hitbox.collider_, target->GetHurtboxChest().collider_) ||
					IsSphereHit(state.hitbox.collider_, target->GetHurtboxHead().collider_) ||
					IsSphereHit(state.hitbox.collider_, target->GetHurtboxRoot().collider_))
				{
					// ターゲットに対してノックバックの方向を計算するためのベクトルを定義する
					Vector3 forward = target->GetPosition() - owner_->GetPosition();
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
					knockBackDirection.x = right.x * state.def.knockbackDirection.x + up.x * state.def.knockbackDirection.y + forward.x * state.def.knockbackDirection.z;
					knockBackDirection.y = right.y * state.def.knockbackDirection.x + up.y * state.def.knockbackDirection.y + forward.y * state.def.knockbackDirection.z;
					knockBackDirection.z = right.z * state.def.knockbackDirection.x + up.z * state.def.knockbackDirection.y + forward.z * state.def.knockbackDirection.z;
					knockBackDirection = knockBackDirection.Normalize();

					// 当たり判定の中心点を取得する
					Vector3 hitPosition = static_cast<Collision3DInstanceSphere*>(state.hitbox.collider_)->param_->center;

					// ターゲットにダメージを与える
					bool isHit = target->OnDamage(state.def.damage, state.def.damageReaction, state.def.knockback, knockBackDirection, owner_->GetWorldPosition(), owner_,
						std::make_optional(hitPosition));

					// 攻撃が何かしら敵に触れたら、攻撃の移動速度を遅くする
					if (moveSpeed_ > 0.2f)currentMoveSpeed_ = 0.2f;

					// この攻撃でヒットしたターゲットをリストに追加する
					state.hitCharacters.push_back(target);
				}
			}
		} 
		else
		{
			// 攻撃の時間帯を過ぎたら当たり判定を削除する
			state.DeleteHitbox();
		}
	}

	// 攻撃の特定の時間帯は移動する
	if(attackTimer_ >= moveStartTime_ && attackTimer_ <= moveEndTime_)
	{
		Vector3 direction = owner_->GetDirection();
		Vector3 position = owner_->GetPosition();
		position += (currentMoveSpeed_ * owner_->RageModeAttackSpeed()) * (direction * engine_->GetDeltaTime() * engine_->GetTimeScale());
		owner_->SetPosition(position);
	}

	// 攻撃時間が経過したら基底の更新処理を呼び出す
	if (attackTimer_ >= attackTime_)
	{
		Attack::Update();
	}
}

/// @brief リセット
void ComboAttack::Reset()
{
	// ブレイクポイントのチェック
	BreakpointOnReset();

	// 基底のリセット
	Attack::Reset();

	// 攻撃タイマーを初期化する
	attackTimer_ = 0.0f;

	// すべての判定をリセット・削除する
	for (auto& state : hitStates_)
	{
		state.hitCharacters.clear();
		state.DeleteHitbox();
	}
}

/// @brief 終了、中断
void ComboAttack::Exit()
{
	// ブレイクポイントのチェック
	BreakpointOnExit();

	// すべての判定をリセット・削除する
	for (auto& state : hitStates_)
	{
		state.hitCharacters.clear();
		state.DeleteHitbox();
	}

	// 基底の終了処理
	Attack::Exit();
}

/// @brief 次の攻撃に移行できるかどうか
/// @return 
bool ComboAttack::IsCanNextCombo()const
{
	// プレイヤー以外のキャラクターはコンボキャンセルできないと仮定する
	if (!owner_->IsPlayer())return false;

	// 攻撃タイマーがコンボキャンセル受付時間内であれば、次の攻撃に移行できると仮定する
	return !IsExec() || (attackTimer_ >= cancelStartTime_ && attackTimer_ <= cancelEndTime_); 
}

/// @brief 攻撃中かどうか
/// @return 
bool ComboAttack::IsGrabWeapon() const
{ 
	return isGrabWeapon_ && IsExec() && !owner_->GetWeapon() ? attackTimer_ >= grabWeaponStartTime_ && attackTimer_ <= grabWeaponEndTime_ : false;
}