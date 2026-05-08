#pragma once
#include "../Entity.h"

class Item : public Entity
{
public:

	struct InitData
	{
		// 位置
		Vector3 position = Vector3(0.0f, 0.0f, 0.0f);
	};

public:

	/// @brief コンストラクタ
	/// @param initData 
	Item(const InitData& initData);

	/// @brief デストラクタ
	virtual ~Item() override;

	/// @brief 更新処理
	void Update() override;

	/// @brief 壊れたかどうか
	/// @return 
	bool IsBreak() const { return isBreak_; }


protected:

	// エンジン
	const GrowthEngine* engine_ = nullptr;

	/// @brief アイテムのリスト
	static std::vector<Item*> items_;

	// アイテムが壊れたかどうか
	bool isBreak_ = false;


protected:

	// 耐久力
	int durability_ = 0;

	// 攻撃力
	float attackPower_ = 0;
};

