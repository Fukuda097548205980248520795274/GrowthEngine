#include "Item.h"

// アイテムインスタンスの共有リスト
std::vector<Item*> Item::items_{};

/// @brief コンストラクタ
/// @param initData 
Item::Item(const InitData& initData)
{
	// インスタンスリストに登録する
	items_.push_back(this);

	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();
}

/// @brief デストラクタ
Item::~Item()
{
	// インスタンスリストから自分を除外する
	auto it = std::remove(items_.begin(), items_.end(), this);
	items_.erase(it, items_.end());
}

/// @brief 更新処理
void Item::Update()
{
	// 耐久力が0以下なら壊れる
	if (durability_ <= 0)
	{
		isBreak_ = true;
		return;
	}

	// 基底クラスの更新
	Entity::Update();
}