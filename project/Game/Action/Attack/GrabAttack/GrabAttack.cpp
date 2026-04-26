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
	partName_ = initData.grabPartName;
	hitboxStartTime_ = initData.hitboxStartTime;
	hitboxEndTime_ = initData.hitboxEndTime;

	grabMaxTime_ = initData.grabTime;
	damage_ = 0;
	staggerTime_ = 0.0f;
	knockback_ = 0.0f;
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
	if (!hasHit_)
	{
		// 判定の発生時間内かチェック
		if (attackTimer_ >= hitboxStartTime_ && attackTimer_ <= hitboxEndTime_)
		{
			if (hitbox_.collider_ == nullptr)
				hitbox_.collider_ = owner_->GetHitboxGroup()->CreateInstance();

			// 指定ボーンに当たり判定を追従させる
			if (!partName_.empty())
			{
				auto sphere = static_cast<Collision3DInstanceSphere*>(hitbox_.collider_);
				Matrix4x4 boneMatrix = owner_->GetBoneMatrix(partName_);
				sphere->param_->center = Vector3(boneMatrix.m[3][0], boneMatrix.m[3][1], boneMatrix.m[3][2]);
				sphere->param_->radius = 0.5f; // つかみ判定は少し大きめに設定
			}

			// 当たり判定チェック
			for (Character* target : Character::GetCharacters())
			{
				// 自分自身、味方、死体は除外
				if (target == owner_ || target->GetCharacterTag() == owner_->GetCharacterTag() || target->IsDead()) continue;

				if (hitbox_.IsHit()) // ※実際の衝突判定処理に合わせてください
				{
					// 【つかみ成功】
					hasHit_ = true;
					DeleteHitbox();

					// Characterクラスへ状態を移行する（時間はCharacter側で管理するか、引数で渡す）
					owner_->ExecuteGrab(target , grabMaxTime_);

					// アクションを即座に終了させ、Character::Update のホールド処理に移行させる
					Attack::Update();
					return;
				}
			}
		}
	}
	else if (!hasHit_ && attackTimer_ > hitboxEndTime_)
	{
		// 発生時間を過ぎたら判定を消す（空振り）
		DeleteHitbox();
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