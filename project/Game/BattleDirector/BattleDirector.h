#pragma once
#include "GrowthEngine.h"
#include <set>
#include <unordered_map>

class Character;
class Player;

class BattleDirector
{
public:

	/// @brief インスタンスを取得する
	/// @return 
	static BattleDirector& GetInstance();

	/// @brief 攻撃トークンを要求する
	/// @param npc 
	/// @return 
	bool RequestAttackToken(Character* npc);

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

	// 最大攻撃トークン数
	size_t maxAttackTokens_ = 1;

	/// @brief 攻撃トークンを保持しているキャラクターのセット
	std::unordered_map<Character*, std::set<Character*>> targetTokenHolders_;

	/// @brief NPCとそのターゲットのマッピング
	std::unordered_map<Character*, Character*> npcToTargetMap_;


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

