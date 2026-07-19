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


	// 拡縮用のUIボタンと現在の倍率表示
	ImGui::Text("ズーム: %d%%", static_cast<int>(zoom_ * 100.0f));
	ImGui::SameLine();
	if (ImGui::Button(" ＋ ")) { zoom_ += 0.1f; }
	ImGui::SameLine();
	if (ImGui::Button(" － ")) { zoom_ -= 0.1f; }
	ImGui::SameLine();
	if (ImGui::Button("等倍 (100%)")) { zoom_ = 1.0f; }

	// マウスホイール単体での拡縮操作
	if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows))
	{
		float wheel = ImGui::GetIO().MouseWheel;
		if (wheel != 0.0f)
		{
			zoom_ += wheel * 0.05f; // スクロール感度の調整
		}
	}

	// ズーム率の限界値を制限（例: 40% 〜 150% 程度が文字が潰れず綺麗です）
	if (zoom_ < 0.4f) zoom_ = 0.4f;
	if (zoom_ > 1.5f) zoom_ = 1.5f;


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

			// ビューアー専用のズーム倍率を設定
			ImGui::SetWindowFontScale(zoom_);

			// ルートノードから再帰的に子ノード・リンクを全自動描画
			bt->GetRoot()->DrawDebuggerRecursive(zoom_);

			ImNodes::EndNodeEditor();

			// 最後にフォントスケールを等倍に戻す（他のウィンドウに影響させないため）
			ImGui::SetWindowFontScale(1.0f);
		}
		else
		{
			ImGui::Text("This NPC does not have an active Behavior Tree.");
		}
	}

	ImGui::End();

#endif
}