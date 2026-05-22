#include "Weapon.h"
#include "Entity/Character/Character.h"

#include <numbers>

// 武器インスタンスの共有リスト
std::vector<Weapon*> Weapon::weapons_{};

/// @brief コンストラクタ
/// @param initData 
Weapon::Weapon(const InitData& initData) : Entity()
{
	// インスタンスリストに登録する
	weapons_.push_back(this);

	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

	// 位置
	worldTransform_->translate_ = initData.position;

	// 耐久力
	durability_ = initData.durability;

	// 攻撃力
	attackPower_ = initData.attackPower;

	// カテゴリ
	category_ = initData.category;

	// 壊れない武器かどうか
	isUnbreakable_ = initData.isUnbreakable;

	// 着地判定
	if (initData.landingCollision)
	{
		landingCollision_ = initData.landingCollision;

		// コリジョンの位置を武器の位置に設定する
		landingCollision_->param_->center = GetWorldPosition();
		landingCollision_->param_->radius = Vector3(0.05f, 0.05f, 0.05f); 
	}

	// モデル
	if (initData.model)
	{
		model_ = initData.model;

		// ワールドトランスフォームの親をモデルに設定する
		model_->SetParent(worldTransform_.get());
	}
}

/// @brief デストラクタ
Weapon::~Weapon()
{
	// 当たり判定を削除する
	if (landingCollision_)landingCollision_->Delete();
	landingCollision_ = nullptr;

	// インスタンスリストから自分を除外する
	auto it = std::remove(weapons_.begin(), weapons_.end(), this);
	weapons_.erase(it, weapons_.end());
}

/// @brief 更新処理
void Weapon::Update()
{
	// 有効でないときは更新しない
	if (!isActive_)return;

	// デルタタイムを取得する
	float dt = engine_->GetDeltaTime();

	if (!isBreak_)
	{
		// 着地しているかどうか
		LandingCheck();

		// 持ち主がいるときは持ち主の位置に追従する
		if (IsEquipped())
		{
			// カテゴリに応じてトランスフォームを調整する
			switch (category_)
			{
				// 片手武器
			case WeaponCategory::OneHanded:

				// 90度回転させる
				worldTransform_->rotate_.x = std::numbers::pi_v<float> / 2.0f;

				// ワールドトランスフォームの親を持ち主の右手に設定する
				worldTransform_->SetParent(owner_->GetBoneMatrix(JointType::HandR));

				break;


				// 両手武器
			case WeaponCategory::TwoHanded:

				break;
			}
		}

		// 落下の更新
		FallUpdate(dt);

		// 基底クラスの更新
		Entity::Update();

		// 耐久力が0以下で壊れない武器でないときは壊れる
		if (durability_ <= 0 && !isUnbreakable_)
		{
			isBreak_ = true;

			// 当たり判定を削除する
			if (landingCollision_)landingCollision_->Delete();
			landingCollision_ = nullptr;

			return;
		}

		// 着地判定の位置を更新する
		if (landingCollision_)landingCollision_->param_->center = GetWorldPosition();
	}
}

/// @brief 描画処理
void Weapon::Draw()
{
	// 有効でないときは描画しない
	if (!isActive_)return;

	// 壊れている武器は描画しない
	if (isBreak_) return;

	// モデルがあるときは描画する
	if (model_)model_->Draw();
}

/// @brief 武器に入るダメージ
/// @param damage 
void Weapon::TakeDamage(int damage)
{
	// 壊れない武器なら、耐久力を減らさない
	if (isUnbreakable_) return;

	// 耐久力を減らす
	durability_ -= damage;
}

/// @brief 落下の更新
/// @param deltaTime 
void Weapon::FallUpdate(float deltaTime)
{
	// 重力による落下処理
	if (!isGrounded_)
	{
		// 落下速度を更新する
		velocityY_ += kGravity * deltaTime;
		if (velocityY_ < kMaxFallSpeed) velocityY_ = kMaxFallSpeed;

		// Y方向の位置を更新する
		worldTransform_->translate_.y += velocityY_ * deltaTime;
	}
}

/// @brief 着地判定の更新
void Weapon::LandingCheck()
{
	// 着地しているかどうかのフラグをリセットする
	isGrounded_ = false;

	// 所持者がいる場合、着地している
	if (IsEquipped())
	{
		isGrounded_ = true;

		// Y方向の速度をリセットする（着地したので落下を止める）
		velocityY_ = 0.0f;

		return;
	}

	// コリジョンがないと処理しない
	if (!landingCollision_)return;

	// コリジョンの状態を確認する
	if (landingCollision_->isCollision_)
	{
		// コリジョンの当たり判定がAABBであることを前提に、床との接触位置を計算する
		auto floorCollision = static_cast<Collision3DInstanceAABB*>(landingCollision_->hitOpponent_);
		worldTransform_->translate_.y = floorCollision->param_->center.y + floorCollision->param_->radius.y;

		// 着地していると判定する
		isGrounded_ = true;

		// Y方向の速度をリセットする（着地したので落下を止める）
		velocityY_ = 0.0f;
	}
}