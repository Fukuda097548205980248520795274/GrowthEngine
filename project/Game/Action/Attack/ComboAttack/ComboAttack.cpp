#include "ComboAttack.h"
#include "Entity/Character/Character.h"
#include "Entity/Weapon/Weapon.h"

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

/// @brief 実行
void ComboAttack::Exec()
{
	// 基底の実行
	Attack::Exec();

	// アニメーションを設定する
	owner_->SetAnimation(hAttackMotion_, true , false);

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
}

/// @brief 更新処理
void ComboAttack::Update()
{
	// 攻撃中に回避、スタイルチェンジが行われた場合は、攻撃を終了する
	if(owner_->IsJustAvoided() || owner_->IsStyleChanging())
	{
		this->Exit();
		return;
	}

	// コンボキャンセル受付時間内であれば、次の攻撃への入力をチェックする
	if (attackTimer_ >= cancelStartTime_ && attackTimer_ <= cancelEndTime_)
	{
		if (owner_)
		{
			// バッファされた攻撃入力を取得する
			AttackInputType bufferedInput = owner_->GetBufferedAttackInput();

			if (bufferedInput == AttackInputType::Light && nextLightAttack_)
			{
				// ライト攻撃への移行は、ヘビー攻撃への移行よりも優先されると仮定する（両方入力されている場合はヘビー攻撃に移行する）
				owner_->ConsumeBufferedAttackInput();
				this->Exit();
				nextLightAttack_->Exec();
				return;
			}
			else if (bufferedInput == AttackInputType::Heavy && nextHeavyAttack_)
			{
				// ヘビー攻撃への移行は、ライト攻撃への移行よりも優先されると仮定する（両方入力されている場合はヘビー攻撃に移行する）
				owner_->ConsumeBufferedAttackInput();
				this->Exit();
				nextHeavyAttack_->Exec();
				return;
			}
		}
	}

	// 攻撃タイマーを更新する
	attackTimer_ += engine_->GetDeltaTime();

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
				state.hitbox.collider_ = owner_->GetHitboxGroup()->CreateInstance();

			// 当たり判定の位置とサイズを攻撃者のボーンに基づいて更新する
			auto sphere = static_cast<Collision3DInstanceSphere*>(state.hitbox.collider_);
			if (state.def.jointType == JointType::Weapon)
			{
				// 武器の当たり判定は、武器の位置とサイズを使用する
				sphere->param_->center = owner_->GetWeapon()->GetWorldPosition();
			}
			else
			{
				Matrix4x4 boneMatrix = owner_->GetBoneMatrix(state.def.jointType);
				sphere->param_->center = Vector3(boneMatrix.m[3][0], boneMatrix.m[3][1], boneMatrix.m[3][2]);
			}
			sphere->param_->radius = state.def.radius;

			// 受け流したかどうかを示すフラグ
			bool isParried = false;

			// ターゲットのリストを取得する
			for (Character* target : Character::GetCharacters())
			{
				// ターゲットが自分自身、同じ陣営、またはすでに倒れている場合はスキップする
				if (target == owner_ || target->GetCharacterTag() == owner_->GetCharacterTag() || target->IsDead()) continue;

				// すでにこの攻撃でヒットしているターゲットはスキップする
				if (std::find(state.hitCharacters.begin(), state.hitCharacters.end(), target) != state.hitCharacters.end())
					continue;

				// 当たり判定がヒットした場合の処理
				if (state.hitbox.IsHit())
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

					// ターゲットにダメージを与える
					bool isHit = target->OnDamage(state.def.damage, state.def.damageReaction, state.def.knockback, knockBackDirection, owner_->GetWorldPosition() , owner_);

					// ヒットしなかった場合は、移動速度を半減させる（ガードされた場合など）
					if (!isHit)
						currentMoveSpeed_ = moveSpeed_ * 0.5f;

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

	// 攻撃中に受け流された場合は攻撃を終了する
	if (owner_->IsParried())
	{
		this->Exit();
		return;
	}

	// 攻撃の特定の時間帯は移動する
	if(attackTimer_ >= moveStartTime_ && attackTimer_ <= moveEndTime_)
	{
		Vector3 direction = owner_->GetDirection();
		Vector3 position = owner_->GetPosition();
		position += currentMoveSpeed_ * (direction * engine_->GetDeltaTime());
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
	// 基底のリセット
	Action::Reset();

	// 攻撃タイマーを初期化する
	attackTimer_ = 0.0f;

	// すべての判定をリセット・削除する
	for (auto& state : hitStates_)
	{
		state.hitCharacters.clear();
		state.DeleteHitbox();
	}
}

/// @brief 次の攻撃があるかどうか
/// @return 
bool ComboAttack::HasNextAttack(AttackInputType inputType) const
{
	if (inputType == AttackInputType::Light)
	{
		return nextLightAttack_ != nullptr; // 弱の派生先がセットされていればtrue
	} 
	else if (inputType == AttackInputType::Heavy)
	{
		return nextHeavyAttack_ != nullptr; // 強の派生先がセットされていればtrue
	}
	return false;
}

/// @brief 終了、中断
void ComboAttack::Exit()
{
	// すべての判定をリセット・削除する
	for (auto& state : hitStates_)
	{
		state.hitCharacters.clear();
		state.DeleteHitbox();
	}

	// 基底の終了処理
	Attack::Exit();
}