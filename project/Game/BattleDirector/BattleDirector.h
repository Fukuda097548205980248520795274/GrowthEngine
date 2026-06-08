#pragma once
#include "GrowthEngine.h"
#include <set>
#include <unordered_map>

class Character;

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

private:

	// シングルトンのためコンストラクタはprivate
	BattleDirector() = default;

	// 最大攻撃トークン数
	size_t maxAttackTokens_ = 1;

	/// @brief 攻撃トークンを保持しているキャラクターのセット
	std::unordered_map<Character*, std::set<Character*>> targetTokenHolders_;

	/// @brief NPCとそのターゲットのマッピング
	std::unordered_map<Character*, Character*> npcToTargetMap_;
};

