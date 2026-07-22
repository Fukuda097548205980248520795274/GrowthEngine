#include "CutsceneEditor.h"
#include "GrowthEngine.h"
#include "CutsceneSerializer/CutsceneSerializer.h"
#include "Entity/Character/Character.h"

/// @brief 初期化処理
/// @param cutsceneCamera 
/// @param mainCamera 
void CutsceneEditor::Initialize(MainCamera3D* cutsceneCamera, MainCamera3D* mainCamera)
{
	cutsceneCamera_ = cutsceneCamera;
	mainCamera_ = mainCamera;

	// 初期データの設定
	editingData_.name = "NewCutscene";
	editingData_.duration = 5.0f;

	// ファイルリストを更新
	RefreshFileList();
}

/// @brief エディタを開くか閉じるかを設定する
/// @param active 
void CutsceneEditor::SetActive(bool active)
{
	isActive_ = active;
}

/// @brief エディタを開くときの処理
void CutsceneEditor::OnEnableEditor()
{
	if (cutsceneCamera_)
	{
		// カットシーン用カメラに切り替え
		cutsceneCamera_->Switch();

		// メインカメラの位置・回転・画角をカットシーン用カメラにコピー
		if (mainCamera_)
		{
			cutsceneCamera_->param_->transform.translate = mainCamera_->param_->transform.translate;
			cutsceneCamera_->param_->transform.rotate = mainCamera_->param_->transform.rotate;
			cutsceneCamera_->param_->setting.fov = mainCamera_->param_->setting.fov;
		}
	}
}

/// @brief エディタを閉じるときの処理
void CutsceneEditor::OnDisableEditor()
{
	if (mainCamera_)
	{
		// メインカメラに戻す
		mainCamera_->Switch();
	}
}

/// @brief 更新処理
/// @param dt 
void CutsceneEditor::Update(float dt)
{
	// エディタのアクティブ状態の変化を検知
	if (isActive_ && !wasActive_) 
	{
		OnEnableEditor();
	}
	else if (!isActive_ && wasActive_) 
	{
		// エディタを閉じるときの処理
		OnDisableEditor();
	}

	// 前フレームのアクティブ状態を更新
	wasActive_ = isActive_;

	if (!isActive_) return;

	// スペースキーで再生・停止を切り替え
	if (!ImGui::GetIO().WantTextInput && ImGui::IsKeyPressed(ImGuiKey_Space))
	{
		isPlaying_ = !isPlaying_;
	}

	// 左右キーで1フレーム分の時間を進める・戻す
	if (!ImGui::GetIO().WantTextInput && ImGui::IsKeyPressed(ImGuiKey_LeftArrow))
	{
		currentTime_ -= 1.0f / 60.0f; // 1フレーム分戻す
		if (currentTime_ < 0.0f) currentTime_ = 0.0f;
	}

	// 右キーで1フレーム分の時間を進める
	if (!ImGui::GetIO().WantTextInput && ImGui::IsKeyPressed(ImGuiKey_RightArrow))
	{
		currentTime_ += 1.0f / 60.0f; // 1フレーム分進める
		if (currentTime_ > editingData_.duration) currentTime_ = editingData_.duration;
	}

	// プレビュー再生中の処理
	if (isPlaying_)
	{
		currentTime_ += dt;

		// 再生時間が総時間を超えた場合は停止
		if (currentTime_ >= editingData_.duration)
		{
			currentTime_ = editingData_.duration;
			isPlaying_ = false;
		}

		// 戻り値の型を CameraSample に変更
		CameraSample sample = SampleCutscene(editingData_, currentTime_);

		// カットシーン用カメラのパラメータを更新
		if (!editingData_.positionKeys.empty()) cutsceneCamera_->param_->transform.translate = sample.position;
		if (!editingData_.rotationKeys.empty()) cutsceneCamera_->param_->transform.rotate = sample.rotation;
		if (!editingData_.fovKeys.empty()) cutsceneCamera_->param_->setting.fov = sample.fov;

		// キャラクターのキーフレームをサンプリングして座標を更新
		for (const auto& track : editingData_.characterTracks)
		{
			Character* targetChar = nullptr;
			for (auto* character : Character::GetCharacters())
			{
				if (character && character->GetEditorName() == track.characterName)
				{
					targetChar = character;
					break;
				}
			}

			if (targetChar)
			{
				CharacterSample charSample = SampleCharacterTrack(track, currentTime_);
				if (!track.positionKeys.empty()) targetChar->SetPosition(charSample.position);
			}
		}
	}
	else
	{
		// 自由カメラの操作
		UpdateFreeCamera(dt);
	}
}

/// @brief 自由カメラの更新
/// @param dt 
void CutsceneEditor::UpdateFreeCamera(float dt)
{
	if (!cutsceneCamera_) return;

	
}

/// @brief ファイルリストを更新する
void CutsceneEditor::RefreshFileList()
{
	fileList_.clear();

	// ディレクトリが存在しない場合は作成する
	if (!std::filesystem::exists(kCutsceneDir))
	{
		std::filesystem::create_directories(kCutsceneDir);
	}

	// ディレクトリ内の .json ファイルを列挙
	for (const auto& entry : std::filesystem::directory_iterator(kCutsceneDir))
	{
		// 拡張子が .json のファイルのみをリストに追加
		if (entry.path().extension() == ".json")
			fileList_.push_back(entry.path().filename().string());
	}

	if (selectedFileIndex_ >= fileList_.size())
	{
		selectedFileIndex_ = 0;
	}
}

/// @brief UIを描画する
void CutsceneEditor::DrawUI()
{
#ifdef DEVELOPMENT
	DrawEditorUI();
	DrawTimelineUI();
#endif
}

/// @brief エディタUIを描画する
void CutsceneEditor::DrawEditorUI()
{
	if (!isActive_) return;

	// ImGuiウィンドウの描画
	if (ImGui::Begin("カットシーンエディタ", &isActive_))
	{
		ImGui::Text("--- ファイル管理 ---");

		if (ImGui::Button("リフレッシュ"))
			RefreshFileList();

		// ファイルリストが存在する場合、コンボボックスとロードボタンを表示
		if (!fileList_.empty())
		{
			std::vector<const char*> items;
			for (const auto& file : fileList_)
			{
				items.push_back(file.c_str());
			}

			ImGui::Combo("ファイル", &selectedFileIndex_, items.data(), static_cast<int>(items.size()));
			ImGui::SameLine();
			if (ImGui::Button("選択"))
			{
				currentFilePath_ = kCutsceneDir + fileList_[selectedFileIndex_];
				editingData_ = LoadCutscene(currentFilePath_);
				currentTime_ = 0.0f;
			}
		}

		ImGui::Separator();

		// 新規保存とデータリセットの処理
		static char saveFilename[128] = "cutscene_new";
		ImGui::InputText("保存するファイル名", saveFilename, IM_ARRAYSIZE(saveFilename));

		if (ImGui::Button("保存"))
		{
			std::string filenameStr(saveFilename);
			// 拡張子が含まれていなければ追加
			if (filenameStr.find(".json") == std::string::npos) {
				filenameStr += ".json";
			}

			std::string path = kCutsceneDir + filenameStr;
			SaveCutscene(path, editingData_);

			// 保存後にリストを更新
			RefreshFileList();
		}

		ImGui::SameLine();

		// 編集データを空の新規状態に戻すボタン
		if (ImGui::Button("新規データ"))
		{
			editingData_ = KeyframeCutsceneData();
			editingData_.name = "NewCutscene";
			editingData_.duration = 5.0f;
			currentTime_ = 0.0f;
			strcpy_s(saveFilename, "cutscene_new");
		}


		ImGui::Separator();
		ImGui::Text("--- カットシーン編集 ---");

		// カットシーン基本情報
		static char sceneName[64] = "";
		strcpy_s(sceneName, editingData_.name.c_str());
		if (ImGui::InputText("カットシーン名", sceneName, IM_ARRAYSIZE(sceneName)))
		{
			editingData_.name = sceneName;
		}
		ImGui::DragFloat("総時間 (秒)", &editingData_.duration, 0.1f, 0.1f, 60.0f);

		ImGui::Separator();

		// キーフレーム操作
		ImGui::Text("キーフレーム管理");
		if (cutsceneCamera_)
		{
			auto& trans = cutsceneCamera_->param_->transform;
			auto& setting = cutsceneCamera_->param_->setting;

			ImGui::DragFloat3("カメラ 位置", &trans.translate.x, 0.1f);
			if (ImGui::Button("今の時間に位置キーを追加"))AddKeyframeToTrack(editingData_.positionKeys, currentTime_, trans.translate);

			ImGui::Separator();

			ImGui::DragFloat3("カメラ 回転", &trans.rotate.x, 0.1f);
			if (ImGui::Button("今の時間に回転キーを追加"))AddKeyframeToTrack(editingData_.rotationKeys, currentTime_, trans.rotate);

			ImGui::Separator();

			ImGui::DragFloat("カメラ 画角", &setting.fov, 0.01f);
			if (ImGui::Button("今の時間に画角キーを追加"))AddKeyframeToTrack(editingData_.fovKeys, currentTime_, setting.fov);
		}


		ImGui::Separator();
		ImGui::Text("--- キャラクター制御 ---");

		// 現在アクティブなキャラクターの名前リストを取得
		std::vector<std::string> activeCharNames;
		int selectedCharIndex = -1;

		int idx = 0;
		for (auto* character : Character::GetCharacters())
		{
			if (character)
			{
				std::string name = character->GetEditorName();
				activeCharNames.push_back(name);
				if (name == selectedCharacterName_)
				{
					selectedCharIndex = idx;
				}
				idx++;
			}
		}

		// キャラクター選択コンボボックス
		if (!activeCharNames.empty())
		{
			std::vector<const char*> charItems;
			for (const auto& name : activeCharNames)
			{
				charItems.push_back(name.c_str());
			}

			if (ImGui::Combo("動かすキャラ", &selectedCharIndex, charItems.data(), static_cast<int>(charItems.size())))
			{
				selectedCharacterName_ = activeCharNames[selectedCharIndex];
			}
		}
		else
		{
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "シーン上にキャラクターが存在しません。");
		}

		// 選択されたキャラクターに対する操作
		Character* selectedChar = nullptr;
		for (auto* character : Character::GetCharacters())
		{
			if (character && character->GetEditorName() == selectedCharacterName_)
			{
				selectedChar = character;
				break;
			}
		}

		if (selectedChar)
		{
			WorldTransform3D* transform = selectedChar->GetWorldTransform();

			ImGui::Text("選択中: %s", selectedCharacterName_.c_str());
			ImGui::DragFloat3("キャラ位置", &transform->translate_.x, 0.1f);

			if (ImGui::Button("今の時間にキャラ位置キーを追加"))
			{
				auto it = std::find_if(editingData_.characterTracks.begin(), editingData_.characterTracks.end(),
					[this](const CharacterCutsceneTrack& t) { return t.characterName == selectedCharacterName_; });

				if(it != editingData_.characterTracks.end())
					AddKeyframeToTrack(it->positionKeys, currentTime_, transform->translate_);
			}

			if (ImGui::Button("このキャラの全トラックを削除"))
			{
				editingData_.characterTracks.erase(
					std::remove_if(editingData_.characterTracks.begin(), editingData_.characterTracks.end(),
						[this](const CharacterCutsceneTrack& t) { return t.characterName == selectedCharacterName_; }),
					editingData_.characterTracks.end()
				);
			}
		}


		ImGui::Separator();
		ImGui::Text("--- 選択中のキーフレーム設定 ---");

		// 選択中のキーフレームがある場合
		if (selectedTrack_ != DraggingTrack::None && selectedKeyIndex_ != -1)
		{
			InterpolationType* targetInterp = nullptr;
			const char* trackName = "";

			// トラックに応じて対象のキーフレームの補間タイプを取得
			if (selectedTrack_ == DraggingTrack::Position && selectedKeyIndex_ < editingData_.positionKeys.size())
			{
				targetInterp = &editingData_.positionKeys[selectedKeyIndex_].interpType;
				trackName = "カメラ位置";
			}
			else if (selectedTrack_ == DraggingTrack::Rotation && selectedKeyIndex_ < editingData_.rotationKeys.size())
			{
				targetInterp = &editingData_.rotationKeys[selectedKeyIndex_].interpType;
				trackName = "カメラ回転";
			}
			else if (selectedTrack_ == DraggingTrack::FOV && selectedKeyIndex_ < editingData_.fovKeys.size())
			{
				targetInterp = &editingData_.fovKeys[selectedKeyIndex_].interpType;
				trackName = "カメラ画角";
			}

			if (targetInterp)
			{
				ImGui::Text("トラック: %s [インデックス: %d]", trackName, selectedKeyIndex_);

				// 補間タイプの名前定義
				const char* interpolationNames[] = { "Linear", "EaseIn", "EaseOut", "EaseInOut", "CatmullRom" };
				int currentInterpInt = static_cast<int>(*targetInterp);

				// 補間タイプのコンボボックス
				if (ImGui::Combo("補間タイプ", &currentInterpInt, interpolationNames, IM_ARRAYSIZE(interpolationNames)))
				{
					// 変更をデータに反映
					*targetInterp = static_cast<InterpolationType>(currentInterpInt);
				}

				if (ImGui::Button("選択中のキーを削除"))
				{
					auto removeKey = [&](auto& keys)
						{
							if (selectedKeyIndex_ < keys.size())
							{
								keys.erase(keys.begin() + selectedKeyIndex_);
							}
						};

					if (selectedTrack_ == DraggingTrack::Position) removeKey(editingData_.positionKeys);
					else if (selectedTrack_ == DraggingTrack::Rotation) removeKey(editingData_.rotationKeys);
					else if (selectedTrack_ == DraggingTrack::FOV) removeKey(editingData_.fovKeys);

					// 削除後は選択をクリア
					selectedTrack_ = DraggingTrack::None;
					selectedKeyIndex_ = -1;
				}
			}
			else
			{
				// データが見つからない場合（削除された可能性など）は選択をクリア
				selectedTrack_ = DraggingTrack::None;
				selectedKeyIndex_ = -1;
			}
		}
		else
		{
			ImGui::Text("タイムラインでキーフレームを選択してください。");
		}
	}
	ImGui::End();
}

/// @brief タイムラインUIを描画する
void CutsceneEditor::DrawTimelineUI()
{
	if (!isActive_) return;

	if (ImGui::Begin("タイムライン", &isActive_))
	{
		// 再生/一時停止ボタン
		if (ImGui::Button(isPlaying_ ? "一時停止 (Space)" : "再生 (Space)"))
		{
			isPlaying_ = !isPlaying_;
		}

		ImGui::SameLine();

		// 停止ボタン（時間を0に戻す）
		if (ImGui::Button("停止"))
		{
			isPlaying_ = false;
			currentTime_ = 0.0f;

			// カメラの姿勢を0秒の状態に更新
			CameraSample sample = SampleCutscene(editingData_, currentTime_);
			if (!editingData_.positionKeys.empty()) cutsceneCamera_->param_->transform.translate = sample.position;
			if (!editingData_.rotationKeys.empty()) cutsceneCamera_->param_->transform.rotate = sample.rotation;
			if (!editingData_.fovKeys.empty()) cutsceneCamera_->param_->setting.fov = sample.fov;
		}

		// スナップ機能の設定UI
		ImGui::Checkbox("スナップ有効", &snapEnabled_);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(100.0f);
		ImGui::DragFloat("スナップ間隔", &snapStep_, 0.01f, 0.01f, 1.0f, "%.2f 秒");



		ImVec2 canvasPos = ImGui::GetCursorScreenPos();
		ImVec2 canvasSize = ImVec2(ImGui::GetContentRegionAvail().x, 60.0f); // 高さを確保
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		// 背景の描画
		drawList->AddRectFilled(canvasPos, ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y), IM_COL32(40, 40, 40, 255));
		drawList->AddRect(canvasPos, ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y), IM_COL32(100, 100, 100, 255));


		// 再生中の時間を示す縦線の描画
		if (editingData_.duration > 0.0f)
		{
			float trackHeight = canvasSize.y / (3 + editingData_.characterTracks.size() * 2);
			float posY = canvasPos.y + trackHeight * 0.5f;

			// キャラクタートラックの描画
			float currentTrackY = canvasPos.y + trackHeight * 3.5f; // カメラ3行の下から開始
			for (const auto& track : editingData_.characterTracks)
			{
				// 位置キーフレームの描画
				for (const auto& kf : track.positionKeys)
				{
					float kfX = canvasPos.x + ((kf.time / editingData_.duration) * canvasSize.x);
					drawList->AddCircleFilled(ImVec2(kfX, currentTrackY), 4.0f, IM_COL32(200, 50, 200, 255)); // 紫色など
				}
				currentTrackY += trackHeight;
			}
		}


		// タイムライン全体とのインタラクション領域を生成
		ImGui::InvisibleButton("TimelineInteraction", canvasSize);
		bool isCanvasHovered = ImGui::IsItemHovered();
		bool isCanvasActive = ImGui::IsItemActive();
		bool isCanvasClicked = ImGui::IsItemClicked();

		ImVec2 mousePos = ImGui::GetIO().MousePos;
		float mouseX = mousePos.x - canvasPos.x;

		if (editingData_.duration > 0.0f)
		{
			// 各トラックのY座標を計算
			float posY = canvasPos.y + canvasSize.y * 0.25f;
			float rotY = canvasPos.y + canvasSize.y * 0.5f;
			float fovY = canvasPos.y + canvasSize.y * 0.75f;

			// キーフレームのインタラクション範囲を定義
			float interactRadiusSq = 8.0f * 8.0f; // 掴める範囲(半径8px)
			bool isHoveringAnyKey = false;
			DraggingTrack hoveredTrack = DraggingTrack::None;
			int hoveredIndex = -1;
			std::string hoveredCharName = "";

			// キーフレームのホバー判定を行うラムダ関数
			auto checkHover = [&](const auto& keys, float trackY, DraggingTrack type, const std::string& charName = "")
				{
					for (int i = 0; i < static_cast<int>(keys.size()); ++i) 
					{
						float kfX = canvasPos.x + ((keys[i].time / editingData_.duration) * canvasSize.x);

						// マウスカーソルとキーフレームマーカーの距離(二乗)を比較
						float distSq = (mousePos.x - kfX) * (mousePos.x - kfX) + (mousePos.y - trackY) * (mousePos.y - trackY);
						if (distSq <= interactRadiusSq)
						{
							isHoveringAnyKey = true;
							hoveredTrack = type;
							hoveredIndex = i;
							hoveredCharName = charName;
							break;
						}
					}
				};

			// ドラッグ中でなければホバー判定を行う
			if (isCanvasHovered && draggingTrack_ == DraggingTrack::None)
			{
				checkHover(editingData_.positionKeys, posY, DraggingTrack::Position);
				if (!isHoveringAnyKey) checkHover(editingData_.rotationKeys, rotY, DraggingTrack::Rotation);
				if (!isHoveringAnyKey) checkHover(editingData_.fovKeys, fovY, DraggingTrack::FOV);


				// キャラクタートラックのホバー判定
				float trackHeight = canvasSize.y / (3 + editingData_.characterTracks.size() * 2);
				float currentTrackY = canvasPos.y + trackHeight * 3.5f;

				// キャラクタートラックのキーフレームのホバー判定
				for (const auto& track : editingData_.characterTracks)
				{
					if (!isHoveringAnyKey)
						checkHover(track.positionKeys, currentTrackY, DraggingTrack::CharacterPosition, track.characterName);

					currentTrackY += trackHeight;
				}
			}

			// クリックした瞬間の処理
			if (isCanvasClicked)
			{
				if (isHoveringAnyKey)
				{
					// キーフレームを選択状態にする
					selectedTrack_ = hoveredTrack;
					selectedKeyIndex_ = hoveredIndex;

					// ドラッグ状態に移行 (既存コードの処理をここに移動)
					draggingTrack_ = hoveredTrack;
					draggingKeyIndex_ = hoveredIndex;
				}
				else
				{
					// 何もない場所をクリックしたら選択を解除
					selectedTrack_ = DraggingTrack::None;
					selectedKeyIndex_ = -1;
				}
			}

			// クリックした瞬間にキーフレームの上にいたらドラッグ状態に移行
			if (isCanvasClicked && isHoveringAnyKey) 
			{
				draggingTrack_ = hoveredTrack;
				draggingKeyIndex_ = hoveredIndex;
				draggingCharacterName_ = hoveredCharName;
			}

			// ドラッグ中の処理
			if (isCanvasActive)
			{
				float newRatio = std::clamp(mouseX / canvasSize.x, 0.0f, 1.0f);
				float newTime = newRatio * editingData_.duration;

				// スナップ機能が有効な場合は、スナップ間隔に丸める
				if (snapEnabled_ && snapStep_ > 0.0f)
				{
					// 最も近いスナップ間隔に丸める
					newTime = std::round(newTime / snapStep_) * snapStep_;

					// 総時間を超えないようにクランプ
					newTime = std::clamp(newTime, 0.0f, editingData_.duration);
				}


				if (draggingTrack_ != DraggingTrack::None)
				{
					// キーフレームを掴んでいる場合、対象のキーフレームの時間を更新
					if (draggingTrack_ == DraggingTrack::Position) editingData_.positionKeys[draggingKeyIndex_].time = newTime;
					else if (draggingTrack_ == DraggingTrack::Rotation) editingData_.rotationKeys[draggingKeyIndex_].time = newTime;
					else if (draggingTrack_ == DraggingTrack::FOV) editingData_.fovKeys[draggingKeyIndex_].time = newTime;
					else if (draggingTrack_ == DraggingTrack::CharacterPosition)
					{
						// 対象のキャラクターを探してキーフレーム時間を更新
						auto it = std::find_if(editingData_.characterTracks.begin(), editingData_.characterTracks.end(),
							[&](const CharacterCutsceneTrack& t) { return t.characterName == draggingCharacterName_; });
						if (it != editingData_.characterTracks.end())
							it->positionKeys[draggingKeyIndex_].time = newTime;
					}

					// 移動先の時間をシークバーにも反映して即座にプレビュー
					currentTime_ = newTime;
				}
				else
				{
					// キーフレームを掴んでいない（何もない場所をクリックした）場合は通常通り再生位置をシーク
					currentTime_ = newTime;
				}

				// カメラパラメータの更新
				isPlaying_ = false;
				CameraSample sample = SampleCutscene(editingData_, currentTime_);
				if (!editingData_.positionKeys.empty()) cutsceneCamera_->param_->transform.translate = sample.position;
				if (!editingData_.rotationKeys.empty()) cutsceneCamera_->param_->transform.rotate = sample.rotation;
				if (!editingData_.fovKeys.empty()) cutsceneCamera_->param_->setting.fov = sample.fov;
			}

			// ドラッグを終了した瞬間に、キーフレームの時間をソートして順序を正す
			if (ImGui::IsMouseReleased(0) && draggingTrack_ != DraggingTrack::None)
			{
				// 時間を変更したため、順序が狂わないようにソートし直す
				auto sortByTime = [](const auto& a, const auto& b) { return a.time < b.time; };

				if (draggingTrack_ == DraggingTrack::Position)
					std::sort(editingData_.positionKeys.begin(), editingData_.positionKeys.end(), sortByTime);
				else if (draggingTrack_ == DraggingTrack::Rotation)
					std::sort(editingData_.rotationKeys.begin(), editingData_.rotationKeys.end(), sortByTime);
				else if (draggingTrack_ == DraggingTrack::FOV)
					std::sort(editingData_.fovKeys.begin(), editingData_.fovKeys.end(), sortByTime);
				else if (draggingTrack_ == DraggingTrack::CharacterPosition)
				{
					// キャラクタートラックのキーフレームもソート
					auto it = std::find_if(editingData_.characterTracks.begin(), editingData_.characterTracks.end(),
						[&](const CharacterCutsceneTrack& t) { return t.characterName == draggingCharacterName_; });
					if (it != editingData_.characterTracks.end())
						std::sort(it->positionKeys.begin(), it->positionKeys.end(), sortByTime);
				}

				// ドラッグ状態を解除
				draggingTrack_ = DraggingTrack::None;
				draggingKeyIndex_ = -1;
			}


			// 選択中のキーフレームを強調表示 (白い円枠を描画)
			if (selectedTrack_ != DraggingTrack::None && selectedKeyIndex_ != -1)
			{
				float kfX = 0.0f;
				float kfY = 0.0f;
				if (selectedTrack_ == DraggingTrack::Position && selectedKeyIndex_ < editingData_.positionKeys.size())
				{
					kfX = canvasPos.x + ((editingData_.positionKeys[selectedKeyIndex_].time / editingData_.duration) * canvasSize.x);
					kfY = posY;
				}
				else if (selectedTrack_ == DraggingTrack::Rotation && selectedKeyIndex_ < editingData_.rotationKeys.size())
				{
					kfX = canvasPos.x + ((editingData_.rotationKeys[selectedKeyIndex_].time / editingData_.duration) * canvasSize.x);
					kfY = rotY;
				}
				else if (selectedTrack_ == DraggingTrack::FOV && selectedKeyIndex_ < editingData_.fovKeys.size())
				{
					kfX = canvasPos.x + ((editingData_.fovKeys[selectedKeyIndex_].time / editingData_.duration) * canvasSize.x);
					kfY = fovY;
				}

				// 強調表示の円を描画 (少し大きめの白い円枠)
				drawList->AddCircle(ImVec2(kfX, kfY), 6.0f, IM_COL32(255, 255, 255, 255), 12, 2.0f);
			}


			// キーフレームの描画
			for (const auto& kf : editingData_.positionKeys)
			{
				float kfX = canvasPos.x + ((kf.time / editingData_.duration) * canvasSize.x);
				drawList->AddCircleFilled(ImVec2(kfX, posY), 4.0f, IM_COL32(50, 255, 50, 255));
			}
			for (const auto& kf : editingData_.rotationKeys) 
			{
				float kfX = canvasPos.x + ((kf.time / editingData_.duration) * canvasSize.x);
				drawList->AddCircleFilled(ImVec2(kfX, rotY), 4.0f, IM_COL32(50, 150, 255, 255));
			}
			for (const auto& kf : editingData_.fovKeys)
			{
				float kfX = canvasPos.x + ((kf.time / editingData_.duration) * canvasSize.x);
				drawList->AddCircleFilled(ImVec2(kfX, fovY), 4.0f, IM_COL32(255, 255, 50, 255));
			}
		}

		// 再生ヘッド（赤いライン）を描画
		float currentRatio = editingData_.duration > 0.0f ? (currentTime_ / editingData_.duration) : 0.0f;
		float playheadX = canvasPos.x + (currentRatio * canvasSize.x);
		drawList->AddLine(ImVec2(playheadX, canvasPos.y), ImVec2(playheadX, canvasPos.y + canvasSize.y), IM_COL32(255, 50, 50, 255), 2.0f);
	}

	ImGui::End();
}