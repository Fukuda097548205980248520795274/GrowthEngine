#pragma once
#include "GrowthEngine.h"
#include <set>
#include <unordered_map>

class Character;
class Player;
class NavMesh;

// @brief トークンの種類
enum class ActionTokenType
{
	// 攻撃
	Attack,

	// 挑発
	Taunt,

	// フェイント
	Feint
};

// @brief トークンのデータ
struct TokenData
{
	/// @brief ターゲット
	Character* target;

	/// @brief トークンの種類
	ActionTokenType type;
};

/// @brief 戦闘スロットの情報
struct CombatSlot
{
	/// @brief 占有しているかどうか
	bool isOccupied = false;

	/// @brief 占有しているキャラクター
	Character* occupant = nullptr;

	// @brief ターゲットに対する角度オフセット（ラジアン）
	float angleOffset = 0.0f;

	// @brief ターゲットとの距離
	float distance = 6.0f;
};

class BattleDirector
{
public:

	/// @brief インスタンスを取得する
	/// @return 
	static BattleDirector& GetInstance();

	/// @brief インスタンスを削除する
	static void Delete();

	/// @brief 攻撃トークンを要求する
	/// @param npc 
	/// @param type 
	/// @return 
	bool RequestAttackToken(Character* npc, ActionTokenType type);

	/// @brief 攻撃トークンを返却する
	/// @param npc 
	void ReleaseAttackToken(Character* npc);

	/// @brief クリア処理
	void Clear();

	/// @brief 更新処理
	/// @param dt 
	void Update(float dt);

	/// @brief 指定されたNPCが最も攻撃に適しているかどうかを判定する
	/// @param NPC 
	/// @return 
	bool IsBestAttacker(Character* NPC);

	/// @brief カメラの前方向を設定する
	/// @param forward 
	void SetCameraForward(const Vector3& forward) { cameraForward_ = forward; }

	/// @brief NPCにスロットを割り当てる
	/// @param npc 
	/// @param target 
	void AssignSlot(Character* npc, Character* target);

	/// @brief NPCのスロットを解放する
	/// @param npc 
	void ReleaseSlot(Character* npc);

	/// @brief NPCのスロットのワールド座標を取得する
	/// @param npc 
	/// @param target 
	/// @return 
	std::optional<Vector3> GetSlotWorldPosition(Character* npc, Character* target);

	/// @brief ターゲットされているNPCの数を取得する
	/// @param target 
	/// @return 
	int GetTargetingCount(Character* target);

	/// @brief ターゲットごとの使用中スロット数を取得する
	/// @param target 
	/// @return 
	int GetUsedSlotCount(Character* target);

	/// @brief スロットの占有者を取得する
	/// @param target 
	/// @param slotIndex 
	/// @return 
	Character* GetSlotOccupant(Character* target, int slotIndex);


private:

	// シングルトンのためコンストラクタはprivate
	BattleDirector() = default;

	/// @brief 最大トークン数
	std::unordered_map<ActionTokenType, size_t> maxTokens_ = { {ActionTokenType::Attack, 1},{ActionTokenType::Taunt, 2},{ActionTokenType::Feint, 1} };

	/// @brief ターゲットごとの攻撃トークンの保持者
	std::unordered_map<Character*, std::unordered_map<ActionTokenType, std::set<Character*>>> targetTokenHolders_;

	/// @brief NPCとそのターゲットのマッピング
	std::unordered_map<Character*, TokenData> npcToTargetMap_;


private:

	/// @brief 戦闘スロットを最適化する
	void OptimizeSlots();

	/// @brief ターゲットごとの戦闘スロットの情報
	std::unordered_map<Character*, std::vector<CombatSlot>> targetSlots_;

	/// @brief NPCごとの現在のスロット番号
	std::unordered_map<Character*, int> npcCurrentSlots_;

	/// @brief 最大スロット数
	static constexpr int kMaxSlots = 8;


private:

	/// @brief 攻撃トークンのユーティリティスコアを計算する
	/// @param attacker 
	/// @param target 
	/// @return 
	float CalculateUtilityScore(Character* attacker, Character* target);

	/// @brief ターゲットごとの攻撃トークンのクールダウン時間
	std::unordered_map<Character*, float> targetTokenCooldowns_;


private:

	/// @brief 戦況のテンションを更新する
	/// @param player 
	void UpdateTension(Player* player);

	/// @brief 戦況のテンション
	float globalTension_ = 1.0f;

	/// @brief プレイヤーの体力が少ないときのテンションの減少率
	float lowHpTensionMultiplier_ = 0.5f;

	/// @brief プレイヤーの体力が中程度のときのテンションの増加率
	float mediumHpTensionMultiplier_ = 0.8f;

	/// @brief プレイヤーの体力が多いときのテンションの増加率
	float highHpTensionMultiplier_ = 1.0f;

	/// @brief カメラの前方向
	Vector3 cameraForward_ = { 0.0f, 0.0f, 1.0f };
};

