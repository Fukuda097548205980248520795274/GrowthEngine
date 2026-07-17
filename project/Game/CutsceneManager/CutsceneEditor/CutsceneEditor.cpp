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

		// 再生時間がカットシーンの長さを超えたら停止
		if (currentTime_ >= editingData_.duration)
		{
			currentTime_ = editingData_.duration;
			isPlaying_ = false;
		}

		// キーフレーム補間を適用
		CameraKeyframe sample = SampleCutscene(editingData_, currentTime_);
		cutsceneCamera_->param_->transform.translate = sample.position;
		cutsceneCamera_->param_->transform.rotate = sample.rotation;
		cutsceneCamera_->param_->setting.fov = sample.fov;
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
/// @param rot 
/// @param fov 
void CutsceneEditor::AddKeyframe(float time, const Vector3& pos, const Vector3& rot, float fov)
{
	// 同一時間軸にキーフレームがすでに存在する場合は上書き
	auto it = std::find_if(editingData_.keyframes.begin(), editingData_.keyframes.end(),
		[time](const CameraKeyframe& k) { return std::abs(k.time - time) < 0.01f; });

	// すでに存在する場合は上書き
	if (it != editingData_.keyframes.end())
	{
		it->position = pos;
		it->rotation = rot;
		it->fov = fov;
	}
	else
	{
		// 新しいキーフレームを追加
		CameraKeyframe newKey;
		newKey.time = time;
		newKey.position = pos;
		newKey.rotation = rot;
		newKey.fov = fov;
		editingData_.keyframes.push_back(newKey);
	}

	// 時間順に並び替え
	std::sort(editingData_.keyframes.begin(), editingData_.keyframes.end(),
		[](const CameraKeyframe& a, const CameraKeyframe& b) { return a.time < b.time; });
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

		// タイムラインとシークコントロール
		ImGui::Text("Timeline");

		// タイムラインの描画領域を確保
		ImVec2 canvasPos = ImGui::GetCursorScreenPos();
		ImVec2 canvasSize = ImVec2(ImGui::GetContentRegionAvail().x, 40.0f); // 横幅いっぱい、高さ40px
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		// 1. タイムラインの背景を描画 (ダークグレー)
		drawList->AddRectFilled(canvasPos, ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y), IM_COL32(40, 40, 40, 255));
		drawList->AddRect(canvasPos, ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y), IM_COL32(100, 100, 100, 255)); // 枠線

		// 2. キーフレームのマーカーを描画
		if (editingData_.duration > 0.0f)
		{
			for (const auto& kf : editingData_.keyframes)
			{
				// 時間からX座標を計算
				float kfRatio = kf.time / editingData_.duration;
				float kfX = canvasPos.x + (kfRatio * canvasSize.x);

				// キーフレームの位置にひし形(または円)を描画 (緑色)
				ImVec2 center(kfX, canvasPos.y + canvasSize.y * 0.5f);
				float radius = 5.0f;
				drawList->AddQuadFilled(
					ImVec2(center.x, center.y - radius),
					ImVec2(center.x + radius, center.y),
					ImVec2(center.x, center.y + radius),
					ImVec2(center.x - radius, center.y),
					IM_COL32(0, 255, 0, 255)
				);
			}
		}

		// 3. 再生ヘッド（現在時間のライン）を描画
		float currentRatio = editingData_.duration > 0.0f ? (currentTime_ / editingData_.duration) : 0.0f;
		float playheadX = canvasPos.x + (currentRatio * canvasSize.x);
		drawList->AddLine(
			ImVec2(playheadX, canvasPos.y),
			ImVec2(playheadX, canvasPos.y + canvasSize.y),
			IM_COL32(255, 50, 50, 255),
			2.0f
		); // 赤い縦線

		// 4. タイムライン上でのマウス操作（シーク操作）
		ImGui::InvisibleButton("TimelineInteraction", canvasSize);
		if (ImGui::IsItemActive() || ImGui::IsItemClicked())
		{
			// マウスのX座標から現在時間を逆算
			float mouseX = ImGui::GetIO().MousePos.x - canvasPos.x;
			float newRatio = std::clamp(mouseX / canvasSize.x, 0.0f, 1.0f);
			currentTime_ = newRatio * editingData_.duration;

			// シークバーを操作した場合は再生を停止し、即座にプレビュー反映
			isPlaying_ = false;
			CameraKeyframe sample = SampleCutscene(editingData_, currentTime_);
			cutsceneCamera_->param_->transform.translate = sample.position;
			cutsceneCamera_->param_->transform.rotate = sample.rotation;
			cutsceneCamera_->param_->setting.fov = sample.fov;
		}

		ImGui::Separator();

		// キーフレーム操作
		ImGui::Text("キーフレーム管理");
		if (cutsceneCamera_)
		{
			auto& trans = cutsceneCamera_->param_->transform;
			auto& setting = cutsceneCamera_->param_->setting;

			ImGui::Text("カメラ 位置: (%.2f, %.2f, %.2f)", trans.translate.x, trans.translate.y, trans.translate.z);
			ImGui::Text("カメラ 回転: (%.2f, %.2f, %.2f)", trans.rotate.x, trans.rotate.y, trans.rotate.z);

			if (ImGui::Button("今の時間にキーフレームを追加する"))
			{
				AddKeyframe(currentTime_, trans.translate, trans.rotate, setting.fov);
			}
		}

		// 登録されているキーフレームの一覧
		ImGui::Text("記録されているキーフレーム :");
		for (auto it = editingData_.keyframes.begin(); it != editingData_.keyframes.end(); )
		{
			ImGui::PushID(static_cast<int32_t>(it->time));
			ImGui::Text("時間 : %.2fs Pos(%.1f, %.1f, %.1f)", it->time, it->position.x, it->position.y, it->position.z);
			ImGui::SameLine();
			if (ImGui::Button("削除"))
			{
				it = editingData_.keyframes.erase(it);
			}
			else
			{
				++it;
			}
			ImGui::PopID();
		}
	}
	ImGui::End();
}