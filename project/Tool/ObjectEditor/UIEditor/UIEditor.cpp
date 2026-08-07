#include "UIEditor.h"
#include "UISerializer/UISerializer.h"

/// @brief コンストラクタ
UIEditor::UIEditor()
{
	// テクスチャフォルダを走査してロードする
	RefreshTextureList();
}

/// @brief 読み込む
/// @param filename 
void UIEditor::Load(const std::string& filename)
{
	// ファイルパスを生成
	std::string filePath = kUIDir + filename + ".json";

	// ファイルを読み込む前に、現在の状態を履歴に保存
	SaveHistoryState();

	selectedElementIndex_ = -1;
	uiElements_ = FromJson(filePath, loadedTextures_);

	// ファイル名から拡張子を除いた名前を取得して、保存用の入力欄にセット
	strcpy_s(inputFilename_, filename.c_str());

	// 読み込み成功時に「今開いているファイル名」として保持
	currentFileName_ = filename;
	isFileOpen_ = true;
}

/// @brief 描画処理
void UIEditor::Draw()
{
	// 保持しているすべてのUI要素（スプライト）を描画する
	for (auto& elem : uiElements_)
	{
		if (elem.sprite)
		{
			elem.sprite->Draw();
		}
	}
}

/// @brief UIを描画する
void UIEditor::DrawUI()
{
#ifdef DEVELOPMENT

	// ImGuiのIOを取得
	ImGuiIO& io = ImGui::GetIO();
	bool isCtrl = io.KeyCtrl;

	// テキスト入力中やアイテムがアクティブな場合には無効にする
	if (!ImGui::GetIO().WantTextInput && !ImGui::IsAnyItemActive())
	{
		// Ctrl + Z で Undo
		if (isCtrl && ImGui::IsKeyPressed(ImGuiKey_Z))
		{
			Undo();
		}

		// Ctrl + Y で Redo
		if (isCtrl && ImGui::IsKeyPressed(ImGuiKey_Y))
		{
			Redo();
		}

		// Ctrl + S で 保存
		if (isCtrl && ImGui::IsKeyPressed(ImGuiKey_S))
		{
			Save();
		}

		// Deleteキーで選択中のスプライトを削除
		if (ImGui::IsKeyPressed(ImGuiKey_Delete) || ImGui::IsKeyPressed(ImGuiKey_Backspace))
		{
			DeleteSelectedElement();
		}
	}

	// ImGuiウィンドウの描画
	DrawHierarchyWindow();
	DrawInspectorWindow();
	DrawAssetsWindow();

#endif
}

/// @brief スプライトを取得する
/// @param name 
/// @return 
Sprite* UIEditor::GetSprite(const std::string& name) const
{
	for (const auto& elem : uiElements_)
	{
		if (elem.name == name)
		{
			// 見つかったらポインタを返す
			return elem.sprite.get();
		}
	}

	// 見つからなかった場合
	return nullptr;
}

/// @brief テクスチャフォルダを走査してロードする
void UIEditor::RefreshTextureList()
{
	// リストをリセット
	loadedTextures_.clear();
	textureNames_.clear();

	// ディレクトリが存在しない場合は作成
	if (!std::filesystem::exists(kTextureDir))
	{
		std::filesystem::create_directories(kTextureDir);
	}

	// ディレクトリ内を走査
	for (const auto& entry : std::filesystem::directory_iterator(kTextureDir))
	{
		// PNGファイルのみを対象とする
		if (entry.path().extension() == ".png")
		{
			std::string filename = entry.path().filename().string();
			std::string fullPath = kTextureDir + filename;

			// テクスチャをロード
			TextureHandle handle = GrowthEngine::GetInstance()->LoadTexture(fullPath);

			// コンボボックス表示用とハンドル検索用に登録
			textureNames_.push_back(filename);
			loadedTextures_[filename] = handle;
		}
	}
}

/// @brief ヒエラルキーウィンドウ描画
void UIEditor::DrawHierarchyWindow()
{
#ifdef DEVELOPMENT

	if (ImGui::Begin("UI - ヒエラルキー"))
	{
		for (int i = 0; i < uiElements_.size(); ++i)
		{
			bool isSelected = (selectedElementIndex_ == i);
			if (ImGui::Selectable(uiElements_[i].name.c_str(), isSelected))
			{
				selectedElementIndex_ = i;
			}
		}
	}
	ImGui::End();

#endif
}

/// @brief インスペクターウィンドウ描画
void UIEditor::DrawInspectorWindow()
{
#ifdef DEVELOPMENT

	if (ImGui::Begin("UI - インスペクター"))
	{
		if (selectedElementIndex_ >= 0 && selectedElementIndex_ < uiElements_.size())
		{
			auto& selectedData = uiElements_[selectedElementIndex_];
			auto* param = selectedData.sprite->param_;

			// 名前の変更
			char nameBuffer[64];
			strcpy_s(nameBuffer, selectedData.name.c_str());

			// Enterキーが押されたか、入力が終了したかを判定
			bool isNameEntered = ImGui::InputText("名前", nameBuffer, IM_ARRAYSIZE(nameBuffer), ImGuiInputTextFlags_EnterReturnsTrue);

			// 名前が変更された場合、または入力が終了した場合に処理を行う
			if (isNameEntered || ImGui::IsItemDeactivatedAfterEdit())
			{
				std::string newName(nameBuffer);
				if (newName != selectedData.name)
				{
					SaveHistoryState();

					// 自分自身のインデックスを除外して重複チェック
					selectedData.name = GetUniqueName(newName, selectedElementIndex_);
				}
			}


			// テクスチャのコンボボックス
			if (!textureNames_.empty())
			{
				std::vector<const char*> items;
				int currentItemIndex = -1;
				for (int i = 0; i < textureNames_.size(); ++i)
				{
					items.push_back(textureNames_[i].c_str());
					if (textureNames_[i] == selectedData.textureFilename)
						currentItemIndex = i;
				}

				if (ImGui::Combo("テクスチャ", &currentItemIndex, items.data(), static_cast<int>(items.size())))
				{
					selectedData.textureFilename = textureNames_[currentItemIndex];
					param->material.hTexture = loadedTextures_[selectedData.textureFilename];
				}
			}

			// トランスフォームの編集
			ImGui::DragFloat2("位置 (Translate)", &param->transform.translate.x, 1.0f);
			if (ImGui::IsItemActivated()) SaveHistoryState();

			ImGui::DragFloat2("大きさ (Scale)", &param->transform.scale.x, 0.01f);
			if (ImGui::IsItemActivated()) SaveHistoryState();

			ImGui::DragFloat("回転 (Rotate)", &param->transform.rotate, 0.01f);
			if (ImGui::IsItemActivated()) SaveHistoryState();


			// 色の編集
			ImGui::ColorEdit4("色 (Color)", &param->material.color.x);
			if (ImGui::IsItemActivated()) SaveHistoryState();

			// アンカーのコンボボックス
			const char* anchorNames[] = { "LeftTop", "Top", "RightTop","Left", "Center", "Right","LeftBottom", "Bottom", "RightBottom" };
			int currentAnchor = static_cast<int>(param->screenAnchor);
			if (ImGui::Combo("アンカー", &currentAnchor, anchorNames, IM_ARRAYSIZE(anchorNames)))
			{
				SaveHistoryState();
				param->screenAnchor = static_cast<Engine::Render2D::ScreenAnchor>(currentAnchor);
			}

			// ブレンドモードのコンボボックス
			const char* blendNames[] = { "なし", "ノーマル", "加算", "減算", "乗算", "スクリーン" };
			int currentBlend = static_cast<int>(param->blendMode);
			if (ImGui::Combo("ブレンドモード", &currentBlend, blendNames, IM_ARRAYSIZE(blendNames)))
			{
				SaveHistoryState();
				param->blendMode = static_cast<BlendMode>(currentBlend);
			}

			ImGui::Separator();
			ImGui::Text("--- 描画順（レイヤー） ---");

			// ひとつ前（奥）に移動
			if (ImGui::Button("上へ移動") && selectedElementIndex_ > 0)
			{
				SaveHistoryState();
				std::swap(uiElements_[selectedElementIndex_], uiElements_[selectedElementIndex_ - 1]);
				selectedElementIndex_--;
			}
			ImGui::SameLine();

			// ひとつ後ろ（手前）に移動
			if (ImGui::Button("下へ移動") && selectedElementIndex_ < uiElements_.size() - 1)
			{
				SaveHistoryState();
				std::swap(uiElements_[selectedElementIndex_], uiElements_[selectedElementIndex_ + 1]);
				selectedElementIndex_++;
			}
			ImGui::SameLine();

			// スプライトの削除
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
			if (ImGui::Button("削除"))
			{
				SaveHistoryState();
				DeleteSelectedElement();
			}
			ImGui::PopStyleColor();
		}
		else
		{
			// 何も選択されていない時の案内表示
			ImGui::Text("ヒエラルキーからUI要素を選択してください。");
		}
	}
	ImGui::End();

#endif
}

/// @brief アセットウィンドウ描画
void UIEditor::DrawAssetsWindow()
{
#ifdef DEVELOPMENT
	
	if (ImGui::Begin("UI - アセットブラウザ"))
	{
		// 新規ファイル作成ボタン
		if (ImGui::Button("新規ファイル作成"))
		{
			// ポップアップを開く前に入力欄にデフォルト名を入れておく
			strcpy_s(inputFilename_, "ui_new");
			ImGui::OpenPopup("新規ファイル作成ポップアップ");
		}

		// 新規ファイル作成のポップアップ
		if (ImGui::BeginPopupModal("新規ファイル作成ポップアップ", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::InputText("ファイル名", inputFilename_, IM_ARRAYSIZE(inputFilename_));

			ImGui::Separator();

			// 作成ボタン
			if (ImGui::Button("作成", ImVec2(120, 0)))
			{
				if (strlen(inputFilename_) > 0)
				{
					// 既存のデータをクリア
					uiElements_.clear();
					selectedElementIndex_ = -1;

					// 入力された名前を「今開いているファイル名」として保持
					currentFileName_ = inputFilename_;
					isFileOpen_ = true;

					// 保持した名前で空のファイルを作成
					Save();

					ImGui::CloseCurrentPopup(); // ポップアップを閉じる
				}
			}

			ImGui::SameLine();

			// キャンセルボタン
			if (ImGui::Button("キャンセル", ImVec2(120, 0)))
			{
				ImGui::CloseCurrentPopup(); // 何もせずに閉じる
			}

			ImGui::EndPopup();
		}


		// ファイルが開いているとき
		if (isFileOpen_)
		{
			ImGui::SameLine();
			ImGui::Text("編集中: %s.json", currentFileName_.c_str());
			ImGui::Separator();

			// 保存ボタン
			if (ImGui::Button("保存"))
				Save();

			ImGui::SameLine();
			if (ImGui::Button("テクスチャ再読み込み")) RefreshTextureList();
			
			ImGui::SameLine();

			static char newSpriteName[64] = "New Sprite";
			static int newTextureIndex = 0;

			// 新規スプライト追加ボタン
			if (ImGui::Button("新規スプライト追加"))
			{
				// ポップアップを開く前に初期値をセット
				strcpy_s(newSpriteName, "New Sprite");
				newTextureIndex = 0;
				ImGui::OpenPopup("新規スプライト作成");
			}

			// 新規スプライト作成のポップアップ
			if (ImGui::BeginPopupModal("新規スプライト作成", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				// 名前の入力
				ImGui::InputText("名前", newSpriteName, IM_ARRAYSIZE(newSpriteName));

				// テクスチャの選択
				if (!textureNames_.empty())
				{
					std::vector<const char*> items;
					for (const auto& name : textureNames_)
					{
						items.push_back(name.c_str());
					}
					ImGui::Combo("テクスチャ", &newTextureIndex, items.data(), static_cast<int>(items.size()));
				} 
				else
				{
					ImGui::TextDisabled("※テクスチャが見つかりません");
				}

				ImGui::Separator();

				// 作成ボタン
				if (ImGui::Button("作成", ImVec2(120, 0)))
				{
					// 新規作成時の状態を履歴に保存
					SaveHistoryState();

					UIElementData newData;
					newData.name = GetUniqueName(newSpriteName);

					// テクスチャが選択されているか確認
					TextureHandle hTex = 0;
					if (!textureNames_.empty() && newTextureIndex >= 0 && newTextureIndex < textureNames_.size())
					{
						newData.textureFilename = textureNames_[newTextureIndex];
						hTex = loadedTextures_[newData.textureFilename];
					} else
					{
						newData.textureFilename = "";
					}

					// スプライトを生成してリストに追加
					newData.sprite = std::make_unique<Sprite>(hTex, newData.name);
					uiElements_.push_back(std::move(newData));

					// 選択状態を新規作成したものに合わせる
					selectedElementIndex_ = static_cast<int>(uiElements_.size()) - 1;

					ImGui::CloseCurrentPopup();
				}

				ImGui::SameLine();

				// キャンセルボタン
				if (ImGui::Button("キャンセル", ImVec2(120, 0)))
				{
					ImGui::CloseCurrentPopup(); // 何もせずに閉じる
				}

				ImGui::EndPopup();
			}
		}

		ImGui::Separator();

		// ファイル一覧の表示
		if (std::filesystem::exists(kUIDir))
		{
			// ウィンドウの右端の座標を取得（折り返しの計算用）
			float windowVisibleX = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
			ImGuiStyle& style = ImGui::GetStyle();

			// ディレクトリ内のファイルを走査
			for (const auto& entry : std::filesystem::directory_iterator(kUIDir))
			{
				// .json ファイルのみを対象とする
				if (entry.path().extension() == ".json")
				{
					std::string filename = entry.path().filename().string();
					std::string path = entry.path().string();

					// ボタンとして表示（50x50のサイズ）
					if (ImGui::Button(filename.c_str(), ImVec2(50, 50)))
					{
						// ファイルを読み込む前に、現在の状態を履歴に保存
						SaveHistoryState(); 

						selectedElementIndex_ = -1;
						uiElements_ = FromJson(path, loadedTextures_);

						// ファイル名から拡張子を除いた名前を取得して、保存用の入力欄にセット
						std::string nameWithoutExt = entry.path().stem().string();
						strcpy_s(inputFilename_, nameWithoutExt.c_str());

						// 読み込み成功時に「今開いているファイル名」として保持
						currentFileName_ = nameWithoutExt;
						isFileOpen_ = true;
					}

					// 次のアイテムを描画した時にウィンドウの右端をはみ出さないか計算
					float lastItemMaxX = ImGui::GetItemRectMax().x;
					float nextItemMaxX = lastItemMaxX + style.ItemSpacing.x + 100.0f;

					// はみ出さない場合は横に並べる（SameLine）、はみ出す場合はそのまま（自動で改行される）
					if (nextItemMaxX < windowVisibleX)
					{
						ImGui::SameLine();
					}
				}
			}
		}
		else
		{
			ImGui::TextDisabled("ディレクトリが見つかりません: %s", kUIDir.c_str());
		}
	}
	ImGui::End();

#endif
}

/// @brief UI要素の名前が重複しないようにユニークな名前を生成する
/// @param baseName 
/// @param ignoreIndex 
/// @return 
std::string UIEditor::GetUniqueName(const std::string& baseName, int ignoreIndex) const
{
	std::string currentName = baseName;
	int count = 1;

	while (true)
	{
		bool isDuplicate = false;

		for (int i = 0; i < uiElements_.size(); ++i)
		{
			// 自分自身は重複チェックから除外する
			if (i == ignoreIndex) continue;

			// 名前が重複しているかチェック
			if (uiElements_[i].name == currentName)
			{
				isDuplicate = true;
				break;
			}
		}

		// 重複がなければその名前で確定
		if (!isDuplicate)
			break;

		// 重複している場合は _1, _2 のように連番を付与して再チェック
		currentName = baseName + "_" + std::to_string(count);
		count++;
	}

	return currentName;
}

/// @brief UIデータをファイルに保存する
void UIEditor::Save()
{
	// ファイル名が空の場合は保存しない
	if (currentFileName_.empty()) return;

	// ディレクトリが存在しない場合は作成
	if (!std::filesystem::exists(kUIDir))
		std::filesystem::create_directories(kUIDir);

	// 拡張子が .json でない場合は追加する
	std::string filenameStr = currentFileName_;
	if (filenameStr.find(".json") == std::string::npos)
		filenameStr += ".json";

	std::string path = kUIDir + filenameStr;
	ToJson(path, uiElements_);
}

/// @brief 選択中のUI要素を削除する
void UIEditor::DeleteSelectedElement()
{
	if (selectedElementIndex_ >= 0 && selectedElementIndex_ < uiElements_.size())
	{
		SaveHistoryState();
		uiElements_.erase(uiElements_.begin() + selectedElementIndex_);
		selectedElementIndex_ = -1; // 選択解除
	}
}

/// @brief 現在の状態を履歴に保存する
void UIEditor::SaveHistoryState()
{
	// 現在のリストの状態をJSON化してUndoスタックに積む
	undoStack_.push_back(ToJsonData(uiElements_));

	// 新しい操作をしたのでRedoスタックはクリアする
	redoStack_.clear();

	// 履歴の上限を超えたら古いものから削除
	if (undoStack_.size() > 50)
	{
		undoStack_.erase(undoStack_.begin());
	}
}

/// @brief 元に戻す
void UIEditor::Undo()
{
	if (undoStack_.empty()) return;

	// 現在の状態をRedoスタックに退避
	redoStack_.push_back(ToJsonData(uiElements_));

	// Undoスタックの最新のデータを復元
	uiElements_ = FromJsonData(undoStack_.back(), loadedTextures_);
	undoStack_.pop_back();

	// 選択インデックスが範囲外にならないよう調整
	if (selectedElementIndex_ >= uiElements_.size()) selectedElementIndex_ = -1;
}

/// @brief やり直し
void UIEditor::Redo()
{
	if (redoStack_.empty()) return;

	// 現在の状態をUndoスタックに退避
	undoStack_.push_back(ToJsonData(uiElements_));

	// Redoスタックの最新のデータを復元
	uiElements_ = FromJsonData(redoStack_.back(), loadedTextures_);
	redoStack_.pop_back();

	if (selectedElementIndex_ >= uiElements_.size()) selectedElementIndex_ = -1;
}