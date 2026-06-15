#include "StageEditorUI.h"
#include <numbers>
#include "../StageFileManager/StageFileManager.h"
#include "../StageSpawner/StageSpawner.h"
#include "../StageEditorHistory/StageEditorHistory.h"

#include "Scene/GameScene/GameScene.h"

#include "Entity/Character/Character.h"
#include "Entity/Weapon/Weapon.h"
#include "StageObject/StageObject.h"
#include "NavMesh/NavMesh.h"

/// @brief 初期化
void StageEditorUI::Initialize()
{
	// モーションマネージャのインスタンスを取得
	motionManager_ = MotionManager::GetInstance();

	// UIクラスの初期化
	placementUI_ = std::make_unique<StageEditorUIPlacement>(spawner_, history_);
	navMeshUI_ = std::make_unique<StageEditorUINavMesh>();
	navMeshInfoUI_ = std::make_unique<StageEditorUINavMeshInfo>();
	objectListUI_ = std::make_unique<StageEditorUIObjectList>(spawner_, history_, scene_->GetBehaviorTreeEditor());
	guizmo_ = std::make_unique<StageEditorGuizmo>();

	// 入力キーの初期化
	inputModelChange_ = std::make_unique<InputKey>("StageEditor_ModelChange", InputState::Trigger, DIK_TAB);

	// ビヘイビアツリーの名前リストをロード
	LoadBehaviorTreeNames();
}

/// @brief 更新処理
void StageEditorUI::Update()
{
	// ブジェクト配置モード と ナビメッシュ編集モード を切り替え
	if (inputModelChange_->IsInput())
	{
		if (currentMode_ == EditorMode::ObjectPlacement)
		{
			currentMode_ = EditorMode::NavMeshEdit;
		}
		else
		{
			currentMode_ = EditorMode::ObjectPlacement;
		}
	}
}

/// @brief UIの描画
/// @param placementList 
/// @param currentFileName 
/// @param isPlaying 
/// @param navMesh 
/// @param canExtrude 
/// @param canBridge 
void StageEditorUI::DrawUI(std::vector<PlacementData>& placementList, std::string& currentFileName, bool& isPlaying,
	NavMesh* navMesh, StageEditorNavMeshController* navMeshController, bool& isDirty, bool canExtrude, bool canBridge)
{
#ifdef _DEVELOPMENT

	if (!ImGui::Begin("Stage Editor")) 
	{
		// ウィンドウが折りたたまれている場合は終了
		ImGui::End();
		return;
	}

	// ファイルが選択されていない場合は、警告を表示してUIの描画を終了する
	if (currentFileName.empty())
	{
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "ステージファイルが選択されていません。");
		ImGui::Text("「ステージプロジェクトアセット」ウィンドウからステージを選択または作成してください。");
		ImGui::End();
		return;
	}

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, 8.0f));

	/*--------------
		操作方法
	--------------*/

	// ショートカットキーの説明
	ImGui::Text("ショートカットキー 一覧");

	// --- 保存の表示 ---
	ImGui::Text("Ctrl + S : 上書き保存");

	// --- Undo の表示切り替え ---
	if (history_->CanUndo())
	{
		ImGui::Text("Ctrl + Z : Undo");
	}
	else
	{
		ImGui::TextColored(ImVec4(0.2f, 0.2f, 0.2f, 1.0f), "Ctrl + Z : Undo (無効)");
	}

	// --- Redo の表示切り替え ---
	if (history_->CanRedo())
	{
		ImGui::Text("Ctrl + Y : Redo");
	}
	else
	{
		ImGui::TextColored(ImVec4(0.2f, 0.2f, 0.2f, 1.0f), "Ctrl + Y : Redo (無効)");
	}

	// --- コピーの表示切り替え ---
	if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(placementList.size()))
	{
		ImGui::Text("Ctrl + C : コピー");
	}
	else
	{
		ImGui::TextColored(ImVec4(0.2f, 0.2f, 0.2f, 1.0f), "Ctrl + C : コピー (リスト内で選択時のみ有効)");
	}

	// --- 貼り付けの表示切り替え ---
	if (hasCopiedData_)
	{
		ImGui::Text("Ctrl + V : 貼り付け");

	}
	else
	{
		ImGui::TextColored(ImVec4(0.2f, 0.2f, 0.2f, 1.0f), "Ctrl + V : 貼り付け (無効)");
	}

	// --- 複製の表示切り替え ---
	if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(placementList.size()))
	{
		ImGui::Text("Ctrl + D : 複製");
	}
	else
	{
		ImGui::TextColored(ImVec4(0.2f, 0.2f, 0.2f, 1.0f), "Ctrl + D : 複製 (リスト内で選択時のみ有効)");
	}

	// --- 削除の表示切り替え ---
	if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(placementList.size()))
	{
		ImGui::Text("Delete / Backspace キー : 削除");
	}
	else
	{
		ImGui::TextColored(ImVec4(0.2f, 0.2f, 0.2f, 1.0f), "Delete / Backspace キー : 削除 (リスト内で選択時のみ有効)");
	}

	// --- モード切り替えの表示 ---
	ImGui::Text("Tab キー : モード切替 (現在のモード : %s)", currentMode_ == EditorMode::ObjectPlacement ? "オブジェクト配置" : "ナビメッシュ編集");

	ImGui::Separator();

	if (currentMode_ == EditorMode::ObjectPlacement)
	{

	}
	else if (currentMode_ == EditorMode::NavMeshEdit)
	{
		// 操作方法の説明
		ImGui::Text("1 キー : 頂点 選択");
		ImGui::Text("2 キー : 辺 選択");
		ImGui::Text("3 キー : 面 選択");

		// --- 押し出し (Extrude) の表示切り替え ---
		if (canExtrude)
		{
			// 成功時: 緑色でショートカットを表示
			ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "E キー : 押し出し");
		}
		else
		{
			// 失敗時: 灰色で無効表示にし、右側にオレンジ色でアドバイスを表示
			ImGui::TextColored(ImVec4(0.2f, 0.2f, 0.2f, 1.0f), "E キー : 押し出し (無効)");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.0f, 1.0f), "<- 辺を1つ選択してください ( 2キー で 辺選択 )");
		}

		// --- ブリッジ (Bridge) の表示切り替え ---
		if (canBridge)
		{
			// 成功時: 緑色でショートカットを表示
			ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "B キー : ブリッジ");
		}
		else
		{
			// 失敗時: 灰色で無効表示にし、右側にオレンジ色でアドバイスを表示
			ImGui::TextColored(ImVec4(0.2f, 0.2f, 0.2f, 1.0f), "B キー : ブリッジ (無効)");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.0f, 1.0f), "<- 辺を2つ選択してください ( 2キー で 辺選択 )");
		}
		ImGui::Separator();
	}



	/*---------
		生成
	---------*/

	// 現在編集中のファイル名を表示
	ImGui::Text("ステージ : %s", currentFileName.c_str());

	// 実行 / 停止 ボタン
	if (isPlaying)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f)); // STOPボタンは赤色
		if (ImGui::Button("停止", ImVec2(100, 30)))
		{
			// プレイモードを終了したら、まずはシーン上のすべての実体を削除する
			for (auto& data : placementList)spawner_->DeleteActualEntity(data);
			placementList.clear();

			// プレイモードを終了したら、ゲームシーンをリセットして初期状態に戻す
			scene_->Reset();

			// プレイモードを終了したら、ファイルからステージデータを再読み込みして初期状態に戻す
			assert(fileManager_->LoadFromFile(currentFileName, placementList, spawner_, navMesh) && "実行停止時のファイル読み込みに失敗しました");

			isPlaying = false;
			Entity::SetUpdateEnabled(false);// すべての実体の更新を停止する
			StageObject::SetUpdateEnabled(false); // すべてのステージオブジェクトの更新を停止する
		}
		ImGui::PopStyleColor();
	}
	else
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f)); // PLAYボタンは緑色
		if (ImGui::Button("実行", ImVec2(100, 30)))
		{
			isPlaying = true;
			Entity::SetUpdateEnabled(true); // すべての実体の更新を再開する
			StageObject::SetUpdateEnabled(true); // すべてのステージオブジェクトの更新を再開する
		}
		ImGui::PopStyleColor();
	}

	ImGui::Separator();

	// ショートカットキーの処理
	HandleShortcuts(placementList, currentFileName, isPlaying, navMesh, isDirty);

	// モード切替の表示
	if (currentMode_ == EditorMode::ObjectPlacement)
	{
		guizmo_->UpdateAndDraw(placementList, selectedIndex_, isDirty, history_);
		objectListUI_->DrawWindow(placementList, selectedIndex_, isDirty, hasCopiedData_, copiedData_, navMesh, behaviorTreeNames_);
		placementUI_->DrawUI(placementList, selectedIndex_, isDirty, behaviorTreeNames_);
	}
	else if (currentMode_ == EditorMode::NavMeshEdit)
	{
		navMeshUI_->DrawUI(navMesh, canExtrude, canBridge, isDirty);
		navMeshInfoUI_->DrawWindow(navMesh, navMeshController);
	}

	ImGui::PopStyleVar();


	ImGui::End();

#endif
}

/// @brief アセットウィンドウの描画
/// @param placementList 
/// @param currentFileName 
/// @param isPlaying 
/// @param isDirty 
/// @param navMesh 
void StageEditorUI::DrawAssetWindow(std::vector<PlacementData>& placementList, std::string& currentFileName, bool& isPlaying, NavMesh* navMesh, bool& isDirty)
{
#ifdef _DEVELOPMENT

	if (!ImGui::Begin("Stage Project Assets")) 
	{
		ImGui::End();
		return;
	}

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, 8.0f));

	// 新規ステージの作成
	if (ImGui::Button("新規ステージ"))
	{
		ImGui::OpenPopup("New Stage Popup");
	}

	// 次に読み込むファイル名を保留するための変数
	bool requestSavePopup = false;

	// 新規ステージ作成のポップアップ
	if (ImGui::BeginPopup("New Stage Popup"))
	{
		// 新しいファイル名の入力
		static char newFileName[64] = "";
		ImGui::InputText("ファイル名", newFileName, 64);

		// 作成ボタン
		if (ImGui::Button("作成"))
		{
			// 入力されたファイル名に ".json" 拡張子を付加する
			std::string fileName = newFileName;
			if (fileName.find(".json") == std::string::npos) fileName += ".json";

			// 実行中に新しいステージを作成する場合は、先に実行を停止する
			if (isPlaying)
			{
				isPlaying = false;
				Entity::SetUpdateEnabled(false);
				StageObject::SetUpdateEnabled(false);
			}

			// 新しいステージを作成する前に、現在の配置リストに基づいてすべての実体を消去する
			if (isDirty)
			{
				pendingAction_ = PendingAction::New;
				pendingFileName_ = fileName;

				// 変更が保存されていないことを警告するポップアップを開く
				requestSavePopup = true;
			}
			else
			{
				// 変更が保存されていない場合でも、新しいステージを作成する前にすべての実体を消去する
				scene_->Reset();

				// すでにファイルが存在している場合は上書き保存する
				for (auto& data : placementList) spawner_->DeleteActualEntity(data);
				placementList.clear();
				currentFileName = fileName;
				fileManager_->SaveToFile(currentFileName, placementList, navMesh);
			}

			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	ImGui::Separator();
	ImGui::Text("保存ステージ:");
	ImGui::Spacing();

	// フォルダ内のjsonファイルを取得
	std::vector<std::string> files = fileManager_->GetSavedStageFiles();

	// グリッドレイアウトの計算
	float thumbnailSize = 64.0f;
	float padding = 16.0f;
	float cellSize = thumbnailSize + padding;
	float panelWidth = ImGui::GetContentRegionAvail().x;
	int columnCount = std::max(1, static_cast<int>(panelWidth / cellSize));

	// ポップアップを開くための状態管理変数
	static std::string fileToCopy = "";
	static std::string fileToDelete = "";

	// テーブルを開始
	if (ImGui::BeginTable("StageAssetGrid", columnCount))
	{
		for (const auto& file : files)
		{
			ImGui::TableNextColumn();
			ImGui::PushID(file.c_str());

			// 選択中のファイルは背景色を変える
			bool isSelected = (currentFileName == file);
			if (isSelected)
			{
				ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(ImVec4(0.3f, 0.5f, 0.8f, 0.5f)));
			}

			// ファイル名をボタンとして表示
			if (ImGui::Button(file.c_str(), ImVec2(thumbnailSize, thumbnailSize)))
			{
				// 実行中なら実行を停止する
				if (isPlaying)
				{
					isPlaying = false;
					Entity::SetUpdateEnabled(false);
					StageObject::SetUpdateEnabled(false);
				}

				// ファイルを切り替える前に、現在の配置リストに基づいてすべての実体を消去する
				if (isDirty)
				{
					pendingAction_ = PendingAction::Load;
					pendingFileName_ = file;
					
					// 変更が保存されていないことを警告するポップアップを開く
					requestSavePopup = true;
				}
				else
				{
					// 変更が保存されていない場合でも、ファイルを切り替える前にすべての実体を消去する
					scene_->Reset();

					// すでにファイルが存在している場合は上書き保存する
					for (auto& data : placementList) spawner_->DeleteActualEntity(data);
					placementList.clear();
					currentFileName = file;
					fileManager_->LoadFromFile(currentFileName, placementList, spawner_, navMesh);
				}
			}

			// 右クリックメニュー
			if (ImGui::BeginPopupContextItem("FileContextMenu"))
			{
				// 読み込み
				if (ImGui::MenuItem("読み込み"))
				{
					// 実行中なら実行を停止する
					if (isPlaying)
					{
						isPlaying = false;
						Entity::SetUpdateEnabled(false);
						StageObject::SetUpdateEnabled(false);
					}

					// ファイルを切り替える前に、現在の配置リストに基づいてすべての実体を消去する
					if (isDirty)
					{
						pendingAction_ = PendingAction::Load;
						pendingFileName_ = file;
						
						// 変更が保存されていないことを警告するポップアップを開く
						requestSavePopup = true;
					}
					else
					{
						// 変更が保存されていない場合でも、新しいステージを作成する前にすべての実体を消去する
						scene_->Reset();

						// すでにファイルが存在している場合は上書き保存する
						for (auto& data : placementList) spawner_->DeleteActualEntity(data);
						placementList.clear();
						currentFileName = file;
						fileManager_->LoadFromFile(currentFileName, placementList, spawner_, navMesh);
					}
				}

				// 上書き保存 (プレイ中は保存できないようにする)
				if (ImGui::MenuItem("上書き保存", nullptr, false, !isPlaying))
				{
					// 右クリックした対象を現在のファイルとして設定し、上書き保存する
					currentFileName = file;
					fileManager_->SaveToFile(currentFileName, placementList, navMesh);
					isDirty = false;
				}

				// コピー
				if (ImGui::MenuItem("コピー", nullptr, false, !isPlaying))
				{
					fileToCopy = file;
				}

				ImGui::Separator();

				// 削除は赤文字で表示
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
				if (ImGui::MenuItem("削除", nullptr, false, !isPlaying))
				{
					fileToDelete = file;
				}
				ImGui::PopStyleColor();


				ImGui::EndPopup();
			}

			ImGui::TextWrapped("%s", file.c_str());
			ImGui::PopID();
		}
		ImGui::EndTable();
	}


	// 保留中のアクションがある場合は、確認のポップアップを開く
	if (requestSavePopup)
	{
		ImGui::OpenPopup("Save Confirmation Popup");
	}

	// 保留中のアクションがある場合は、確認のポップアップを表示する
	if (ImGui::BeginPopupModal("Save Confirmation Popup", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		// 重要な操作なので、赤いテキストで警告を表示して強調する
		ImGui::Text("変更が保存されていません。\n現在のステージを保存しますか？");
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		// 保存ボタンを押して別のスクリプトを触る
		if (ImGui::Button("保存", ImVec2(140, 0)))
		{
			if (!currentFileName.empty())
			{
				fileManager_->SaveToFile(currentFileName, placementList, navMesh);
			}
			isDirty = false;
			ExecutePendingAction(placementList, currentFileName, navMesh);
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		// キャンセル(破棄)ボタンを押して別のスクリプトを触る
		if (ImGui::Button("キャンセル", ImVec2(140, 0)))
		{
			isDirty = false; // 変更を破棄したとみなす
			ExecutePendingAction(placementList, currentFileName, navMesh);
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		// 戻るボタンで今のスクリプトを触り続ける
		if (ImGui::Button("戻る", ImVec2(120, 0)))
		{
			pendingAction_ = PendingAction::None;
			pendingFileName_ = "";
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}



	// コピー対象が設定されていればモーダルを開く
	if (!fileToCopy.empty() && !ImGui::IsPopupOpen("Copy Stage"))
	{
		ImGui::OpenPopup("Copy Stage");
	}

	// コピー実行用のモーダルウィンドウ
	if (ImGui::BeginPopupModal("Copy Stage", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("'%s' に コピー:", fileToCopy.c_str());
		static char copyFileName[64] = "";

		ImGui::InputText("新規ファイル名", copyFileName, 64);

		if (ImGui::Button("Copy", ImVec2(120, 0)))
		{
			std::string newFileNameStr = copyFileName;
			if (!newFileNameStr.empty())
			{
				if (newFileNameStr.find(".json") == std::string::npos) newFileNameStr += ".json";

				// std::filesystem を使ってファイルを直接コピー
				try 
				{
					// コピー元とコピー先のパスを作成
					std::filesystem::path srcPath = "./Assets/Parameter/StageData/" + fileToCopy;
					std::filesystem::path destPath = "./Assets/Parameter/StageData/" + newFileNameStr;
					std::filesystem::copy_file(srcPath, destPath, std::filesystem::copy_options::overwrite_existing);
				}
				catch (const std::exception& e) 
				{
					// エラー時の処理 (必要であれば)
					(void)e;
				}
			}

			fileToCopy = ""; // 対象をクリア
			copyFileName[0] = '\0'; // 入力欄をクリア
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		// キャンセル ボタン
		if (ImGui::Button("キャンセル", ImVec2(120, 0)))
		{
			fileToCopy = "";
			copyFileName[0] = '\0';
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}



	// 削除対象が設定されていればモーダルを開く
	if (!fileToDelete.empty())
	{
		ImGui::OpenPopup("Delete Confirmation");
	}

	// モーダルウィンドウの描画
	if (ImGui::BeginPopupModal("Delete Confirmation", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		// パスからファイル名だけを抽出して表示
		std::string targetName = std::filesystem::path(fileToDelete).filename().string();

		ImGui::Text("'%s' を削除してもよろしいですか？", fileToDelete.c_str());
		ImGui::Separator();

		// はい ボタン
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
		if (ImGui::Button("削除", ImVec2(120, 0)))
		{
			// 物理的にファイルを削除
			std::filesystem::remove("./Assets/Parameter/StageData/" + fileToDelete);

			// もし現在開いているファイルを削除した場合は、エディタの情報をリセットする
			if (currentFileName == targetName)
			{
				// 変更が保存されていない場合でも、新しいステージを作成する前にすべての実体を消去する
				scene_->Reset();

				// ファイルを切り替える前に、現在の配置リストに基づいてすべての実体を消去する
				for (auto& data : placementList)spawner_->DeleteActualEntity(data);
				placementList.clear();

				currentFileName = "";
				selectedIndex_ = -1;
			}

			fileToDelete = ""; // 対象をクリア
			ImGui::CloseCurrentPopup();
		}
		ImGui::PopStyleColor();


		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();

		// キャンセル ボタン
		if (ImGui::Button("キャンセル", ImVec2(120, 0)))
		{
			fileToDelete = ""; // キャンセル時は対象をクリアするだけ
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	ImGui::PopStyleVar();

	ImGui::End();
#endif
}


/// @brief 保留中のアクションを実行する
/// @param placementList 
/// @param currentFileName 
/// @param navMesh 
void StageEditorUI::ExecutePendingAction(std::vector<PlacementData>& placementList, std::string& currentFileName, NavMesh* navMesh)
{
	if (pendingAction_ == PendingAction::Load)
	{
		// ファイルを切り替える前に、現在の配置リストに基づいてすべての実体を消去する
		scene_->Reset();

		// ファイルを切り替える前に、現在の配置リストに基づいてすべての実体を消去する
		for (auto& data : placementList) spawner_->DeleteActualEntity(data);
		placementList.clear();
		currentFileName = pendingFileName_;
		fileManager_->LoadFromFile(currentFileName, placementList, spawner_, navMesh);
	}
	else if (pendingAction_ == PendingAction::New)
	{
		// ファイルを切り替える前に、現在の配置リストに基づいてすべての実体を消去する
		scene_->Reset();

		// 新しいファイルを作成する前に、現在の配置リストに基づいてすべての実体を消去する
		for (auto& data : placementList) spawner_->DeleteActualEntity(data);
		placementList.clear();
		currentFileName = pendingFileName_;
		fileManager_->SaveToFile(currentFileName, placementList, navMesh);
	}

	// 実行後はリセット
	pendingAction_ = PendingAction::None;
	pendingFileName_ = "";
}


/// @brief ショートカットキーの処理
/// @param placementList 
/// @param currentFileName 
/// @param isPlaying 
/// @param navMesh 
/// @param isDirty 
void StageEditorUI::HandleShortcuts(std::vector<PlacementData>& placementList, std::string& currentFileName, bool& isPlaying, NavMesh* navMesh, bool& isDirty)
{
	// プレイ中はショートカットキーを無効化する
	if (isPlaying)return;


	ImGuiIO& io = ImGui::GetIO();

	// Ctrl + S で上書き保存
	if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_S))
	{
		fileManager_->SaveToFile(currentFileName, placementList, navMesh);
		isDirty = false;
	}

	// Ctrl + Z でアンドゥ
	if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Z))
	{
		history_->Undo(placementList, spawner_);
		isDirty = true;
	}

	// Ctrl + Y でリドゥ
	if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Y))
	{
		history_->Redo(placementList, spawner_);
		isDirty = true;
	}

	if (!io.WantTextInput)
	{
		// Ctrl + C でコピー
		if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_C))
		{
			// 有効なオブジェクトが選択されている場合のみコピー
			if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(placementList.size()))
			{
				// 配置データ全体をコピーする
				copiedData_ = placementList[selectedIndex_];

				// 別ファイルへのペーストに対応するため、実体へのポインタはリセットする
				copiedData_.instancePtr = nullptr;
				hasCopiedData_ = true;
			}
		}

		// Ctrl + V でペースト
		if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_V))
		{
			if (hasCopiedData_)
			{
				// 現在の状態を履歴に保存して変更フラグを立てる
				history_->SaveHistory(placementList);
				isDirty = true;

				// クリップボードからデータを複製
				PlacementData newData = copiedData_;

				// 実体をシーンに生成してリストに追加
				spawner_->SpawnActualEntity(newData);
				placementList.push_back(newData);

				// ペーストしたオブジェクトを自動的に選択状態にする
				selectedIndex_ = static_cast<int>(placementList.size()) - 1;
			}
		}

		// Ctrl + D で複製
		if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_D))
		{
			// 有効なオブジェクトが選択されている場合のみ複製
			if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(placementList.size()))
			{
				// 現在の状態を履歴に保存して変更フラグを立てる
				history_->SaveHistory(placementList);
				isDirty = true;

				// 選択中のオブジェクトのデータを複製
				PlacementData newData = placementList[selectedIndex_];

				// 複製したオブジェクトは少し位置をずらして生成する
				newData.position.x += 0.5f;
				newData.position.z += 0.5f;
				newData.instancePtr = nullptr;

				// 実体をシーンに生成してリストに追加
				spawner_->SpawnActualEntity(newData);
				placementList.push_back(newData);

				// 複製したオブジェクトを自動的に選択状態にする
				selectedIndex_ = static_cast<int>(placementList.size()) - 1;
			}
		}

		// Delete / Backspace キーで削除
		if (ImGui::IsKeyPressed(ImGuiKey_Delete) || ImGui::IsKeyPressed(ImGuiKey_Backspace))
		{
			if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(placementList.size()))
			{
				// 現在の状態を履歴に保存して変更フラグを立てる
				history_->SaveHistory(placementList);
				isDirty = true;

				// シーンから実体を削除してリストからも削除
				spawner_->DeleteActualEntity(placementList[selectedIndex_]);
				placementList.erase(placementList.begin() + selectedIndex_);

				// 選択状態をリセット
				selectedIndex_ = -1;
			}
		}
	}
}

/// @brief ビヘイビアツリーデータの名前を読み込む
void StageEditorUI::LoadBehaviorTreeNames()
{
	behaviorTreeNames_.clear();

	// ビヘイビアツリーのデータが保存されているフォルダのパス
	std::string directoryPath = "./Assets/Parameter/BehaviorTree/";

	// フォルダが存在しない場合は処理を終了
	if (!std::filesystem::exists(directoryPath))
		return;

	// フォルダ内のファイルを走査
	for (const auto& entry : std::filesystem::directory_iterator(directoryPath))
	{
		if (entry.is_regular_file())
		{
			// stem() を使うと、拡張子を除いたファイル名を取得できます
			behaviorTreeNames_.push_back(entry.path().stem().string());
		}
	}
}