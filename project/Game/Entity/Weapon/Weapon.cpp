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
	// インスタンスリストから自分を除外する
	auto it = std::remove(weapons_.begin(), weapons_.end(), this);
	weapons_.erase(it, weapons_.end());
}

/// @brief 更新処理
void Weapon::Update()
{
	// 持ち主がいるときは持ち主の位置に追従する
	if (owner_)
	{
		switch (category_)
		{
		case WeaponCategory::OneHanded:
			worldTransform_->rotate_.x = std::numbers::pi_v<float> / 2.0f; // 90度回転させる
			break;

		case WeaponCategory::TwoHanded:

			break;
		}

		// ワールドトランスフォームの親を持ち主の右手に設定する
		worldTransform_->SetParent(owner_->GetBoneMatrix(JointType::HandR));
	}

	// 耐久力が0以下なら壊れる
	if (durability_ <= 0)
	{
		isBreak_ = true;
		return;
	}

	// 基底クラスの更新
	Entity::Update();
}

/// @brief 描画処理
void Weapon::Draw()
{
	// モデルがあるときは描画する
	if (model_)model_->Draw();
}