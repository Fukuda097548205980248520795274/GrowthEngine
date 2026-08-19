#pragma once
#include "../Entity.h"
#include "BehaviorTree/BehaviorTree.h"
#include "ComboTree/ComboTree.h"

class Character;
class WeaponGetButton;

enum class WeaponCategory
{
	None,
	OneHanded, // 片手武器
	TwoHanded, // 両手武器
	Gun, // 銃
};

// @brief 武器の状態ツリーセット
struct WeaponStateTreeSet
{
	// Player用
	std::shared_ptr<ComboTree> comboTreeX = nullptr;
	std::shared_ptr<ComboTree> comboTreeY = nullptr;
	std::shared_ptr<ComboTree> comboTreeB = nullptr;

	// NPC用
	std::shared_ptr<BehaviorTree> behaviorTree = nullptr;
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
		PrefabInstanceStaticModel* model = nullptr;

		/// @brief 着地判定グループ
		Collision3DInstanceCapsule* landingCollision = nullptr;

		/// @brief 武器取得ボタン
		WeaponGetButton* button = nullptr;
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
	void SetOwner(Character* owner);

	/// @brief 所持されているかどうか
	/// @return 
	bool IsEquipped() const { return owner_ != nullptr; }

	/// @brief 武器に入るダメージ
	/// @param damage 
	void TakeDamage(int damage);

	/// @brief 有効かどうかを設定する
	/// @param isActive 
	void SetActive(bool isActive) { isActive_ = isActive; }

	/// @brief 耐久力を設定する
	/// @param durability 
	void SetDurability(int durability) { durability_ = durability; maxDurability_ = std::max(maxDurability_, durability); }

	/// @brief 最大耐久力を取得する
	/// @return 
	int GetMaxDurability() const { return maxDurability_; }

	/// @brief 攻撃力を設定する
	/// @param attackPower 
	void SetAttackPower(float attackPower) { attackPower_ = attackPower; }

	/// @brief 壊れない武器かどうかを設定する
	/// @param isUnbreakable 
	void SetIsUnbreakable(bool isUnbreakable) { isUnbreakable_ = isUnbreakable; }

	/// @brief 有効かどうかを取得する
	/// @return 
	bool IsActive() const { return isActive_; }

	/// @brief 武器を吹き飛ばす
	/// @param velocity 
	void BlowAway(const Vector3& velocity);

	/// @brief 全武器のリストを取得する
	/// @return 
	static const std::vector<Weapon*>& GetWeapons() { return weapons_; }

	/// @brief 武器の状態ツリーセットを設定する
	/// @param stateName 
	/// @param treeSet 
	void SetStateTreeSet(const std::string& stateName, const WeaponStateTreeSet& treeSet) { stateTrees_[stateName] = treeSet; }

	/// @brief 武器の状態ツリーセットを取得する
	/// @param stateName 
	/// @return 
	WeaponStateTreeSet* GetStateTreeSet(const std::string& stateName);

	/// @brief プレイヤーが入手できる範囲にいるかどうかを設定する
	/// @param isInRange 
	void SetIsPlayerInRange(bool isInRange) { isPlayerInRange_ = isInRange; }

	/// @brief ナイフカテゴリかどうかを取得する
	/// @return 
	bool IsCategoryKnife() const { return category_ == WeaponCategory::OneHanded || category_ == WeaponCategory::TwoHanded; }

	/// @brief 銃カテゴリかどうかを取得する
	/// @return 
	bool IsCategoryGun() const { return category_ == WeaponCategory::Gun; }


protected:

	// エンジン
	const GrowthEngine* engine_ = nullptr;

	/// @brief モデル
	PrefabInstanceStaticModel* model_ = nullptr;

	/// @brief 武器のリスト
	static std::vector<Weapon*> weapons_;

	// 入手できる範囲にプレイヤーがいるかどうか
	bool isPlayerInRange_ = false;

	// 武器が壊れたかどうか
	bool isBreak_ = false;

	// 武器のカテゴリ
	WeaponCategory category_ = WeaponCategory::None;

	// 耐久力
	int durability_ = 0;

	/// @brief 最大耐久力
	int maxDurability_ = 0;

	// 攻撃力
	float attackPower_ = 1.0f;

	// 武器の持ち主
	Character* owner_ = nullptr;

	/// @brief 壊れない武器かどうか
	bool isUnbreakable_ = false;

	/// @brief 有効かどうか
	bool isActive_ = true;

	/// @brief 武器の状態ツリーセット
	std::unordered_map<std::string, WeaponStateTreeSet> stateTrees_;


protected:

	/// @brief 武器取得ボタンの更新
	void UpdateButton();

	/// @brief 武器取得ボタン
	WeaponGetButton* button_ = nullptr;


protected:

	/// @brief 落下の更新
	/// @param deltaTime 
	void FallUpdate(float deltaTime);

	/// @brief 着地判定の更新
	void LandingCheck();

	/// @brief 着地判定
	Collision3DInstanceCapsule* landingCollision_ = nullptr;

	// 現在の速度
	Vector3 velocity_ = Vector3(0.0f, 0.0f, 0.0f);

	// 地面に接地しているかどうか
	bool isGrounded_ = false;

	// 重力加速度
	const float kGravity = -9.8f;

	// 最大落下速度
	const float kMaxFallSpeed = -20.0f;


public:

	/// @brief デバッグUIを描画する
	/// @param placementData 
	/// @param placementList 
	/// @param history 
	/// @param isDirty 
	void DrawDebugUI(PlacementData* placementData, std::vector<PlacementData>& placementList, StageEditorHistory* history, bool* isDirty) override;
};

