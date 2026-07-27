#include "LightEditor.h"
#include "LightSerializer/LightSerializer.h"

/// @brief UIを描画する
void LightEditor::DrawUI()
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

/// @brief 平行光源を取得する
/// @param name 
/// @return 
LightDirectional* LightEditor::GetDirectionalLight(const std::string& name) const
{
	for (auto& elem : lightElements_)
	{
		if (elem.name == name && elem.lightType == Engine::Light::Type::Directional)
		{
			return static_cast<LightDirectional*>(elem.light.get());
		}
	}

	return nullptr;
}

/// @brief 点光源を取得する
/// @param name 
/// @return 
LightPoint* LightEditor::GetPointLight(const std::string& name) const
{
	for (auto& elem : lightElements_)
	{
		if (elem.name == name && elem.lightType == Engine::Light::Type::Point)
		{
			return static_cast<LightPoint*>(elem.light.get());
		}
	}
	return nullptr;
}

/// @brief スポットライトを取得する
/// @param name 
/// @return 
LightSpot* LightEditor::GetSpotLight(const std::string& name) const
{
	for (auto& elem : lightElements_)
	{
		if (elem.name == name && elem.lightType == Engine::Light::Type::Spot)
		{
			return static_cast<LightSpot*>(elem.light.get());
		}
	}
	return nullptr;
}

/// @brief ヒエラルキーウィンドウ描画
void LightEditor::DrawHierarchyWindow()
{
#ifdef DEVELOPMENT

	if (ImGui::Begin("ライト - ヒエラルキー"))
	{
		for (int i = 0; i < lightElements_.size(); ++i)
		{
			bool isSelected = (selectedElementIndex_ == i);
			if (ImGui::Selectable(lightElements_[i].name.c_str(), isSelected))
			{
				selectedElementIndex_ = i;
			}
		}
	}
	ImGui::End();

#endif
}

/// @brief インスペクターウィンドウ描画
void LightEditor::DrawInspectorWindow()
{
#ifdef DEVELOPMENT

	if (ImGui::Begin("ライト - インスペクター"))
	{
		if (selectedElementIndex_ >= 0 && selectedElementIndex_ < lightElements_.size())
		{
			auto& selectedData = lightElements_[selectedElementIndex_];

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


			if (selectedData.lightType == Engine::Light::Type::Directional)
			{
				auto* param = static_cast<LightDirectional*>(selectedData.light.get())->param_;

				// 方向の編集
				ImGui::DragFloat3("方向 (Direction)", &param->direction.x, 0.01f, -1.0f, 1.0f);
				if (ImGui::IsItemActivated()) SaveHistoryState();

				// サイズの編集
				ImGui::DragFloat2("サイズ (Size)", &param->size.x, 0.1f, 0.0f, 1000.0f);
				if (ImGui::IsItemActivated()) SaveHistoryState();

				// 最小深度値の編集
				ImGui::DragFloat("最小深度値 (Min Depth)", &param->minDepth, 0.1f, 0.0f, 1000.0f);
				if (ImGui::IsItemActivated()) SaveHistoryState();

				// 最大深度値の編集
				ImGui::DragFloat("最大深度値 (Max Depth)", &param->maxDepth, 0.1f, 0.0f, 1000.0f);
				if (ImGui::IsItemActivated()) SaveHistoryState();

				// 輝度の編集
				ImGui::DragFloat("輝度 (Intensity)", &param->intensity, 0.1f, 0.0f, 1000.0f);
				if (ImGui::IsItemActivated()) SaveHistoryState();

				// 色の編集
				ImGui::ColorEdit3("色 (Color)", &param->color.x);
				if (ImGui::IsItemActivated()) SaveHistoryState();
			}
			else if (selectedData.lightType == Engine::Light::Type::Point)
			{
				auto* param = static_cast<LightPoint*>(selectedData.light.get())->param_;

				// 位置の編集
				ImGui::DragFloat3("位置 (Position)", &param->position.x, 1.0f, 0.0f, 100000.0f);
				if (ImGui::IsItemActivated()) SaveHistoryState();

				// 輝度の編集
				ImGui::DragFloat("輝度 (Intensity)", &param->intensity, 0.1f, 0.0f, 1000.0f);
				if (ImGui::IsItemActivated()) SaveHistoryState();

				// 色の編集
				ImGui::ColorEdit3("色 (Color)", &param->color.x);
				if (ImGui::IsItemActivated()) SaveHistoryState();

				// ライトの届く距離の編集
				ImGui::DragFloat("ライトの届く距離 (Radius)", &param->radius, 0.1f, 0.0f, 100000.0f);
				if (ImGui::IsItemActivated()) SaveHistoryState();

				// 減衰率の編集
				ImGui::DragFloat("減衰率 (Decay)", &param->decay, 0.01f, 0.0f, 1.0f);
				if (ImGui::IsItemActivated()) SaveHistoryState();
			}
			else if (selectedData.lightType == Engine::Light::Type::Spot)
			{
				auto* param = static_cast<LightSpot*>(selectedData.light.get())->param_;

				// 位置の編集
				ImGui::DragFloat3("位置 (Position)", &param->position.x, 1.0f, 0.0f, 100000.0f);
				if (ImGui::IsItemActivated()) SaveHistoryState();

				// 方向の編集
				ImGui::DragFloat3("方向 (Direction)", &param->direction.x, 0.01f, -1.0f, 1.0f);
				if (ImGui::IsItemActivated()) SaveHistoryState();

				// 輝度の編集
				ImGui::DragFloat("輝度 (Intensity)", &param->intensity, 0.1f, 0.0f, 1000.0f);
				if (ImGui::IsItemActivated()) SaveHistoryState();

				// 色の編集
				ImGui::ColorEdit3("色 (Color)", &param->color.x);
				if (ImGui::IsItemActivated()) SaveHistoryState();

				// ライトの届く距離の編集
				ImGui::DragFloat("ライトの届く距離 (Distance)", &param->distance, 0.1f, 0.0f, 1000.0f);
				if (ImGui::IsItemActivated()) SaveHistoryState();

				// 減衰率の編集
				ImGui::DragFloat("減衰率 (Decay)", &param->decay, 0.01f, 0.0f, 1.0f);
				if (ImGui::IsItemActivated()) SaveHistoryState();

				// 光の当たる角度の編集
				ImGui::DragFloat("光の当たる角度 (Cos Angle)", &param->cosAngle, 0.01f, 0.0f, 1.0f);
				if (ImGui::IsItemActivated()) SaveHistoryState();

				// フォールオフ開始角度の編集
				ImGui::DragFloat("フォールオフ開始角度 (Cos Falloff Start)", &param->cosFalloffStart, 0.01f, param->cosAngle, 1.0f);
				if (ImGui::IsItemActivated()) SaveHistoryState();
			}
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
void LightEditor::DrawAssetsWindow()
{
#ifdef DEVELOPMENT

	if (ImGui::Begin("ライト - アセット"))
	{
		if (ImGui::Button("新規ファイル作成"))
		{
			// ポップアップを開く前に入力欄にデフォルト名を入れておく
			strcpy_s(inputFilename_, "light_new");
			ImGui::OpenPopup("新規ファイル作成ポップアップ");
		}

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
					lightElements_.clear();
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

		if (isFileOpen_)
		{
			ImGui::SameLine();
			ImGui::Text("編集中: %s.json", currentFileName_.c_str());
			ImGui::Separator();

			// 保存ボタン
			if (ImGui::Button("保存"))
				Save();

			ImGui::SameLine();

			static char newLightName[64] = "New Light";
			static int newLightType = 0; // 0: None, 1: Directional, 2: Point, 3: Spot

			// 新規ライト追加ボタン
			if (ImGui::Button("新規ライト追加"))
			{
				// ポップアップを開く前に初期値をセット
				strcpy_s(newLightName, "New Light");
				ImGui::OpenPopup("新規ライト作成");
			}

			// 新規ライト作成のポップアップ
			if (ImGui::BeginPopupModal("新規ライト作成", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				// 名前の入力
				ImGui::InputText("名前", newLightName, IM_ARRAYSIZE(newLightName));

				// ライトの種類の選択
				const char* lightTypes[] = { "None", "Directional", "Point", "Spot" };
				ImGui::Combo("ライトの種類", &newLightType, lightTypes, IM_ARRAYSIZE(lightTypes));

				ImGui::Separator();

				// 作成ボタン
				if (newLightType != static_cast<int>(Engine::Light::Type::None) && ImGui::Button("作成", ImVec2(120, 0)))
				{
					// 新規作成時の状態を履歴に保存
					SaveHistoryState();

					LightElementData newData;
					newData.name = GetUniqueName(newLightName);
					newData.lightType = static_cast<Engine::Light::Type>(newLightType);

					// ライトの種類に応じてライトを生成
					if (newData.lightType == Engine::Light::Type::Directional)
					{
						newData.light = std::make_unique<LightDirectional>(newData.name);
					} else if (newData.lightType == Engine::Light::Type::Point)
					{
						newData.light = std::make_unique<LightPoint>(newData.name);
					} else if (newData.lightType == Engine::Light::Type::Spot)
					{
						newData.light = std::make_unique<LightSpot>(newData.name);
					}

					// 新規作成したライトをリストに追加
					lightElements_.push_back(std::move(newData));

					// 選択状態を新規作成したものに合わせる
					selectedElementIndex_ = static_cast<int>(lightElements_.size()) - 1;

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


		if (std::filesystem::exists(kLightDir))
		{
			// ウィンドウの右端の座標を取得（折り返しの計算用）
			float windowVisibleX = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
			ImGuiStyle& style = ImGui::GetStyle();

			// ディレクトリ内のファイルを走査
			for (const auto& entry : std::filesystem::directory_iterator(kLightDir))
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
						lightElements_ = FromJson(path);

						// ファイル名から拡張子を除いた名前を取得して、保存用の入力欄にセット
						std::string nameWithoutExt = entry.path().stem().string();
						strcpy_s(inputFilename_, nameWithoutExt.c_str());

						// 現在開いているファイル名を更新
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
			ImGui::TextDisabled("ディレクトリが見つかりません: %s", kLightDir.c_str());
		}
	}
	ImGui::End();

#endif
}

/// @brief UI要素の名前が重複しないようにユニークな名前を生成する
/// @param baseName 
/// @param ignoreIndex 
/// @return 
std::string LightEditor::GetUniqueName(const std::string& baseName, int ignoreIndex) const
{
	std::string currentName = baseName;
	int count = 1;

	while (true)
	{
		bool isDuplicate = false;

		for (int i = 0; i < lightElements_.size(); ++i)
		{
			// 自分自身は重複チェックから除外する
			if (i == ignoreIndex) continue;

			// 名前が重複しているかチェック
			if (lightElements_[i].name == currentName)
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

/// @brief ファイルに保存する
void LightEditor::Save()
{
	// ファイル名が空の場合は保存しない
	if (currentFileName_.empty()) return;

	// ディレクトリが存在しない場合は作成する
	if (!std::filesystem::exists(kLightDir))
		std::filesystem::create_directories(kLightDir);

	// ファイル名に拡張子が含まれていない場合は .json を付与する
	std::string filenameStr = currentFileName_;
	if (filenameStr.find(".json") == std::string::npos)
		filenameStr += ".json";

	std::string path = kLightDir + filenameStr;
	ToJson(path, lightElements_);
}

/// @brief 選択中のUI要素を削除する
void LightEditor::DeleteSelectedElement()
{
	if (selectedElementIndex_ >= 0 && selectedElementIndex_ < lightElements_.size())
	{
		SaveHistoryState();
		lightElements_.erase(lightElements_.begin() + selectedElementIndex_);
		selectedElementIndex_ = -1; // 選択解除
	}

}

/// @brief 現在の状態を履歴に保存する
void LightEditor::SaveHistoryState()
{
	// 現在のリストの状態をJSON化してUndoスタックに積む
	undoStack_.push_back(ToJsonData(lightElements_));

	// 新しい操作をしたのでRedoスタックはクリアする
	redoStack_.clear();

	// 履歴の上限を超えたら古いものから削除
	if (undoStack_.size() > 50)
	{
		undoStack_.erase(undoStack_.begin());
	}
}

/// @brief 元に戻す
void LightEditor::Undo()
{
	if (undoStack_.empty()) return;

	// 現在の状態をRedoスタックに退避
	redoStack_.push_back(ToJsonData(lightElements_));

	// Undoスタックの最新のデータを復元
	lightElements_ = FromJsonData(undoStack_.back());
	undoStack_.pop_back();

	// 選択インデックスが範囲外にならないよう調整
	if (selectedElementIndex_ >= lightElements_.size()) selectedElementIndex_ = -1;
}

/// @brief やり直し
void LightEditor::Redo()
{
	if (redoStack_.empty()) return;

	// 現在の状態をUndoスタックに退避
	undoStack_.push_back(ToJsonData(lightElements_));

	// Redoスタックの最新のデータを復元
	lightElements_ = FromJsonData(redoStack_.back());
	redoStack_.pop_back();

	if (selectedElementIndex_ >= lightElements_.size()) selectedElementIndex_ = -1;
}