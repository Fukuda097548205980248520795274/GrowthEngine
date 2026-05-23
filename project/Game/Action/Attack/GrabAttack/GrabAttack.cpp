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
	attackTimer_ += engine_->GetDeltaTime();

	// すでにつかんでいる、またはつかまれている状態なら、攻撃処理は行わずに終了する
	if (owner_->IsGrabbed() || owner_->IsGrabbing())
	{
		Attack::Update();
		return;
	}

	// ------------------------------------------
	// 1. つかみ判定の処理
	// ------------------------------------------

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

				// 当たり判定チェック
				for (Character* target : Character::GetCharacters())
				{
					// 自分自身は判定しない
					if (target == owner_) continue;

					// 同じ陣営は判定しない（例：プレイヤー側の攻撃はプレイヤー側には当たらない）
					if (target->GetCharacterTag() == owner_->GetCharacterTag())continue;

					// 死亡しているキャラクターは判定しない
					if (target->IsDead())continue;

					if (hitbox_.IsHit()) // ※実際の衝突判定処理に合わせてください
					{
						// 【つかみ成功】
						hasHit_ = true;
						DeleteHitbox();

						// Characterクラスへ状態を移行する（時間はCharacter側で管理するか、引数で渡す）
						owner_->ExecuteGrab(target, grabMaxTime_);

						// アクションを即座に終了させ、Character::Update のホールド処理に移行させる
						Attack::Update();
						return;
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

	// ------------------------------------------
	// 2. 踏み込み移動の処理
	// ------------------------------------------
	if (attackTimer_ >= moveStartTime_ && attackTimer_ <= moveEndTime_)
	{
		Vector3 direction = owner_->GetDirection();
		Vector3 position = owner_->GetPosition();
		position += moveSpeed_ * (direction * engine_->GetDeltaTime());
		owner_->SetPosition(position);
	}

	// ------------------------------------------
	// 3. アクション終了処理（空振り時）
	// ------------------------------------------
	if (attackTimer_ >= attackTime_)
	{
		Attack::Update(); // Action::Update()が呼ばれ、終了処理へ
	}
}

/// @brief アクションが成功したとき、または再度実行されたときに呼ばれる
void GrabAttack::Reset()
{
	Action::Reset();
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