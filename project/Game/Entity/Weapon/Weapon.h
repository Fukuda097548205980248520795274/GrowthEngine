#pragma once
#include "../Entity.h"

class Character;

enum class WeaponCategory
{
	None,
	OneHanded, // 片手武器
	TwoHanded, // 両手武器
};

class Weapon : public Entity
{
public:

	struct InitData
	{
		// 位置
		Vector3 position = Vector3(0.0f, 0.0f, 0.0f);

		// 耐久力
		int durability = 100;

		/// @brief 攻撃力
		float attackPower = 1.0f;

		WeaponCategory category = WeaponCategory::None;

		/// @brief モデル
		Render3DStaticModel* model = nullptr;
	};

public:

	/// @brief コンストラクタ
	/// @param initData 
	Weapon(const InitData& initData);

	/// @brief デストラクタ
	virtual ~Weapon() override;

	/// @brief 更新処理
	void Update() override;

	/// @brief 描画処理
	void Draw();

	/// @brief 壊れたかどうか
	/// @return 
	bool IsBreak() const { return isBreak_; }

	/// @brief 耐久力を取得する
	/// @return 
	int GetDurability() const { return durability_; }

	/// @brief 攻撃力を取得する
	/// @return 
	float GetAttackPower() const { return attackPower_; }

	/// @brief 武器のカテゴリを取得する
	/// @return 
	WeaponCategory GetCategory() const { return category_; }

	/// @brief 所持者を設定する
	/// @param owner 
	void SetOwner(Character* owner) { owner_ = owner; }


protected:

	// エンジン
	const GrowthEngine* engine_ = nullptr;

	/// @brief モデル
	Render3DStaticModel* model_ = nullptr;

	/// @brief 武器のリスト
	static std::vector<Weapon*> weapons_;

	// 武器が壊れたかどうか
	bool isBreak_ = false;

	// 武器のカテゴリ
	WeaponCategory category_ = WeaponCategory::None;

	// 耐久力
	int durability_ = 0;

	// 攻撃力
	float attackPower_ = 1.0f;

	// 武器の持ち主
	Character* owner_ = nullptr;
};

