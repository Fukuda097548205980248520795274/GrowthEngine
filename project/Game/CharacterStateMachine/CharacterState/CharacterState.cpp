#include "CharacterState.h"
#include "Entity/Character/NPC/NPC.h"

/// @brief ビヘイビアツリーのリクエスト
void CharacterState::BehaviorTreeRequest()
{
	// 所有者がプレイヤーでない場合、NPCにビヘイビアツリーの変更をリクエストする
	if (!owner_->IsPlayer())
	{
		NPC* npc = static_cast<NPC*>(owner_);
		npc->RequestBehaviorTreeChange(behaviorTree_.get());
	}
}