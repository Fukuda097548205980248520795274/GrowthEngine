#include "BehaviorTreeViewer.h"
#include "Entity/Character/Character.h"
#include "Entity/Character/NPC/NPC.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Node/Node.h"
#include <imgui.h>
#include <imnodes.h>
#include <vector>
#include <string>

void BehaviorTreeViewer::DrawUI()
{
#ifdef DEVELOPMENT


	// NPCのリストを取得
	std::vector<NPC*> activeNpcs;
	std::vector<std::string> npcNames;
	auto& characters = Character::GetCharacters();

	for (Character* character : characters)
	{
		if (character && character->GetCharacterTag() != CharacterTag::Player)
		{
			// CharacterからNPCへ安全にキャスト
			if (NPC* npc = dynamic_cast<NPC*>(character))
			{
				activeNpcs.push_back(npc);

				// エディタ用に設定された名前を取得
				std::string name = npc->GetEditorName();
				if (name.empty())
				{
					name = "Unnamed_NPC_" + std::to_string(activeNpcs.size());
				}

				// リスト用の表示名を作成
				npcNames.push_back(name + " (Tag: " + std::to_string((int)npc->GetCharacterTag()) + ")");
			}
		}
	}

	
	ImGui::Begin("NPC選択");
	if (activeNpcs.empty())
	{
		ImGui::Text("有効なNPCが見つかりません。");
	}
	else
	{
		// 選択されているNPCがまだ生存しているかどうかを確認
		bool isTargetAlive = false;
		for (NPC* npc : activeNpcs)
		{
			if (npc == selectedNpc_)
			{
				isTargetAlive = true;
				break;
			}
		}

		// 選択されているNPCが死亡している場合、最初のNPCを選択する
		if (!isTargetAlive)
			selectedNpc_ = activeNpcs[0];

		ImGui::Text("選択中のNPC :");

		// 選択中のNPCの名前を表示
		if (ImGui::BeginListBox("##NPCリスト", ImVec2(-FLT_MIN, 150.0f))) 
		{
			for (int i = 0; i < npcNames.size(); ++i)
			{
				// ポインタが一致しているかどうかで選択状態を判定
				const bool isSelected = (selectedNpc_ == activeNpcs[i]);

				// 選択されたNPCを更新
				if (ImGui::Selectable(npcNames[i].c_str(), isSelected))
					selectedNpc_ = activeNpcs[i];

				// 選択されたNPCにフォーカスを当てる
				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndListBox();
		}
	}
	ImGui::End();



	ImGui::Begin("ランタイムビヘイビアツリービュー");
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

	// ズーム率の限界値を制限
	if (zoom_ < 0.4f) zoom_ = 0.4f;
	if (zoom_ > 1.5f) zoom_ = 1.5f;

	ImGui::Separator();

	// 選択されたNPCのビヘイビアツリーをキャンバスに描画
	if (!activeNpcs.empty())
	{
		// NPCのビヘイビアツリーを取得
		BehaviorTree* bt = selectedNpc_->GetBehaviorTree();
		if (bt && bt->GetRoot())
		{
			// ビヘイビアツリーのノードにランタイムIDを割り当てる
			int idCounter = 1;
			bt->GetRoot()->AssignRuntimeIDs(idCounter);

			// レイアウト計算
			float currentY = 0.0f;
			float offsetX = 300.0f;
			float offsetY = 120.0f;
			bt->GetRoot()->CalculateLayout(0, currentY, offsetX, offsetY);

			// ビビューアー専用のImNodesキャンバスを開始
			ImNodes::BeginNodeEditor();

			// ビューアー専用のズーム倍率を設定
			ImGui::SetWindowFontScale(zoom_);

			// ルートノードから再帰的に子ノード・リンクを全自動描画
			bt->GetRoot()->DrawDebuggerRecursive(zoom_);

			ImNodes::EndNodeEditor();

			// 最後にフォントスケールを等倍に戻す
			ImGui::SetWindowFontScale(1.0f);
		}
		else
		{
			ImGui::Text("このNPCは有効なビヘイビアツリーを持っていません。");
		}
	}
	else
	{
		ImGui::Text("ターゲットを選択してください。");
	}
	ImGui::End();

#endif
}