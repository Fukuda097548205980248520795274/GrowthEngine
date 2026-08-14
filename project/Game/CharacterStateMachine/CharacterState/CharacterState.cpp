#include "CharacterState.h"
#include "Entity/Character/NPC/NPC.h"
#include "Entity/Character/Player/Player.h"
#include "Entity/Weapon/Weapon.h"

/// @brief ツリーのリクエスト
void CharacterState::TreeRequest()
{
	const WeaponStateTreeSet* weaponTrees = nullptr;
	if (owner_->HasWeapon())
	{
		weaponTrees = owner_->GetWeapon()->GetStateTreeSet(name_);
	}

	// 所有者がプレイヤーでない場合、NPCにビヘイビアツリーの変更をリクエストする
	if (!owner_->IsPlayer())
	{
		NPC* npc = static_cast<NPC*>(owner_);

		// 武器の状態ツリーが存在する場合は武器のビヘイビアツリーを使用し、存在しない場合は状態のビヘイビアツリーを使用する
		BehaviorTree* tree = behaviorTree_.get();

		if (owner_->HasWeapon())
		{
			if (weaponTrees && weaponTrees->behaviorTree)tree = weaponTrees->behaviorTree.get();
			else tree = nullptr;
		}

		npc->RequestBehaviorTreeChange(tree);
	}
	else
	{
		// 所有者がプレイヤーの場合、プレイヤーにコンボツリーの変更をリクエストする
		Player* player = static_cast<Player*>(owner_);

		// 武器の状態ツリーが存在する場合は武器のコンボツリーを使用し、存在しない場合は状態のコンボツリーを使用する
		ComboTree* treeX = comboTreeX_.get();
		ComboTree* treeY = comboTreeY_.get();
		ComboTree* treeB = comboTreeB_.get();

		if (owner_->HasWeapon())
		{
			if (weaponTrees && weaponTrees->comboTreeX) treeX = weaponTrees->comboTreeX.get();
			else treeX = nullptr;
			if (weaponTrees && weaponTrees->comboTreeY) treeY = weaponTrees->comboTreeY.get();
			else treeY = nullptr;
			if (weaponTrees && weaponTrees->comboTreeB) treeB = weaponTrees->comboTreeB.get();
			else treeB = nullptr;
		}

		player->RequestComboTreeChange(treeX, treeY, treeB);
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
	const WeaponStateTreeSet* weaponTrees = nullptr;
	if (owner_->HasWeapon())
	{
		weaponTrees = owner_->GetWeapon()->GetStateTreeSet(name_);
	}

	// 所有者がNPCの場合、ビヘイビアツリーを設定する
	if (!owner_->IsPlayer())
	{
		NPC* npc = static_cast<NPC*>(owner_);
		BehaviorTree* currentTree = npc->GetBehaviorTree();

		// 適用すべきツリーを決定する
		BehaviorTree* targetTree = behaviorTree_.get();
		if (owner_->HasWeapon())
		{
			if (weaponTrees && weaponTrees->behaviorTree) targetTree = weaponTrees->behaviorTree.get();
			else targetTree = nullptr;
		}

		// 現在のツリーが空、かつ適用すべきツリーが存在する場合にセットする
		if (!currentTree && targetTree)
		{
			npc->SetBehaviorTree(targetTree);
		}
	}
	else if(owner_->IsPlayer())
	{
		// 所有者がプレイヤーの場合、コンボツリーを設定する
		Player* player = static_cast<Player*>(owner_);

		// 適用すべきツリーを決定する
		ComboTree* targetX = comboTreeX_.get();
		ComboTree* targetY = comboTreeY_.get();
		ComboTree* targetB = comboTreeB_.get();

		if (owner_->HasWeapon())
		{
			if (weaponTrees && weaponTrees->comboTreeX) targetX = weaponTrees->comboTreeX.get();
			else targetX = nullptr;
			if (weaponTrees && weaponTrees->comboTreeY) targetY = weaponTrees->comboTreeY.get();
			else targetY = nullptr;
			if (weaponTrees && weaponTrees->comboTreeB) targetB = weaponTrees->comboTreeB.get();
			else targetB = nullptr;
		}

		// 現在のツリーが空、かつ適用すべきツリーが存在する場合にセットする
		if (!player->GetCurrentComboTreeX() && !player->GetNextComboTreeX() && targetX)
		{
			player->SetCurrentComboTreeX(targetX);
		}
		if (!player->GetCurrentComboTreeY() && !player->GetNextComboTreeY() && targetY)
		{
			player->SetCurrentComboTreeY(targetY);
		}
		if (!player->GetCurrentComboTreeB() && !player->GetNextComboTreeB() && targetB)
		{
			player->SetCurrentComboTreeB(targetB);
		}
	}
}