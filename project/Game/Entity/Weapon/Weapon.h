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

		/// @brief 壊れない武器かどうか
		bool isUnbreakable = false;

		/// @brief モデル
		Render3DStaticModel* model = nullptr;

		/// @brief 着地判定グループ
		Collision3DInstanceAABB* landingCollision = nullptr;
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

	/// @brief 所持されているかどうか
	/// @return 
	bool IsEquipped() const { return owner_ != nullptr; }

	/// @brief 武器に入るダメージ
	/// @param damage 
	void TakeDamage(int damage);

	/// @brief 有効かどうかを設定する
	/// @param isActive 
	void SetActive(bool isActive) { isActive_ = isActive; }

	/// @brief 有効かどうかを取得する
	/// @return 
	bool IsActive() const { return isActive_; }


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

	/// @brief 壊れない武器かどうか
	bool isUnbreakable_ = false;

	/// @brief 有効かどうか
	bool isActive_ = true;


protected:

	/// @brief 落下の更新
	/// @param deltaTime 
	void FallUpdate(float deltaTime);

	/// @brief 着地判定の更新
	void LandingCheck();

	/// @brief 着地判定
	Collision3DInstanceAABB* landingCollision_ = nullptr;

	// 現在の落下速度
	float velocityY_ = 0.0f;

	// 地面に接地しているかどうか
	bool isGrounded_ = false;

	// 重力加速度
	const float kGravity = -9.8f;

	// 最大落下速度
	const float kMaxFallSpeed = -20.0f;
};

