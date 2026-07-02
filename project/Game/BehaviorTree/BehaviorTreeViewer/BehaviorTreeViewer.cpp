#include "BehaviorTreeViewer.h"
#include "Entity/Character/Character.h" // Character::characters_ を利用するため
#include "Entity/Character/NPC/NPC.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Node/Node.h"
#include <imgui.h>
#include <imnodes.h>
#include <vector>
#include <string>

void BehaviorTreeViewer::DrawUI()
{
#ifdef _DEVELOPMENT

	ImGui::Begin("Behavior Tree Runtime Viewer");

	// シーン内の全アクティブキャラクターからNPC（プレイヤー以外）を抽出
	std::vector<NPC*> activeNpcs;
	std::vector<std::string> npcNames;
	auto& characters = Character::GetCharacters(); // Characterクラスの静的メンバから全キャラクターを取得

	for (Character* character : characters)
	{
		if (character && character->GetCharacterTag() != CharacterTag::Player)
		{
			// CharacterからNPCへ安全にキャスト
			if (NPC* npc = dynamic_cast<NPC*>(character))
			{
				activeNpcs.push_back(npc);
				// コンボボックス用の表示名を作成（Tagやアドレス等で識別）
				npcNames.push_back("NPC_" + std::to_string(activeNpcs.size()) + " (Tag: " + std::to_string((int)npc->GetCharacterTag()) + ")");
			}
		}
	}

	if (activeNpcs.empty())
	{
		ImGui::Text("No active NPCs found in the scene.");
		ImGui::End();
		return;
	}

	// デバッグ監視したいNPCをコンボボックスで選択
	if (selectedNpcIndex_ >= activeNpcs.size()) selectedNpcIndex_ = 0;

	if (ImGui::BeginCombo("Target NPC", npcNames[selectedNpcIndex_].c_str()))
	{
		for (int i = 0; i < npcNames.size(); ++i)
		{
			const bool isSelected = (selectedNpcIndex_ == i);
			if (ImGui::Selectable(npcNames[i].c_str(), isSelected))
			{
				selectedNpcIndex_ = i;
			}
			if (isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	ImGui::Separator();

	// 選択されたNPCのビヘイビアツリーをキャンバスに描画
	NPC* targetNpc = activeNpcs[selectedNpcIndex_];
	if (targetNpc)
	{
		BehaviorTree* bt = targetNpc->GetBehaviorTree();
		if (bt && bt->GetRoot())
		{
			// ビビューアー専用のImNodesキャンバスを開始
			ImNodes::BeginNodeEditor();

			// ルートノードから再帰的に子ノード・リンクを全自動描画
			bt->GetRoot()->DrawDebuggerRecursive();

			ImNodes::EndNodeEditor();
		}
		else
		{
			ImGui::Text("This NPC does not have an active Behavior Tree.");
		}
	}

	ImGui::End();

#endif
}