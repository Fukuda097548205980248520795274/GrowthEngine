#include "BattleDirector.h"
#include "Entity/Character/Character.h"

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
	// NPCが現在狙っているターゲットを取得
	Character* target = npc->GetLockOnTarget();

	// ターゲットがいない場合は、攻撃トークンの要求を許可する
	if (!target)return true;

	// 現在の攻撃トークン保持者を取得
	auto it = npcToTargetMap_.find(npc);
	if (it != npcToTargetMap_.end())
	{
		// すでに攻撃トークンを保持している場合は、同じターゲットに対する要求かどうかを確認する
		if (it->second == target)return true;

		// 別のターゲットに対する要求の場合は、現在の攻撃トークンを返却する
		ReleaseAttackToken(npc);
	}

	// 現在の攻撃トークン保持者を取得
	auto& holders = targetTokenHolders_[target];

	// 攻撃トークンの最大数に達していない場合は、攻撃トークンを許可する
	if (holders.size() < maxAttackTokens_)
	{
		// 攻撃トークンを保持しているキャラクターのセットにNPCを追加する
		holders.insert(npc);

		// NPCとそのターゲットのマッピングを更新する
		npcToTargetMap_[npc] = target;
		return true;
	}

	// 攻撃トークンの最大数に達している場合は、攻撃トークンの要求を拒否する
	return false;
}

/// @brief 攻撃トークンを返却する
/// @param npc 
void BattleDirector::ReleaseAttackToken(Character* npc)
{
	// NPCが現在狙っているターゲットを取得
	auto it = npcToTargetMap_.find(npc);
	if (it == npcToTargetMap_.end())return;

	// 現在の攻撃トークン保持者を取得
	Character* target = it->second;

	// NPCとそのターゲットのマッピングを削除する
	auto targetIt = targetTokenHolders_.find(target);
	if (targetIt != targetTokenHolders_.end())
	{
		targetIt->second.erase(npc);

		// NPCとそのターゲットのマッピングを削除する
		if (targetIt->second.empty())
			targetTokenHolders_.erase(targetIt);
	}

	// NPCとそのターゲットのマッピングを削除する
	npcToTargetMap_.erase(it);
}