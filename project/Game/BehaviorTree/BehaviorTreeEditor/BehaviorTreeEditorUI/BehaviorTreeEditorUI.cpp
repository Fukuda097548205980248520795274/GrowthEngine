#include "../BehaviorTreeEditor.h"

/// @brief ノードテーブルを描画する
void BehaviorTreeEditor::DrawNodeTable()
{
#ifdef DEVELOPMENT

	ImGui::Begin("Behavior Tree Editor");

	// ファイルが選択されていない場合はノードエディタを描画せずに終了
	if (currentFileName_.empty())
	{
		ImGui::End();
		return;
	}


	// ノード追加のためのボタン
	if (ImGui::Button("＋ ノードを追加..."))
	{
		ImGui::OpenPopup("AddNodePopup");
	}

	// ノード追加のポップアップメニュー
	if (ImGui::BeginPopup("AddNodePopup"))
	{
		ImGui::SeparatorText("子あり");
		if (ImGui::MenuItem("永続 セレクタ")) AddPersistentSelectorNode();
		if (ImGui::MenuItem("永続 シーケンス")) AddPersistentSequenceNode();
		if (ImGui::MenuItem("再起動 セレクタ")) AddRestartingSelectorNode();
		if (ImGui::MenuItem("再起動 シーケンス")) AddRestartingSequenceNode();
		if (ImGui::MenuItem("ユーティリティ セレクタ")) AddUtilitySelectorNode();

		ImGui::SeparatorText("子なし");
		if (ImGui::MenuItem("条件")) AddConditionNode();
		if (ImGui::MenuItem("アクション")) AddActionNode();

		ImGui::EndPopup();
	}


	ImGui::Separator();

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

	// ズーム率が変更された場合、全ノードに位置更新フラグを立てる
	if (zoom_ != prevZoom_)
	{
		for (auto& node : nodes_)
		{
			node.needSetPos = true;
		}
		prevZoom_ = zoom_;
	}


	// ノードエディタのキャンバスを描画
	DrawNodeEditorCanvas();


	// キャンバスのコンテキストメニュー
	if (ImGui::BeginPopup("CanvasContextMenu"))
	{
		ImGui::SeparatorText("ノード追加");

		// Compositesノード追加のためのサブメニュー
		if (ImGui::BeginMenu("子あり"))
		{
			if (ImGui::MenuItem("永続 セレクタ")) AddPersistentSelectorNode();
			if (ImGui::MenuItem("永続 シーケンス")) AddPersistentSequenceNode();
			if (ImGui::MenuItem("再起動 セレクタ")) AddRestartingSelectorNode();
			if (ImGui::MenuItem("再起動 シーケンス")) AddRestartingSequenceNode();
			if (ImGui::MenuItem("ユーティリティ セレクタ")) AddUtilitySelectorNode();
			ImGui::EndMenu();
		}

		// Tasksノード追加のためのサブメニュー
		if (ImGui::BeginMenu("子なし"))
		{
			if (ImGui::MenuItem("条件")) AddConditionNode();
			if (ImGui::MenuItem("アクション")) AddActionNode();
			ImGui::EndMenu();
		}

		ImGui::EndPopup();
	}


	// パラメータ調整中はショートカットキーの処理を無効化する
	if (!ImGui::GetIO().WantTextInput && !ImGui::IsAnyItemActive())
	{
		// コピーとペーストの処理
		clipboard_->HandleCopy(nodes_, links_);
		clipboard_->HandlePaste(*this);

		// DeleteキーまたはBackspaceキーが押された場合、選択されているノードを削除する
		if (ImGui::IsKeyPressed(ImGuiKey_Delete) || ImGui::IsKeyPressed(ImGuiKey_Backspace))
		{
			DeleteSelectedNodes();
		}

		// Ctrl+Sで現在のツリーを保存する
		if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_S))
		{
			SaveCurrentTree();
		}

		// Ctrl+ZでUndoを実行
		if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Z))
		{
			history_->Undo(*this);

			// 変更があったのでフラグを立てる
			isDirty_ = true;
		}

		// Ctrl+YでRedoを実行
		if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Y))
		{
			history_->Redo(*this);

			// 変更があったのでフラグを立てる
			isDirty_ = true;
		}

		// Ctrl + Shift + N で整頓
		if (ImGui::GetIO().KeyCtrl && ImGui::GetIO().KeyShift && ImGui::IsKeyPressed(ImGuiKey_N))
		{
			AutoArrangeNodes();

			// 変更があったのでフラグを立てる
			isDirty_ = true;
		}

		// Homeキー または Ctrl + F でルートノードにカメラを移動
		if (ImGui::IsKeyPressed(ImGuiKey_Home) || (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_F)))
		{
			int rootId = FindRootNodeId();
			if (rootId != -1)
			{
				ImNodes::EditorContextMoveToNode(rootId);
			}
		}
	}



	// リンクの作成と削除の処理
	int start_pin, end_pin;

	// リンクが作成された場合
	if (ImNodes::IsLinkCreated(&start_pin, &end_pin))
	{
		// ノード追加前の状態を履歴に保存する
		history_->SaveHistory(nodes_, links_, currentId_);

		// 新しいリンクを追加
		EditorLink newLink;
		newLink.id = GetNextId();
		newLink.startPinId = start_pin;
		newLink.endPinId = end_pin;
		newLink.startNodeId = GetNodeIdFromPinId(start_pin);
		newLink.endNodeId = GetNodeIdFromPinId(end_pin);
		links_.push_back(newLink);

		// 変更があったのでフラグを立てる
		isDirty_ = true;
	}


	// リンクが削除された場合
	int linkId;

	// ImNodes::IsLinkDestroyedは削除されたリンクのIDを返す関数
	if (ImNodes::IsLinkDestroyed(&linkId))
	{
		// ノード追加前の状態を履歴に保存する
		history_->SaveHistory(nodes_, links_, currentId_);

		auto it = std::find_if(links_.begin(), links_.end(),
			[linkId](const EditorLink& link) { return link.id == linkId; });
		if (it != links_.end()) {
			links_.erase(it);
		}

		// 変更があったのでフラグを立てる
		isDirty_ = true;
	}

	ImGui::End();

#endif
}

/// @brief プロパティウィンドウを描画する
void BehaviorTreeEditor::DrawPropertyWindow()
{
#ifdef DEVELOPMENT

	// プロパティウィンドウの開始
	ImGui::Begin("Node Properties");

	// 選択されているノードの数を取得
	int numSelected = ImNodes::NumSelectedNodes();

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, 8.0f));

	/*--------------
		操作方法
	--------------*/

	// ショートカットキーの説明
	ImGui::Text("ショートカットキー 一覧");

	// 保存の表示
	ImGui::Text("Ctrl + S : 上書き保存");

	// Undo の表示切り替え
	if (history_->CanUndo())
	{
		ImGui::Text("Ctrl + Z : Undo");
	}
	else
	{
		ImGui::TextColored(ImVec4(0.2f, 0.2f, 0.2f, 1.0f), "Ctrl + Z : Undo (無効)");
	}

	// Redo の表示切り替え
	if (history_->CanRedo())
	{
		ImGui::Text("Ctrl + Y : Redo");
	}
	else
	{
		ImGui::TextColored(ImVec4(0.2f, 0.2f, 0.2f, 1.0f), "Ctrl + Y : Redo (無効)");
	}

	// コピーとペーストの表示切り替え
	if (numSelected > 0)
	{
		ImGui::Text("Ctrl + C : コピー");
		ImGui::Text("Ctrl + V : ペースト");
	}
	else
	{
		ImGui::TextColored(ImVec4(0.2f, 0.2f, 0.2f, 1.0f), "Ctrl + C : コピー (ノード選択時のみ有効)");
		ImGui::TextColored(ImVec4(0.2f, 0.2f, 0.2f, 1.0f), "Ctrl + V : ペースト (コピーしたノードがある場合のみ有効)");
	}

	// リンク（接続線）の削除
	if (ImNodes::NumSelectedLinks() > 0)
	{
		ImGui::Text("Ctrl + 左クリック : リンク（接続線）削除");
	}
	else
	{
		ImGui::TextColored(ImVec4(0.2f, 0.2f, 0.2f, 1.0f), "Ctrl + 左クリック : リンク（接続線）削除 (リンク選択時のみ有効)");
	}

	// ノード削除の表示切り替え
	if (numSelected > 0)
	{
		ImGui::Text("Delete / Backspace : ノード削除");
	}
	else
	{
		ImGui::TextColored(ImVec4(0.2f, 0.2f, 0.2f, 1.0f), "Delete / Backspace : ノード削除 (ノード選択時のみ有効)");
	}

	// ノード自動整列の表示
	ImGui::Text("Ctrl + Shift + N : ノード自動整列");

	// ルートノードにカメラを移動
	ImGui::Text("Ctrl + F : ルートノードにカメラを移動");

	ImGui::Separator();



	/*------------------
		パラメータ調整
	------------------*/

	// 履歴と変更フラグをまとめて処理するラムダ関数
	auto HistorySaveIfChanged = [this]() {if (ImGui::IsItemActivated()) { history_->SaveHistory(nodes_, links_, currentId_); isDirty_ = true; }};

	if (numSelected == 1)
	{
		// 1つだけ選択されている場合、そのノードのIDを取得
		int selectedNodeId;
		ImNodes::GetSelectedNodes(&selectedNodeId);

		// IDからノードを検索
		auto it = std::find_if(nodes_.begin(), nodes_.end(),
			[selectedNodeId](const EditorNode& n) { return n.id == selectedNodeId; });

		if (it != nodes_.end())
		{
			EditorNode& node = *it;

			// ノードの種類などを表示
			ImGui::Text("ノード ID: %d", node.id);
			ImGui::Separator();

			// ノード名の編集UI
			HistorySaveIfChanged();
			ImGui::InputText("ノード名", node.name, sizeof(node.name));

			// ノードの種類に応じて、パラメータ設定UIをここで描画する
			if (node.type == EditorNodeType::Condition)
			{
				DrawConditionNodeSettings(node);
			}
			else if (node.type == EditorNodeType::Action)
			{
				DrawActionNodeSettings(node);
			}
			else if (node.type == EditorNodeType::UtilitySelector)
			{
				DrawUtilitySelectorNodeSettings(node);
			}
			else
			{
				// それ以外のノードタイプには特に設定項目がないので、その旨を表示
				ImGui::Text("このノードタイプには編集可能なプロパティがありません");
			}
		}
	}
	else if (numSelected > 1)
	{
		// 複数選択時のメッセージ
		ImGui::Text("複数のノードが選択されています");
		ImGui::Text("プロパティを編集するには、1つのノードのみを選択してください");
	}
	else
	{
		// 未選択時のメッセージ
		ImGui::Text("ノードが選択されていません");
	}

	ImGui::PopStyleVar();

	ImGui::End();

#endif
}

/// @brief　プロジェクトウィンドウを描画する
void BehaviorTreeEditor::DrawProjectWindow()
{
#ifdef DEVELOPMENT
	
	ImGui::Begin("Tree Project Assets");

	// 次に読み込むファイル名を保留するための変数
	bool requestSavePopup = false;

	// 新規ツリーボタン
	if (ImGui::Button("新規ツリー"))
	{
		ImGui::OpenPopup("New Tree Popup");
	}

	// 新規ツリーポップアップ
	if (ImGui::BeginPopup("New Tree Popup"))
	{
		// 新しいツリーのファイル名入力
		static char newFileName[64] = "";
		ImGui::InputText("ファイル名", newFileName, 64);

		// 新しいツリーを作成する前に、未保存の変更があるかどうかを確認する
		if (ImGui::Button("作成"))
		{
			if (isDirty_)
			{
				// 未保存の変更があれば保留
				pendingFileName_ = newFileName;
				isPendingNewTree_ = true;
				ImGui::OpenPopup("SaveConfirmationPopup");
				requestSavePopup = true;
			}
			else
			{
				// 変更がなければそのまま作成
				currentFileName_ = newFileName;
				ClearEditor();
				SaveCurrentTree();
				isDirty_ = false;
			}
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	ImGui::Separator();

	// 保存されているツリーの一覧を表示
	ImGui::Text("保存ツリー :");
	auto files = projectManager_.GetFileList();
	ImGui::Spacing();

	// グリッドレイアウトの計算
	float thumbnailSize = 64.0f;
	float padding = 16.0f;
	float cellSize = thumbnailSize + padding;
	float panelWidth = ImGui::GetContentRegionAvail().x;

	// ウィンドウ幅に収まる列数を計算（最低1列）
	int columnCount = std::max(1, static_cast<int>(panelWidth / cellSize));

	// ポップアップを開くための状態管理変数
	static std::string fileToCopy = "";
	static std::string fileToDelete = "";
	static std::string fileToRename = "";

	// ImGuiのテーブルを使用してグリッドレイアウトを作成
	if (ImGui::BeginTable("AssetGrid", columnCount))
	{
		for (const auto& file : files)
		{
			// 次の列に移動
			ImGui::TableNextColumn();
			ImGui::PushID(file.c_str());

			// アイコンのSRVハンドルをImGuiのテクスチャIDに変換
			ImTextureID iconTexture = (ImTextureID)btFileIcon_.ptr;

			// アイコンが選択されているかどうかのハイライト（背景色）
			bool isSelected = (currentFileName_ == file);
			if (isSelected)
			{
				ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(ImVec4(0.3f, 0.5f, 0.8f, 0.5f)));
			}

			// アイコンまたはボタンがクリックされたときの処理
			if (ImGui::ImageButton(file.c_str(), iconTexture, ImVec2(thumbnailSize, thumbnailSize)))
			{
				if (isDirty_ && currentFileName_ != file)
				{
					// 未保存の変更があれば保留してポップアップを開く
					pendingFileName_ = file;
					isPendingNewTree_ = false;
					requestSavePopup = true;
				}
				else
				{
					LoadTree(file);
				}
			}

			// アイコンまたはボタンを右クリックしたときのコンテキストメニュー
			if (ImGui::BeginPopupContextItem("FileContextMenu"))
			{
				// 現在のファイルが選択されているかどうかを判定する変数
				bool isCurrentFile = (currentFileName_ == file);

				// Saveは現在のファイルが選択されているときのみ表示
				if (ImGui::MenuItem("保存", "Ctrl+S", false, isCurrentFile))
				{
					SaveCurrentTree();
				}

				// Loadは常に表示
				if (ImGui::MenuItem("読み込み"))
				{
					LoadTree(file);
				}

				ImGui::Separator();

				// Renameは常に表示
				if (ImGui::MenuItem("名前を変更"))
				{
					fileToRename = file;
				}

				// Copyは常に表示
				if (ImGui::MenuItem("コピー"))
				{
					fileToCopy = file;
				}

				// Deleteは危険な操作なので、赤いテキストで表示して強調する
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
				if (ImGui::MenuItem("削除"))
				{
					fileToDelete = file;
				}

				ImGui::PopStyleColor();

				ImGui::EndPopup();
			}

			// ファイル名をアイコンの下に表示
			ImGui::TextWrapped("%s", file.c_str());

			ImGui::PopID();
		}

		ImGui::EndTable();
	}


	// 名前の変更用のポップアップ
	if (!fileToRename.empty())
	{
		ImGui::OpenPopup("RenamePopup");
	}

	if (ImGui::BeginPopup("RenamePopup"))
	{
		static char newFileName[64] = "";
		ImGui::Text("'%s' の新しい名前:", fileToRename.c_str());
		ImGui::InputText("新規ファイル名", newFileName, 64);

		if (ImGui::Button("変更実行"))
		{
			if (strlen(newFileName) > 0)
			{
				// BehaviorTreeProjectManager に RenameProjectFile メソッドがある場合の想定
				projectManager_.RenameProjectFile(fileToRename, newFileName);

				// 名前を変更したファイルが現在開いているファイルだった場合、内部のファイル名も更新する
				if (currentFileName_ == fileToRename)
				{
					currentFileName_ = newFileName;
				}

				fileToRename = "";
				memset(newFileName, 0, sizeof(newFileName)); // 入力バッファのクリア
				ImGui::CloseCurrentPopup();
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("キャンセル"))
		{
			fileToRename = "";
			memset(newFileName, 0, sizeof(newFileName)); // 入力バッファのクリア
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}


	// 未保存の変更がある場合、保存確認のポップアップを開く
	if (requestSavePopup)
	{
		ImGui::OpenPopup("SaveConfirmationPopup");
	}

	// 保存確認のポップアップ
	if (ImGui::BeginPopupModal("SaveConfirmationPopup", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("未保存の変更があります。\n現在のツリーを保存しますか？");
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		// [セーブボタン] 現在の状態を保存してから、保留していた操作を実行する
		if (ImGui::Button("保存", ImVec2(120, 0)))
		{
			// 保存
			SaveCurrentTree();

			if (isPendingNewTree_)
			{
				// 保留していたのが「新規作成」だった場合
				currentFileName_ = pendingFileName_;
				ClearEditor();
				SaveCurrentTree();
			}
			else
			{
				// 保留していたのが「別のファイルの読み込み」だった場合
				LoadTree(pendingFileName_);
			}
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		// [キャンセルボタン] 保存せずに保留していた操作を実行する
		if (ImGui::Button("キャンセル", ImVec2(100, 0)))
		{
			if (isPendingNewTree_)
			{
				// 保存せずに新規作成へ進む
				currentFileName_ = pendingFileName_;
				ClearEditor();
				SaveCurrentTree();
			}
			else
			{
				// 保存せずにクリックしたファイルを読み込む
				LoadTree(pendingFileName_);
			}

			// フラグ類をリセットしてポップアップを閉じる
			pendingFileName_ = "";
			isPendingNewTree_ = false;
			isDirty_ = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		// [戻るボタン] 何もせずに保留していた操作をキャンセルしてポップアップを閉じる
		if (ImGui::Button("Back", ImVec2(100, 0)))
		{
			// 何もせず、保留データだけクリアしてポップアップを閉じる
			pendingFileName_ = "";
			isPendingNewTree_ = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}


	// コピー用のポップアップ
	if (!fileToCopy.empty())
	{
		ImGui::OpenPopup("CopyPopup");
	}

	// コピー用のポップアップの内容
	if (ImGui::BeginPopup("CopyPopup"))
	{
		// コピー先のファイル名を入力するためのテキストボックス
		static char newFileName[64] = "";
		ImGui::Text("コピー '%s' から:", fileToCopy.c_str());
		ImGui::InputText("新規ファイル名", newFileName, 64);

		// コピー実行ボタンとキャンセルボタン
		if (ImGui::Button("コピー実行"))
		{
			if (strlen(newFileName) > 0)
			{
				projectManager_.CopyProjectFile(fileToCopy, newFileName);
				fileToCopy = "";
				memset(newFileName, 0, sizeof(newFileName));
				ImGui::CloseCurrentPopup();
			}
		}
		ImGui::SameLine();

		// コピー操作は危険な操作ではないので、通常のボタンでキャンセルを表示
		if (ImGui::Button("キャンセル"))
		{
			fileToCopy = "";
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	// 削除用のポップアップ
	if (!fileToDelete.empty())
	{
		ImGui::OpenPopup("DeletePopup");
	}

	// 削除用のポップアップの内容
	if (ImGui::BeginPopupModal("DeletePopup", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		// 重要な操作なので、赤いテキストで警告を表示して強調する
		ImGui::Text("'%s' を削除してもよろしいですか？", fileToDelete.c_str());
		ImGui::Separator();

		// 削除実行ボタンは赤いテキストで表示して強調する
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
		if (ImGui::Button("削除", ImVec2(120, 0)))
		{
			projectManager_.DeleteProjectFile(fileToDelete);
			if (currentFileName_ == fileToDelete)
			{
				ClearEditor();
				currentFileName_ = "";
			}
			fileToDelete = "";
			ImGui::CloseCurrentPopup();
		}
		ImGui::PopStyleColor();

		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();

		// 削除操作は危険な操作なので、赤いテキストで表示して強調する
		if (ImGui::Button("キャンセル", ImVec2(120, 0)))
		{
			fileToDelete = "";
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	ImGui::End();

#endif
}

/// @brief ノードエディタのキャンバスを描画する
void BehaviorTreeEditor::DrawNodeEditorCanvas()
{
	// ノードIDをキー、子ノードIDのリストを値とする隣接リストを作成
	std::unordered_map<int, std::vector<int>> adjList;

	// すべてのリンクを処理して隣接リストを構築
	for (const auto& link : links_)
	{
		// リンクの開始ピンと終了ピンが出力ピンか入力ピンかを判定
		int startNode = link.startNodeId;
		int endNode = link.endNodeId;

		// どちらも有効なノードIDが取得できた場合のみ隣接リストに追加
		if (startNode != -1 && endNode != -1)
		{
			adjList[startNode].push_back(endNode);
		}
	}

	// ノードの展開状態を管理するためのセット
	std::unordered_set<int> hiddenNodes;

	// ノードを非表示にするためのDFS関数
	std::function<void(int)> dfsHide = [&](int nodeID)
		{
			// 子ノードがいない場合は終了
			if (adjList.find(nodeID) == adjList.end()) return;

			// 子ノードをすべて非表示にする
			for (int childID : adjList[nodeID])
			{
				if (hiddenNodes.find(childID) == hiddenNodes.end())
				{
					hiddenNodes.insert(childID);
					dfsHide(childID);
				}
			}
		};

	// 折りたたまれているノードの子ノードをすべて非表示にする
	for (const auto& node : nodes_)
	{
		if (node.isCollapsed)
			dfsHide(node.id);
	}

	// ノードの位置を更新する必要があるかどうかを示すフラグをリセットする
	for (auto& node : nodes_)
	{
		if (hiddenNodes.count(node.id) > 0)
			node.needSetPos = true;
	}

	// ImGuiのスタイルを取得して、ズーム率に応じてフレームパディングとアイテム間隔を調整する
	ImGuiStyle& style = ImGui::GetStyle();
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x * zoom_, style.FramePadding.y * zoom_));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(style.ItemSpacing.x * zoom_, style.ItemSpacing.y * zoom_));

	// ノードエディタの開始
	ImNodes::BeginNodeEditor();

	// ズーム率をImGuiに設定する
	ImGui::SetWindowFontScale(zoom_);

	// 特定のノードをエディタウィンドウの中心に配置する処理を行う
	if (pendingCenterNodeId_ != -1)
	{
		// エディタウィンドウの現在の中心（ウィンドウ相対）を計算
		ImVec2 viewCenter = ImVec2(ImGui::GetWindowSize().x * 0.5f, ImGui::GetWindowSize().y * 0.5f);

		// ImNodes の現在のスクロール（パン）量を取得
		ImVec2 panning = ImNodes::EditorContextGetPanning();

		// スクロール状態を考慮した「グリッド空間上の中心座標」を算出
		ImVec2 gridCenter = ImVec2(viewCenter.x - panning.x, viewCenter.y - panning.y);

		// 保留されているノードIDに対応するノードの位置を、グリッド空間上の中心座標に設定する
		for (auto& node : nodes_)
		{
			if (node.id == pendingCenterNodeId_)
			{
				node.pos.x = gridCenter.x / zoom_;
				node.pos.y = gridCenter.y / zoom_;
				node.needSetPos = true;
				break;
			}
		}

		// 保留状態を解除
		pendingCenterNodeId_ = -1;
	}

	// 折りたたみボタンが押されたノードのIDを保持する
	int toggleCollapsedNodeId = -1;

	// ノードの描画
	for (auto& node : nodes_)
	{
		// ノードが非表示のセットに含まれている場合は描画をスキップする
		if (hiddenNodes.count(node.id) > 0) continue;

		// ノードの位置をImNodesに設定する必要がある場合は、ズーム率を考慮して位置を設定する
		if (node.needSetPos)
		{
			ImNodes::SetNodeGridSpacePos(node.id, ImVec2(node.pos.x * zoom_, node.pos.y * zoom_));
			node.needSetPos = false;
		}

		// ノードの開始
		ImNodes::BeginNode(node.id);


		// ノードタイトルの描画
		ImNodes::BeginNodeTitleBar();

		// セレクタノードとシーケンスノードの場合は、折りたたみ/展開のトグルボタンを描画する
		if (node.type == EditorNodeType::PersistentSelector || node.type == EditorNodeType::RestartingSelector ||
			node.type == EditorNodeType::PersistentSequence || node.type == EditorNodeType::RestartingSequence ||
			node.type == EditorNodeType::UtilitySelector)
		{
			if (ImGui::ArrowButton(node.isCollapsed ? "+" : "-", node.isCollapsed ? ImGuiDir_Right : ImGuiDir_Down))
			{
				// タイトルバーの折りたたみ/展開ボタンがクリックされたので、そのノードのIDを保持する
				toggleCollapsedNodeId = node.id;
			}
		}

		// ノード名の描画 名前が空文字の場合は、ノードの種類に応じたデフォルト名を表示する
		if (node.name[0] == '\0')
		{
			if (node.type == EditorNodeType::PersistentSelector) ImGui::TextUnformatted("永続セレクタ");
			if (node.type == EditorNodeType::PersistentSequence) ImGui::TextUnformatted("永続シーケンス");
			if (node.type == EditorNodeType::RestartingSelector) ImGui::TextUnformatted("再起動セレクタ");
			if (node.type == EditorNodeType::RestartingSequence) ImGui::TextUnformatted("再起動シーケンス");
			if (node.type == EditorNodeType::UtilitySelector) ImGui::TextUnformatted("ユーティリティセレクタ");
			if (node.type == EditorNodeType::Condition) ImGui::TextUnformatted("条件");
			if (node.type == EditorNodeType::Action)ImGui::TextUnformatted("アクション");
		}
		else
		{
			ImGui::TextUnformatted(node.name);
		}
		ImNodes::EndNodeTitleBar();


		// 入力ピンの描画
		ImNodes::BeginInputAttribute(node.inputPinId);
		ImGui::Text("In");
		ImNodes::EndInputAttribute();

		// ノードの内容の描画
		DrawNodeContent(node);

		// ノードの終了
		ImNodes::EndNode();

		// ノードの位置をImNodesから取得してノードデータに保存する
		ImVec2 currentPos = ImNodes::GetNodeGridSpacePos(node.id);
		if (currentPos.x > -99999.0f && currentPos.y > -99999.0f) 
		{
			// 取得した座標をズーム倍率で割って、実際の保存座標に戻す
			float realX = currentPos.x / zoom_;
			float realY = currentPos.y / zoom_;

			// ノードの位置が変更された場合は、変更フラグを立てる
			if (node.pos.x != realX || node.pos.y != realY)
				isDirty_ = true;

			node.pos.x = realX;
			node.pos.y = realY;
		}
	}

	// リンクの描画
	for (auto& link : links_)
	{
		int startNode = link.startNodeId;
		int endNode = link.endNodeId;

		// どちらも有効なノードIDが取得できない場合は描画をスキップする
		if (hiddenNodes.count(startNode) > 0 || hiddenNodes.count(endNode) > 0)continue;

		// リンクを描画する
		ImNodes::Link(link.id, link.startPinId, link.endPinId);
	}

	// キャンバスの空いている部分を右クリックしたときのコンテキストメニューを開くためのフラグ
	bool shouldOpenPopup = false;
	if (ImNodes::IsEditorHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
	{
		shouldOpenPopup = true;
	}


	// ノードエディタの終了
	ImNodes::EndNodeEditor();

	// ImGuiのスタイルを元に戻す
	ImGui::PopStyleVar(2);

	// ImGuiのフォントスケールを元に戻す
	ImGui::SetWindowFontScale(1.0f);

	// 折りたたみ/展開のトグルボタンがクリックされたノードがある場合は、そのノードの折りたたみ状態を変更する
	if (toggleCollapsedNodeId != -1)
	{
		// ノードの折りたたみ状態を変更する前の状態を履歴に保存する
		history_->SaveHistory(nodes_, links_, currentId_);
		isDirty_ = true;

		// 対象のノードを見つけて折りたたみ状態をトグルする
		auto it = std::find_if(nodes_.begin(), nodes_.end(), [toggleCollapsedNodeId](const EditorNode& n) { return n.id == toggleCollapsedNodeId; });
		if (it != nodes_.end())it->isCollapsed = !it->isCollapsed;
	}


	// リンクがホバーされているかどうかを取得するための変数
	int hoveredLinkId;

	// リンクがホバーされている場合は、Ctrlキーが押されていて、かつ左クリックされた瞬間かどうかを判定する
	if (ImNodes::IsLinkHovered(&hoveredLinkId))
	{
		// Ctrlキーが押されていて、かつ左クリックされた瞬間か判定
		if (ImGui::GetIO().KeyCtrl && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			// 削除前の状態を履歴に保存 (Undo対応)
			history_->SaveHistory(nodes_, links_, currentId_);

			// 対象のリンクIDを持つ要素を links_ から削除
			links_.erase(std::remove_if(links_.begin(), links_.end(),
				[hoveredLinkId](const EditorLink& link)
				{
					return link.id == hoveredLinkId;
				}), links_.end());

			// 変更があったのでフラグを立てる
			isDirty_ = true;
		}
	}


	// キャンバスの空いている部分を右クリックしたときのコンテキストメニューを開く
	if (shouldOpenPopup)
	{
		ImGui::OpenPopup("CanvasContextMenu");
	}

	// 現在のフレームで展開されているノードのIDを更新する
	prevHiddenNodes_ = hiddenNodes;
}

/// @brief ノードの内容を描画する
/// @param node 
void BehaviorTreeEditor::DrawNodeContent(EditorNode& node)
{
	// 条件ノードの場合は関数選択UIを描画
	if (node.type == EditorNodeType::Condition)
	{
		// 条件の種類に応じてUIを切り替える
		switch(node.conditionType)
		{
			// 設定なし
		case ConditionType::None:
			ImGui::Text("設定なし");
			break;

			// ターゲットがいるかどうか
		case ConditionType::HasTarget:
			ImGui::Text("ターゲットがいるかどうか");
			break;

		case ConditionType::IsTargetDown:
			ImGui::Text("ターゲットがダウンしているか");
			break;

		case ConditionType::IsNotTargetDown:
			ImGui::Text("ターゲットがダウンしていないか");
			break;

		case ConditionType::IsGrabbing:
			ImGui::Text("掴んでいるか");
			break;

		case ConditionType::IsNotGrabbing:
			ImGui::Text("掴んでいないか");
			break;

		case ConditionType::IsTargetInRange:
			ImGui::Text("ターゲットが範囲内か");
			break;

		case ConditionType::IsTargetOutOfRange:
			ImGui::Text("ターゲットが範囲外か");
			break;

		case ConditionType::IsTargetAttacking:
			ImGui::Text("ターゲットが攻撃しているか");
			break;

		case ConditionType::IsTargetNotAttacking:
			ImGui::Text("ターゲットが攻撃していないか");
			break;

		case ConditionType::IsTargetInAttackSequence:
			ImGui::Text("ターゲットが攻撃シーケンスに入っているか");
			break;

		case ConditionType::IsTargetNotInAttackSequence:
			ImGui::Text("ターゲットが攻撃シーケンスに入っていないか");
			break;
		}
	}

	// 出力ピンの描画 閉じられていないセレクタノードとシーケンスノードのみ出力ピンを描画する
	if (!node.isCollapsed &&
		(node.type == EditorNodeType::PersistentSelector || node.type == EditorNodeType::PersistentSequence ||
			node.type == EditorNodeType::RestartingSelector || node.type == EditorNodeType::RestartingSequence ||
			node.type == EditorNodeType::UtilitySelector))
	{
		ImNodes::BeginOutputAttribute(node.outputPinId);

		// 出力ピンを右側に配置するためにインデントを追加
		ImGui::Indent(60.0f * zoom_);
		ImGui::Text("Out");
		ImNodes::EndOutputAttribute();
	}

	// アクションノードの場合はアクション選択UIを描画
	if (node.type == EditorNodeType::Action)
	{
		ImGui::Text("%s", actionTypeNames[static_cast<int32_t>(node.actionType)]);
	}
}

/// @brief Utilityセレクタノードの設定UIを描画する
/// @param node 
void BehaviorTreeEditor::DrawUtilitySelectorNodeSettings(EditorNode& node)
{
	ImGui::Text("子ノードの評価関数設定:");
	ImGui::Separator();

	bool hasChildren = false;

	// エディタ上のすべての接続線（リンク）から、このノードが出発点（親）になっているものを探す
	for (const auto& link : links_)
	{
		if (link.startNodeId == node.id)
		{
			hasChildren = true;
			int childId = link.endNodeId; // 繋がっている子ノードのID

			// マップにまだこの子ノードの設定がなければ、デフォルト値をセットする
			if (node.childUtilityMap.find(childId) == node.childUtilityMap.end())
			{
				node.childUtilityMap[childId] = UtilityType::FixedDefault;
			}

			// UIをわかりやすくするため、子ノードの名前を取得する（Action名など）
			std::string childName = "Node ID: " + std::to_string(childId);
			auto childIt = std::find_if(nodes_.begin(), nodes_.end(), [childId](const EditorNode& n) { return n.id == childId; });
			if (childIt != nodes_.end())
			{
				childName = childIt->name;
			}

			// 子ノードの名前を表示
			ImGui::Text("▶ %s", childName.c_str());

			// コンボボックスの描画
			int currentItem = static_cast<int>(node.childUtilityMap[childId]);

			// ImGuiでは同じラベルのUIが複数あるとバグるため、ラベル名にIDを混ぜて一意にする（##以降は画面に表示されない）
			std::string comboLabel = "##UtilityCombo_" + std::to_string(childId);

			ImGui::PushItemWidth(static_cast<float>(120.0f * zoom_));
			if (ImGui::Combo(comboLabel.c_str(), &currentItem, utilityTypeNames, IM_ARRAYSIZE(utilityTypeNames)))
			{
				// 変更があったら履歴保存＆フラグ立て
				history_->SaveHistory(nodes_, links_, currentId_);
				isDirty_ = true;

				// マップの値を更新
				node.childUtilityMap[childId] = static_cast<UtilityType>(currentItem);
			}
			ImGui::PopItemWidth();

			ImGui::Spacing(); // 少し隙間を空ける
		}
	}

	if (!hasChildren)
	{
		ImGui::TextDisabled("※接続されている子ノードがありません");
	}
}

/// @brief 条件ノードの設定UIを描画する
/// @param node 
void BehaviorTreeEditor::DrawConditionNodeSettings(EditorNode& node)
{
	ImGui::Text("関数 :");

	// 履歴と変更フラグをまとめて処理するラムダ関数
	auto HistorySaveIfChanged = [this]() {if (ImGui::IsItemActivated()) { history_->SaveHistory(nodes_, links_, currentId_);isDirty_ = true; }};

	// コンボボックスを描画し、変更があったらEnumにキャストして戻す
	int currentItem = static_cast<int>(node.conditionType);
	ImGui::PushItemWidth(static_cast<float>(120.0f * zoom_));
	if (ImGui::Combo("条件", &currentItem, conditionTypeNames, IM_ARRAYSIZE(conditionTypeNames)))
	{
		history_->SaveHistory(nodes_, links_, currentId_);
		isDirty_ = true;

		node.conditionType = static_cast<ConditionType>(currentItem);
	}

	// ターゲットとの距離を条件にする場合は、距離の入力UIを表示
	if (node.conditionType == ConditionType::IsTargetInRange || node.conditionType == ConditionType::IsTargetOutOfRange)
	{
		HistorySaveIfChanged();
		ImGui::DragFloat("距離", &node.conditionParam.distanceToTarget, 0.01f, 0.0f, 10000.0f);
	}
}

/// @brief アクションノードの設定UIを描画する
/// @param node 
void BehaviorTreeEditor::DrawActionNodeSettings(EditorNode& node)
{
	ImGui::PushItemWidth(static_cast<float>(120.0f * zoom_));

	// 履歴と変更フラグをまとめて処理するラムダ関数
	auto HistorySaveIfChanged = [this]() {if (ImGui::IsItemActivated()) { history_->SaveHistory(nodes_, links_, currentId_);isDirty_ = true; }};

	// コンボボックスに表示する文字列の配列
	int currentItem = 0;
	for (int i = 0; i < IM_ARRAYSIZE(actionTypeNames); ++i)
	{
		if (actionTypeNames[static_cast<int32_t>(node.actionType)] == actionTypeNames[i])
		{
			currentItem = i;
			break;
		}
	}

	// コンボボックスを描画し、変更があったら選択された文字列をノードに保存
	if (ImGui::Combo("アクションの種類", &currentItem, actionTypeNames, IM_ARRAYSIZE(actionTypeNames)))
	{
		history_->SaveHistory(nodes_, links_, currentId_);
		isDirty_ = true;

		// 選択された文字列をノードに保存
		node.actionType = static_cast<ActionType>(currentItem);
	}

	// 選択されているアクションに応じてパラメータ設定UIを切り替える
	if (node.actionType == ActionType::ComboAttack)
	{
		if (ImGui::TreeNode("コンボ攻撃 設定"))
		{
			HistorySaveIfChanged();
			ImGui::DragFloat("攻撃時間", &node.comboAttackInitData.attackTime, 0.01f, 0.0f, 10.0f);

			HistorySaveIfChanged();
			ImGui::DragFloat("移動速度", &node.comboAttackInitData.moveSpeed, 0.1f);

			HistorySaveIfChanged();
			ImGui::DragFloat("移動開始時間", &node.comboAttackInitData.moveStartTime, 0.01f);

			HistorySaveIfChanged();
			ImGui::DragFloat("移動終了時間", &node.comboAttackInitData.moveEndTime, 0.01f);

			HistorySaveIfChanged();
			ImGui::Checkbox("武器をつかむ", &node.comboAttackInitData.isGrabWeapon);

			if (node.comboAttackInitData.isGrabWeapon)
			{
				HistorySaveIfChanged();
				ImGui::DragFloat("武器をつかむ開始時間", &node.comboAttackInitData.grabWeaponStartTime, 0.01f, 0.0f, node.comboAttackInitData.grabWeaponEndTime);

				HistorySaveIfChanged();
				ImGui::DragFloat("武器をつかむ終了時間", &node.comboAttackInitData.grabWeaponEndTime, 0.01f, node.comboAttackInitData.grabWeaponStartTime, node.comboAttackInitData.attackTime);
			}


			// 選択されたモーションタイプに応じたモーション名のリストをMotionManagerから取得
			node.motionType = MotionType::Attack;
			std::vector<std::string> motionNames = MotionManager::GetInstance()->GetMotionNames(node.motionType);

			// モーション名のリストが空の場合はエラーメッセージを表示
			if (motionNames.empty())
			{
				ImGui::TextColored(ImVec4(1, 0, 0, 1), "モーションがロードされていません");
			}
			else
			{
				// 現在選択されているモーション名をプレビュー用の文字列として設定
				const char* previewValue = node.motionName.empty() ? "モーションを選択..." : node.motionName.c_str();

				// モーション名選択用のコンボボックスを描画
				if (ImGui::BeginCombo("攻撃モーション", previewValue))
				{
					for (const auto& name : motionNames)
					{
						// 現在のモーション名と同じものが選択されている状態にする
						bool isSelected = (node.motionName == name);
						if (ImGui::Selectable(name.c_str(), isSelected))
						{
							history_->SaveHistory(nodes_, links_, currentId_);
							isDirty_ = true;

							node.motionName = name;
						}
						if (isSelected) ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
			}


			ImGui::Text("当たり判定");

			// 当たり判定のリストを描画
			auto& hitDefs = node.comboAttackInitData.hitDefinitions;
			for (size_t i = 0; i < hitDefs.size(); ++i)
			{
				// 各当たり判定のUIを描画するためにIDをプッシュ
				ImGui::PushID(static_cast<int>(i));

				if (ImGui::TreeNode((std::string("当たり判定 ") + std::to_string(i + 1)).c_str()))
				{
					HistorySaveIfChanged();
					ImGui::DragFloat("開始時間", &hitDefs[i].startTime, 0.01f);

					HistorySaveIfChanged();
					ImGui::DragFloat("終了時間", &hitDefs[i].endTime, 0.01f);

					HistorySaveIfChanged();
					ImGui::DragFloat("半径", &hitDefs[i].radius, 0.01f);

					HistorySaveIfChanged();
					ImGui::InputInt("攻撃力", &hitDefs[i].damage);

					HistorySaveIfChanged();
					ImGui::DragFloat("ノックバック", &hitDefs[i].knockback, 0.1f);

					HistorySaveIfChanged();
					ImGui::DragFloat3("ノックバック方向", &hitDefs[i].knockbackDirection.x, 0.1f);

					// ノーマライズされた方向ベクトルを維持するために、ドラッグ後にベクトルを正規化
					hitDefs[i].knockbackDirection = hitDefs[i].knockbackDirection.Normalize();

					// ダメージリアクション
					const char* damageReactionNames[] = { "None", "LightStagger", "HeavyStagger", "Down" };
					int currentReaction = static_cast<int>(hitDefs[i].damageReaction);
					if (ImGui::Combo("ダメージリアクション", &currentReaction, damageReactionNames, IM_ARRAYSIZE(damageReactionNames)))
					{
						history_->SaveHistory(nodes_, links_, currentId_);
						isDirty_ = true;

						hitDefs[i].damageReaction = static_cast<DamageReaction>(currentReaction);
					}

					// ジョイントタイプ
					int currentJoint = static_cast<int>(hitDefs[i].jointType);
					if (ImGui::Combo("ジョイントタイプ", &currentJoint, jointTypeNames, IM_ARRAYSIZE(jointTypeNames)))
					{
						history_->SaveHistory(nodes_, links_, currentId_);
						isDirty_ = true;

						hitDefs[i].jointType = static_cast<JointType>(currentJoint);
					}

					// 削除ボタン
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
					if (ImGui::Button("当たり判定を削除"))
					{
						// 当たり判定を削除する前に、履歴に保存して変更フラグを立てる
						history_->SaveHistory(nodes_, links_, currentId_);
						isDirty_ = true;

						hitDefs.erase(hitDefs.begin() + i);
						ImGui::PopStyleColor();
						ImGui::TreePop();
						ImGui::PopID();
						break; // 要素を削除した場合はループを抜ける（次のフレームで再描画される）
					}

					ImGui::PopStyleColor();

					ImGui::TreePop();
				}
				ImGui::PopID();
			}

			// 新しい判定を追加するボタン
			if (ImGui::Button("当たり判定を追加"))
			{
				// 新しい当たり判定を追加する前に、履歴に保存して変更フラグを立てる
				history_->SaveHistory(nodes_, links_, currentId_);
				isDirty_ = true;

				HitboxDefinition newDef;
				hitDefs.push_back(newDef);
			}

			ImGui::TreePop();
		}
	}
	else if (node.actionType == ActionType::GrabAttack)
	{
		if (ImGui::TreeNode("掴み 設定"))
		{
			HistorySaveIfChanged();
			ImGui::DragFloat("攻撃時間", &node.grabAttackInitData.attackTime, 0.01f);

			HistorySaveIfChanged();
			ImGui::DragFloat("掴み時間", &node.grabAttackInitData.grabTime, 0.01f);

			HistorySaveIfChanged();
			ImGui::DragFloat("移動速度", &node.grabAttackInitData.moveSpeed, 0.1f);

			HistorySaveIfChanged();
			ImGui::DragFloat("移動開始時間", &node.grabAttackInitData.moveStartTime, 0.01f);

			HistorySaveIfChanged();
			ImGui::DragFloat("移動終了時間", &node.grabAttackInitData.moveEndTime, 0.01f);

			HistorySaveIfChanged();
			ImGui::DragFloat("当たり判定開始時間", &node.grabAttackInitData.hitboxStartTime, 0.01f);

			HistorySaveIfChanged();
			ImGui::DragFloat("当たり判定終了時間", &node.grabAttackInitData.hitboxEndTime, 0.01f);

			HistorySaveIfChanged();
			ImGui::Checkbox("武器をつかむ", &node.grabAttackInitData.isGrabWeapon);

			if (node.grabAttackInitData.isGrabWeapon)
			{
				HistorySaveIfChanged();
				ImGui::DragFloat("武器をつかむ開始時間", &node.grabAttackInitData.grabWeaponStartTime, 0.01f, 0.0f, node.grabAttackInitData.grabWeaponEndTime);

				HistorySaveIfChanged();
				ImGui::DragFloat("武器をつかむ終了時間", &node.grabAttackInitData.grabWeaponEndTime, 0.01f, node.grabAttackInitData.grabWeaponStartTime, node.grabAttackInitData.attackTime);
			}


			// 選択されたモーションタイプに応じたモーション名のリストをMotionManagerから取得
			node.motionType = MotionType::Attack;
			std::vector<std::string> motionNames = MotionManager::GetInstance()->GetMotionNames(node.motionType);

			// モーション名のリストが空の場合はエラーメッセージを表示
			if (motionNames.empty())
			{
				ImGui::TextColored(ImVec4(1, 0, 0, 1), "モーションがロードされていません");
			}
			else
			{
				// 現在選択されているモーション名をプレビュー用の文字列として設定
				const char* previewValue = node.motionName.empty() ? "モーションを選択..." : node.motionName.c_str();

				// モーション名選択用のコンボボックスを描画
				if (ImGui::BeginCombo("攻撃モーション", previewValue))
				{
					for (const auto& name : motionNames)
					{
						// 現在のモーション名と同じものが選択されている状態にする
						bool isSelected = (node.motionName == name);
						if (ImGui::Selectable(name.c_str(), isSelected))
						{
							history_->SaveHistory(nodes_, links_, currentId_);
							isDirty_ = true;

							node.motionName = name;
						}
						if (isSelected) ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
			}


			// ジョイントタイプ
			int currentJoint = static_cast<int>(node.grabAttackInitData.jointType);
			if (ImGui::Combo("Joint Type", &currentJoint, jointTypeNames, IM_ARRAYSIZE(jointTypeNames)))
			{
				history_->SaveHistory(nodes_, links_, currentId_);
				isDirty_ = true;

				node.grabAttackInitData.jointType = static_cast<JointType>(currentJoint);
			}

			ImGui::TreePop();
		}
	}
	else if (node.actionType == ActionType::GrabStrikeAttack)
	{
		if (ImGui::TreeNode("掴み攻撃 設定"))
		{
			HistorySaveIfChanged();
			ImGui::DragFloat("攻撃時間", &node.grabStrikeAttackInitData.attackTime, 0.01f);

			HistorySaveIfChanged();
			ImGui::Checkbox("離すかどうか", &node.grabStrikeAttackInitData.isRelease);

			if (node.grabStrikeAttackInitData.isRelease)
			{
				HistorySaveIfChanged();
				ImGui::DragFloat("移動速度", &node.grabStrikeAttackInitData.moveSpeed, 0.1f);

				HistorySaveIfChanged();
				ImGui::DragFloat("移動開始時間", &node.grabStrikeAttackInitData.moveStartTime, 0.01f);

				HistorySaveIfChanged();
				ImGui::DragFloat("移動終了時間", &node.grabStrikeAttackInitData.moveEndTime, 0.01f);

				HistorySaveIfChanged();
				ImGui::DragFloat("ノックバック", &node.grabStrikeAttackInitData.knockback, 0.1f);

				HistorySaveIfChanged();
				ImGui::DragFloat3("ノックバック方向", &node.grabStrikeAttackInitData.knockbackDirection.x, 0.1f);

				// ノーマライズされた方向ベクトルを維持するために、ドラッグ後にベクトルを正規化
				node.grabStrikeAttackInitData.knockbackDirection = node.grabStrikeAttackInitData.knockbackDirection.Normalize();

				// ダメージリアクション
				const char* damageReactionNames[] = { "None", "LightStagger", "HeavyStagger", "Down" };
				int currentReaction = static_cast<int>(node.grabStrikeAttackInitData.damageReaction);
				if (ImGui::Combo("ダメージリアクション", &currentReaction, damageReactionNames, IM_ARRAYSIZE(damageReactionNames)))
				{
					history_->SaveHistory(nodes_, links_, currentId_);
					isDirty_ = true;

					node.grabStrikeAttackInitData.damageReaction = static_cast<DamageReaction>(currentReaction);
				}
			}


			// 選択されたモーションタイプに応じたモーション名のリストをMotionManagerから取得
			node.motionType = MotionType::Attack;
			std::vector<std::string> attackerMotions = MotionManager::GetInstance()->GetMotionNames(node.motionType);

			// モーション名のリストが空の場合はエラーメッセージを表示
			if (attackerMotions.empty())
			{
				ImGui::TextColored(ImVec4(1, 0, 0, 1), "モーションがロードされていません");
			}
			else
			{
				// 現在選択されているモーション名をプレビュー用の文字列として設定
				const char* previewValue = node.motionName.empty() ? "モーションを選択..." : node.motionName.c_str();

				// モーション名選択用のコンボボックスを描画
				if (ImGui::BeginCombo("攻撃モーション", previewValue))
				{
					for (const auto& name : attackerMotions)
					{
						// 現在のモーション名と同じものが選択されている状態にする
						bool isSelected = (node.motionName == name);
						if (ImGui::Selectable(name.c_str(), isSelected))
						{
							history_->SaveHistory(nodes_, links_, currentId_);
							isDirty_ = true;

							node.motionName = name;
						}
						if (isSelected) ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
			}


			// 選択されたモーションタイプに応じたモーション名のリストをMotionManagerから取得
			node.targetMotionType = MotionType::Stagger;
			std::vector<std::string> targetMotions = MotionManager::GetInstance()->GetMotionNames(node.targetMotionType);

			// モーション名のリストが空の場合はエラーメッセージを表示
			if (targetMotions.empty())
			{
				ImGui::TextColored(ImVec4(1, 0, 0, 1), "モーションがロードされていません");
			}
			else
			{
				// 現在選択されているモーション名をプレビュー用の文字列として設定
				const char* previewValue = node.targetMotionName.empty() ? "モーションを選択..." : node.targetMotionName.c_str();

				// モーション名選択用のコンボボックスを描画
				if (ImGui::BeginCombo("怯みモーション", previewValue))
				{
					for (const auto& name : targetMotions)
					{
						// 現在のモーション名と同じものが選択されている状態にする
						bool isSelected = (node.targetMotionName == name);
						if (ImGui::Selectable(name.c_str(), isSelected))
						{
							history_->SaveHistory(nodes_, links_, currentId_);
							isDirty_ = true;

							node.targetMotionName = name;
						}
						if (isSelected) ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
			}


			ImGui::Text("当たり判定");

			// 当たり判定のリストを描画
			auto& hitDefs = node.grabStrikeAttackInitData.hits;
			for (size_t i = 0; i < hitDefs.size(); ++i)
			{
				// 各当たり判定のUIを描画するためにIDをプッシュ
				ImGui::PushID(static_cast<int>(i));

				if (ImGui::TreeNode((std::string("当たり判定 ") + std::to_string(i + 1)).c_str()))
				{
					HistorySaveIfChanged();
					ImGui::InputInt("攻撃力", &hitDefs[i].damage);

					HistorySaveIfChanged();
					ImGui::DragFloat("攻撃が入る時間", &hitDefs[i].hitTime, 0.01f, 0.0f, 100000.0f);

					// ジョイントタイプ
					int currentJoint = static_cast<int>(hitDefs[i].hitJoint);
					if (ImGui::Combo("ジョイントタイプ", &currentJoint, jointTypeNames, IM_ARRAYSIZE(jointTypeNames)))
					{
						history_->SaveHistory(nodes_, links_, currentId_);
						isDirty_ = true;

						hitDefs[i].hitJoint = static_cast<JointType>(currentJoint);
					}

					// 削除ボタン
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
					if (ImGui::Button("当たり判定を削除"))
					{
						// 当たり判定を削除する前に、履歴に保存して変更フラグを立てる
						history_->SaveHistory(nodes_, links_, currentId_);
						isDirty_ = true;

						hitDefs.erase(hitDefs.begin() + i);
						ImGui::PopStyleColor();
						ImGui::TreePop();
						ImGui::PopID();
						break; // 要素を削除した場合はループを抜ける（次のフレームで再描画される）
					}

					ImGui::PopStyleColor();

					ImGui::TreePop();
				}
				ImGui::PopID();
			}

			// 新しい判定を追加するボタン
			if (ImGui::Button("当たり判定を追加"))
			{
				// 新しい当たり判定を追加する前に、履歴に保存して変更フラグを立てる
				history_->SaveHistory(nodes_, links_, currentId_);
				isDirty_ = true;

				HitDefinition newDef;
				hitDefs.push_back(newDef);
			}



			ImGui::TreePop();
		}
	}
	else if (node.actionType == ActionType::Avoid)
	{
		if (ImGui::TreeNode("回避 設定"))
		{
			HistorySaveIfChanged();
			ImGui::DragFloat2("回避方向", &node.avoidInitData.localDirection.x, 0.01f);
			node.avoidInitData.localDirection = node.avoidInitData.localDirection.Normalize();

			HistorySaveIfChanged();
			ImGui::DragFloat("回避距離", &node.avoidInitData.distance, 0.01f);

			HistorySaveIfChanged();
			ImGui::DragFloat("回避時間", &node.avoidInitData.time, 0.01f);

			ImGui::TreePop();
		}
	}
	else if (node.actionType == ActionType::NavMeshMove)
	{
		if (ImGui::TreeNode("ナビメッシュ移動 設定"))
		{
			HistorySaveIfChanged();
			ImGui::DragFloat("移動速度", &node.navMeshMoveInitData.moveSpeed, 0.1f);

			HistorySaveIfChanged();
			ImGui::DragFloat("停止距離", &node.navMeshMoveInitData.stopDistance, 0.01f);

			HistorySaveIfChanged();
			ImGui::Checkbox("走るかどうか", &node.navMeshMoveInitData.isDash);

			ImGui::TreePop();
		}
	}
	else if (node.actionType == ActionType::NavMeshLeaderMove)
	{
		if (ImGui::TreeNode("ナビメッシュリーダー移動 設定"))
		{
			HistorySaveIfChanged();
			ImGui::DragFloat("移動速度", &node.navMeshLeaderMoveInitData.moveSpeed, 0.1f);

			HistorySaveIfChanged();
			ImGui::DragFloat("停止距離", &node.navMeshLeaderMoveInitData.stopDistance, 0.01f);

			HistorySaveIfChanged();
			ImGui::Checkbox("走るかどうか", &node.navMeshLeaderMoveInitData.isDash);

			ImGui::TreePop();
		}
	}
	else if (node.actionType == ActionType::RequestToken)
	{
		ImGui::Combo("トークンの種類", reinterpret_cast<int*>(&node.tokenType), tokenTypeNames, IM_ARRAYSIZE(tokenTypeNames));
	}

	ImGui::PopItemWidth();
}