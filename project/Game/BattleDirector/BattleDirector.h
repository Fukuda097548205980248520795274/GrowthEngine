#pragma once
#include "GrowthEngine.h"
#include <set>

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

private:

	// シングルトンのためコンストラクタはprivate
	BattleDirector() = default;

	// 最大攻撃トークン数
	size_t maxAttackTokens_ = 1;

	// 攻撃トークンを保持しているNPCのセット
	std::set<Character*> tokenHolders_;
};

