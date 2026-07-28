#pragma once
#include "GrowthEngine.h"
#include <set>
#include <unordered_map>

class Character;
class Player;

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
	Character* target;
	ActionTokenType type;
};

class BattleDirector
{
public:

	/// @brief インスタンスを取得する
	/// @return 
	static BattleDirector& GetInstance();

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
};

