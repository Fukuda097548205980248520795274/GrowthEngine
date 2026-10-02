#include "ConfigEditor.h"
#include <json.hpp>

#include "Scene/GameScene/GameScene.h"

using json = nlohmann::json;

/// @brief GameConfigをJSONに変換する
/// @param j 
/// @param config 
void ToJson(json& j, const GameConfig& config)
{
	j["pivotFollowSpeed"] = config.pivotFollowSpeed;
	j["pivotRotateSpeed"] = config.pivotRotateSpeed;
	j["pivotMaxPitch"] = config.pivotMaxPitch;
	j["pivotCenterOffset"] = {config.pivotCenterOffset.x, config.pivotCenterOffset.y, config.pivotCenterOffset.z};
	j["lockOnMaxPitch"] = config.lockOnMaxPitch;
	j["lockOnCameraFollowSpeed"] = config.lockOnCameraFollowSpeed;
	j["cameraShrinkSpeed"] = config.cameraShrinkSpeed;
	j["cameraExpandSpeed"] = config.cameraExpandSpeed;
}

/// @brief JSONからGameConfigに変換する
/// @param j 
/// @param config 
void FromJson(const json& j, GameConfig& config)
{
	config.pivotFollowSpeed = j.value("pivotFollowSpeed", 10.0f);
	config.pivotRotateSpeed = j.value("pivotRotateSpeed", 1.5f);
	config.pivotMaxPitch = j.value("pivotMaxPitch", 70.0f * (std::numbers::pi_v<float> / 180.0f));

	config.pivotCenterOffset.x = j.value("pivotCenterOffset", std::vector<float>{0.0f, 1.5f, 0.0f})[0];
	config.pivotCenterOffset.y = j.value("pivotCenterOffset", std::vector<float>{0.0f, 1.5f, 0.0f})[1];
	config.pivotCenterOffset.z = j.value("pivotCenterOffset", std::vector<float>{0.0f, 1.5f, 0.0f})[2];

	config.lockOnMaxPitch = j.value("lockOnMaxPitch", 20.0f * (std::numbers::pi_v<float> / 180.0f));
	config.lockOnCameraFollowSpeed = j.value("lockOnCameraFollowSpeed", 3.0f);
	config.cameraShrinkSpeed = j.value("cameraShrinkSpeed", 25.0f);
	config.cameraExpandSpeed = j.value("cameraExpandSpeed", 5.0f);
}

/// @brief 初期化する
/// @param scene 
void ConfigEditor::Initialize(GameScene* scene)
{
	// nullptrチェック
	assert(scene);

	// 引数を受け取る
	scene_ = scene;
}

/// @brief UIを描画する
void ConfigEditor::DrawUI()
{
	ImGui::Begin("設定エディタ");

	// ファイル操作
	if (ImGui::CollapsingHeader("ファイル操作", ImGuiTreeNodeFlags_DefaultOpen))
	{
		static char fileNameBuffer[128] = "Default";

		// 読み込んだファイル名が存在する場合バッファを更新
		if (!currentFileName_.empty() && strcmp(fileNameBuffer, currentFileName_.c_str()) != 0)
		{
			strncpy_s(fileNameBuffer, currentFileName_.c_str(), sizeof(fileNameBuffer));
		}

		ImGui::InputText("File Name", fileNameBuffer, sizeof(fileNameBuffer));

		if (ImGui::Button("Save"))
		{
			Save(fileNameBuffer);
		}
		ImGui::SameLine();
		if (ImGui::Button("Load"))
		{
			Load(fileNameBuffer);
		}
	}

	ImGui::Separator();

	// ピボットカメラのパラメータ
	if (ImGui::CollapsingHeader("ピボットカメラ", ImGuiTreeNodeFlags_DefaultOpen))
	{
		bool isChanged = false;

		// 追従補間速度
		if (ImGui::DragFloat("追従補間速度", &config_.pivotFollowSpeed, 0.01f, 0.0f, 100.0f, "%.2f"))
		{
			isChanged = true;
		}

		// 回転速度 (rad/s)
		if (ImGui::DragFloat("回転速度（ rad/s ）", &config_.pivotRotateSpeed, 0.05f, 0.0f, 20.0f, "%.2f rad/s"))
		{
			isChanged = true;
		}

		// 最大ピッチ角度 (ラジアン操作)
		if (ImGui::SliderAngle("最大ピッチ角度", &config_.pivotMaxPitch, 0.0f, 89.0f, "%.1f deg"))
		{
			isChanged = true;
		}

		// オフセット位置
		if (ImGui::DragFloat3("オフセット位置", &config_.pivotCenterOffset.x, 0.01f, -50.0f, 50.0f, "%.2f"))
		{
			isChanged = true;
		}

		// ロックオン時の最大ピッチ角度 (ラジアン操作)
		if (ImGui::SliderAngle("ロックオン時の最大ピッチ角度", &config_.lockOnMaxPitch, 0.0f, 89.0f, "%.1f deg"))
		{
			isChanged = true;
		}

		// ロックオン時のカメラ追従速度
		if (ImGui::DragFloat("ロックオン時のカメラ追従速度", &config_.lockOnCameraFollowSpeed, 0.01f, 0.0f, 100.0f, "%.2f"))
		{
			isChanged = true;
		}

		// カメラの縮小速度
		if (ImGui::DragFloat("カメラの縮小速度", &config_.cameraShrinkSpeed, 0.01f, 0.0f, 100.0f, "%.2f"))
		{
			isChanged = true;
		}

		// カメラの拡大速度
		if (ImGui::DragFloat("カメラの拡大速度", &config_.cameraExpandSpeed, 0.01f, 0.0f, 100.0f, "%.2f"))
		{
			isChanged = true;
		}

		// パラメータ変更時に即時でシーンへ反映
		if (isChanged && scene_)
		{
			scene_->ApplyConfig(config_);
		}
	}

	ImGui::End();
}

/// @brief 設定ファイルを読み込む
/// @param fileName 
void ConfigEditor::Load(const std::string& fileName)
{
	// 設定ファイルのパスを作成する
	std::string filePath = kDir + fileName + ".json";

	// JSONファイルを読み込む
	std::ifstream file(filePath);
	if (file.is_open())
	{
		json j;
		file >> j;
		file.close();

		// JSONからGameConfigに変換する
		FromJson(j, config_);

		// 現在の設定ファイル名を更新する
		currentFileName_ = fileName;

		// 設定をシーンに反映させる
		scene_->ApplyConfig(config_);
	}
}

/// @brief 設定ファイルを保存する
/// @param fileName 
void ConfigEditor::Save(const std::string& fileName)
{
	// 設定ファイルのパスを作成する
	std::string filePath = kDir + fileName + ".json";

	// GameConfigをJSONに変換する
	json j;
	ToJson(j, config_);

	// JSONファイルに保存する
	std::ofstream file(filePath);
	if (file.is_open())
	{
		file << j.dump(4);
		file.close();
	}
}