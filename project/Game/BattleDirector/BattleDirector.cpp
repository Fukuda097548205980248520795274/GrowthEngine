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
	// すでにトークンを保持しているNPCなら、要求を許可する
	if (tokenHolders_.find(npc) != tokenHolders_.end())
	{
		return true;
	}

	// トークン保持NPCの数が最大数未満なら、要求を許可してNPCをセットに追加する
	if (tokenHolders_.size() < maxAttackTokens_)
	{
		tokenHolders_.insert(npc);
		return true;
	}

	// それ以外の場合は、要求を拒否する
	return false;
}

/// @brief 攻撃トークンを返却する
/// @param npc 
void BattleDirector::ReleaseAttackToken(Character* npc)
{
	tokenHolders_.erase(npc);
}