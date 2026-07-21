#include "CharacterState.h"
#include "Entity/Character/NPC/NPC.h"
#include "Entity/Character/Player/Player.h"

/// @brief ツリーのリクエスト
void CharacterState::TreeRequest()
{
	// 所有者がプレイヤーでない場合、NPCにビヘイビアツリーの変更をリクエストする
	if (!owner_->IsPlayer())
	{
		NPC* npc = static_cast<NPC*>(owner_);
		npc->RequestBehaviorTreeChange(behaviorTree_.get());
	}
	else
	{
		// 所有者がプレイヤーの場合、プレイヤーにコンボツリーの変更をリクエストする
		Player* player = static_cast<Player*>(owner_);
		player->RequestComboTreeChange(comboTreeX_.get(), comboTreeY_.get(), comboTreeB_.get());
	}
}

/// @brief コンボツリーを設定する
/// @param comboTreeX 
/// @param comboTreeY 
/// @param comboTreeB 
void CharacterState::SetComboTree(std::unique_ptr<ComboTree> comboTreeX, std::unique_ptr<ComboTree> comboTreeY, std::unique_ptr<ComboTree> comboTreeB)
{
	comboTreeX_ = std::move(comboTreeX);
	comboTreeY_ = std::move(comboTreeY);
	comboTreeB_ = std::move(comboTreeB);
}

/// @brief ビヘイビアツリーが設定されていなかったときの処理
void CharacterState::HandleBehaviorTreeNotSet()
{
	// 所有者がNPCの場合、ビヘイビアツリーを設定する
	if (!owner_->IsPlayer())
	{
		NPC* npc = static_cast<NPC*>(owner_);
		BehaviorTree* currentTree = npc->GetBehaviorTree();
		if (!currentTree && behaviorTree_)
		{
			npc->SetBehaviorTree(behaviorTree_.get());
		}
	}
	else if(owner_->IsPlayer())
	{
		// 所有者がプレイヤーの場合、コンボツリーを設定する
		Player* player = static_cast<Player*>(owner_);
		if(!player->GetCurrentComboTreeX() && comboTreeX_)
		{
			player->SetCurrentComboTreeX(comboTreeX_.get());
		}
		if(!player->GetCurrentComboTreeY() && comboTreeY_)
		{
			player->SetCurrentComboTreeY(comboTreeY_.get());
		}
		if(!player->GetCurrentComboTreeB() && comboTreeB_)
		{
			player->SetCurrentComboTreeB(comboTreeB_.get());
		}
	}
}