#include "CutsceneEditor.h"
#include "GrowthEngine.h"
#include "CutsceneSerializer/CutsceneSerializer.h"

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

/// @brief キーフレームを追加する
/// @param time 
/// @param pos 
void CutsceneEditor::AddPositionKeyframe(float time, const Vector3& pos)
{
	// 同一時間軸にキーフレームがすでに存在する場合は上書き
	auto it = std::find_if(editingData_.positionKeys.begin(), editingData_.positionKeys.end(),
		[time](const Key<Vector3>& k) { return std::abs(k.time - time) < 0.01f; });

	// すでに存在する場合は上書き
	if (it != editingData_.positionKeys.end())
	{
		it->time = time;
		it->value = pos;
	}
	else
	{
		// 新しいキーフレームを追加
		Key<Vector3> newKey;
		newKey.time = time;
		newKey.value = pos;
		editingData_.positionKeys.push_back(newKey);
	}

	// 時間順に並び替え
	std::sort(editingData_.positionKeys.begin(), editingData_.positionKeys.end(),
		[](const Key<Vector3>& a, const Key<Vector3>& b) { return a.time < b.time; });
}

/// @brief 回転のキーフレームを追加する
/// @param time 
/// @param rot 
void CutsceneEditor::AddRotationKeyframe(float time, const Vector3& rot)
{
	// 同一時間軸にキーフレームがすでに存在する場合は上書き
	auto it = std::find_if(editingData_.rotationKeys.begin(), editingData_.rotationKeys.end(),
		[time](const Key<Vector3>& k) { return std::abs(k.time - time) < 0.01f; });

	// すでに存在する場合は上書き
	if (it != editingData_.rotationKeys.end())
	{
		it->time = time;
		it->value = rot;
	}
	else
	{
		// 新しいキーフレームを追加
		Key<Vector3> newKey;
		newKey.time = time;
		newKey.value = rot;
		editingData_.rotationKeys.push_back(newKey);
	}

	// 時間順に並び替え
	std::sort(editingData_.rotationKeys.begin(), editingData_.rotationKeys.end(),
		[](const Key<Vector3>& a, const Key<Vector3>& b) { return a.time < b.time; });
}

/// @brief FOVのキーフレームを追加する
/// @param time 
/// @param fov 
void CutsceneEditor::AddFovKeyframe(float time, float fov)
{
	// 同一時間軸にキーフレームがすでに存在する場合は上書き
	auto it = std::find_if(editingData_.fovKeys.begin(), editingData_.fovKeys.end(),
		[time](const Key<float>& k) { return std::abs(k.time - time) < 0.01f; });

	// すでに存在する場合は上書き
	if (it != editingData_.fovKeys.end())
	{
		it->time = time;
		it->value = fov;
	} 
	else
	{
		// 新しいキーフレームを追加
		Key<float> newKey;
		newKey.time = time;
		newKey.value = fov;
		editingData_.fovKeys.push_back(newKey);
	}

	// 時間順に並び替え
	std::sort(editingData_.fovKeys.begin(), editingData_.fovKeys.end(),
		[](const Key<float>& a, const Key<float>& b) { return a.time < b.time; });
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
#ifdef _DEVELOPMENT
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
			if (ImGui::Button("今の時間に位置キーを追加")) {
				AddPositionKeyframe(currentTime_, trans.translate);
			}

			ImGui::Separator();

			ImGui::DragFloat3("カメラ 回転", &trans.rotate.x, 0.1f);
			if (ImGui::Button("今の時間に回転キーを追加")) {
				AddRotationKeyframe(currentTime_, trans.rotate);
			}

			ImGui::Separator();

			ImGui::DragFloat("カメラ 画角", &setting.fov, 0.01f);
			if (ImGui::Button("今の時間に画角キーを追加")) {
				AddFovKeyframe(currentTime_, setting.fov);
			}
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
		ImVec2 canvasPos = ImGui::GetCursorScreenPos();
		ImVec2 canvasSize = ImVec2(ImGui::GetContentRegionAvail().x, 60.0f); // 高さを確保
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		// 背景の描画
		drawList->AddRectFilled(canvasPos, ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y), IM_COL32(40, 40, 40, 255));
		drawList->AddRect(canvasPos, ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y), IM_COL32(100, 100, 100, 255));

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

			// キーフレームのホバー判定を行うラムダ関数
			auto checkHover = [&](const auto& keys, float trackY, DraggingTrack type) 
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
			}

			// クリックした瞬間にキーフレームの上にいたらドラッグ状態に移行
			if (isCanvasClicked && isHoveringAnyKey) 
			{
				draggingTrack_ = hoveredTrack;
				draggingKeyIndex_ = hoveredIndex;
			}

			// ドラッグ中の処理
			if (isCanvasActive)
			{
				float newRatio = std::clamp(mouseX / canvasSize.x, 0.0f, 1.0f);
				float newTime = newRatio * editingData_.duration;

				if (draggingTrack_ != DraggingTrack::None)
				{
					// キーフレームを掴んでいる場合、対象のキーフレームの時間を更新
					if (draggingTrack_ == DraggingTrack::Position) editingData_.positionKeys[draggingKeyIndex_].time = newTime;
					else if (draggingTrack_ == DraggingTrack::Rotation) editingData_.rotationKeys[draggingKeyIndex_].time = newTime;
					else if (draggingTrack_ == DraggingTrack::FOV) editingData_.fovKeys[draggingKeyIndex_].time = newTime;

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

				// ドラッグ状態を解除
				draggingTrack_ = DraggingTrack::None;
				draggingKeyIndex_ = -1;
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