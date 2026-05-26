#include "BattleDirector.h"

/// @brief インスタンスを取得する
/// @return 
BattleDirector& BattleDirector::GetInstance()
{
	static BattleDirector instance;
	return instance;
}

/// @brief 攻撃トークンを要求する
/// @param npc 
/// @return 
bool BattleDirector::RequestAttackToken(Character* npc)
{
	if (tokenHolders_.size() < maxAttackTokens_)
	{
		tokenHolders_.insert(npc);
		return true;
	}
	return false;
}

/// @brief 攻撃トークンを返却する
/// @param npc 
void BattleDirector::ReleaseAttackToken(Character* npc)
{
	tokenHolders_.erase(npc);
}