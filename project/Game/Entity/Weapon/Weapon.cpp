#include "Weapon.h"
#include "Entity/Character/Character.h"

// 武器インスタンスの共有リスト
std::vector<Weapon*> Weapon::weapons_{};

/// @brief コンストラクタ
/// @param initData 
Weapon::Weapon(const InitData& initData)
{
	// インスタンスリストに登録する
	weapons_.push_back(this);

	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();
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