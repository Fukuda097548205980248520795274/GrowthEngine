#include "GrabAttack.h"
#include "Entity/Character/Character.h"

GrabAttack::GrabAttack(Character* character, const GrabAttackInitData& initData)
	: Attack(character)
{
	hAttackMotion_ = initData.hAttackMotion;
	attackTime_ = initData.attackTime;
	moveSpeed_ = initData.moveSpeed;
	moveStartTime_ = initData.moveStartTime;
	moveEndTime_ = initData.moveEndTime;
	jointType_ = initData.jointType;
	hitboxStartTime_ = initData.hitboxStartTime;
	hitboxEndTime_ = initData.hitboxEndTime;
	grabMaxTime_ = initData.grabTime;

	// 攻撃の種類を掴みに設定する
	attackType_ = AttackType::Grab;
}

/// @brief 実行
void GrabAttack::Exec()
{
	// 基底クラスの実行処理
	Attack::Exec();

	// 攻撃モーションに切り替え
	owner_->SetAnimation(hAttackMotion_, true, false);

	// タイマーリセット
	attackTimer_ = 0.0f;

	// つかみ判定は攻撃の中盤～終盤に出すのが自然なので、最初は当たり判定なしの状態にしておく
	hasHit_ = false;
	hasThrown_ = false;
	grabbedTarget_ = nullptr;
}

/// @brief 更新処理
void GrabAttack::Update()
{
	attackTimer_ += engine_->GetDeltaTime() * engine_->GetTimeScale();

	// すでにつかんでいる、またはつかまれている状態なら、攻撃処理は行わずに終了する
	if ((!owner_->IsPlayer() && owner_->IsGrabbing()) || owner_->IsGrabbed())
	{
		Attack::Update();
		return;
	}

	if (owner_->GetGrabTarget())
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

	
	if (!owner_->GetGrabTarget())
	{
		// JointType::None なら当たり判定を出さない
		if (jointType_ != JointType::None && jointType_ != JointType::Weapon)
		{
			// 判定の発生時間内かチェック
			if (attackTimer_ >= hitboxStartTime_ && attackTimer_ <= hitboxEndTime_)
			{
				if (!hasHit_)
				{
					// 当たり判定がまだ生成されていない場合は生成する
					if (hitbox_.collider_ == nullptr)
						hitbox_.collider_ = owner_->GetHitboxGroup()->CreateInstance();

					// 指定ボーンに当たり判定を追従させる
					auto sphere = static_cast<Collision3DInstanceSphere*>(hitbox_.collider_);
					Matrix4x4 boneMatrix = owner_->GetBoneMatrix(jointType_);
					sphere->param_->center = Vector3(boneMatrix.m[3][0], boneMatrix.m[3][1], boneMatrix.m[3][2]);
					sphere->param_->radius = 0.25f;


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

					// 当たり判定チェック
					for (Character* target : Character::GetCharacters())
					{
						// 自分自身は判定しない
						if (target == owner_) continue;

						// 同じ側のキャラクターは判定しない（プレイヤー側ならプレイヤー側のキャラクターは判定しない、敵側なら敵側のキャラクターは判定しない）
						if (target->IsPlayerSide() == owner_->IsPlayerSide() && target->IsEnemySide() == owner_->IsEnemySide())continue;

						// 死亡しているキャラクターは判定しない
						if (target->IsDead())continue;

						// ヒットしたかどうか
						if (IsSphereHit(hitbox_.collider_, target->GetHurtboxChest().collider_) ||
							IsSphereHit(hitbox_.collider_, target->GetHurtboxHead().collider_) ||
							IsSphereHit(hitbox_.collider_, target->GetHurtboxRoot().collider_))
						{
							// つかみ成功
							hasHit_ = true;
							DeleteHitbox();

							// Characterクラスへ状態を移行する（時間はCharacter側で管理するか、引数で渡す）
							owner_->ExecuteGrab(target, grabMaxTime_);

							// プレイヤー以外は掴んだら処理が成功
							if (!owner_->IsPlayer())
							{
								Attack::Update();
								return;
							}

							// プレイヤーは掴んだら処理を終了せず、掴んだ状態で次の攻撃入力を待つ
							DeleteHitbox();
							break;
						}
					}
				}
			} 
			else
			{
				// 発生時間を過ぎたら判定を消す（空振り）
				DeleteHitbox();
			}
		}
	}

	// つかみが成功していない状態で、回避や気絶などの状態になったらアクションを終了する
	if (owner_->IsJustAvoided() || owner_->IsIncapacitated())
	{
		this->Exit();
		return;
	}

	/// @brief つかみが成功していない状態で、つかまれたらアクションを終了する
	if (owner_->GetGrabTarget())return;

	// 攻撃の特定の時間帯は移動する
	if (attackTimer_ >= moveStartTime_ && attackTimer_ <= moveEndTime_)
	{
		Vector3 direction = owner_->GetDirection();
		Vector3 position = owner_->GetPosition();
		position += moveSpeed_ * (direction * engine_->GetDeltaTime() * engine_->GetTimeScale());
		owner_->SetPosition(position);
	}

	// 攻撃時間が経過したら基底の更新処理を呼び出す
	if (attackTimer_ >= attackTime_)
	{
		Attack::Update(); // Action::Update()が呼ばれ、終了処理へ
	}
}

/// @brief アクションが成功したとき、または再度実行されたときに呼ばれる
void GrabAttack::Reset()
{
	Attack::Reset();
	attackTimer_ = 0.0f;
	hasHit_ = false;
	DeleteHitbox();
}

/// @brief アクションが終了したとき、または中断されたときに呼ばれる
void GrabAttack::Exit()
{
	DeleteHitbox();
	Attack::Exit();
}

/// @brief 攻撃判定を削除する
void GrabAttack::DeleteHitbox()
{
	if (hitbox_.collider_ != nullptr)
	{
		hitbox_.collider_->Delete();
		hitbox_.collider_ = nullptr;
	}
}