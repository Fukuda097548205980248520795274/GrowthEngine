#pragma once
#include <json.hpp>
#include "../CutsceneKeyframe/CutsceneKeyframe.h"

using json = nlohmann::json;

/// @brief CameraKeyframeをJSONに変換する
/// @param j 
/// @param kf 
inline void ToJson(json& j, const CameraKeyframe& kf)
{
	j["time"] = kf.time;
	j["position"] = { kf.position.x, kf.position.y, kf.position.z };
	j["rotation"] = { kf.rotation.x, kf.rotation.y, kf.rotation.z };
	j["fov"] = kf.fov;
}

/// @brief JSONからCameraKeyframeに変換する
/// @param j 
/// @param kf 
inline void FromJson(const json& j, CameraKeyframe& kf)
{
	kf.time = j.value("time", 0.0f);

	kf.position.x = j.value("position", std::vector<float>{0.0f, 0.0f, 0.0f})[0];
	kf.position.y = j.value("position", std::vector<float>{0.0f, 0.0f, 0.0f})[1];
	kf.position.z = j.value("position", std::vector<float>{0.0f, 0.0f, 0.0f})[2];

	kf.rotation.x = j.value("rotation", std::vector<float>{0.0f, 0.0f, 0.0f})[0];
	kf.rotation.y = j.value("rotation", std::vector<float>{0.0f, 0.0f, 0.0f})[1];
	kf.rotation.z = j.value("rotation", std::vector<float>{0.0f, 0.0f, 0.0f})[2];

	kf.fov = j.value("fov", 0.45f);
}

/// @brief カットシーンデータをJSONに変換して保存する
/// @param filePath 
/// @param data 
inline void SaveCutscene(const std::string& filePath, const KeyframeCutsceneData& data)
{
	json j;
	j["name"] = data.name;
	j["duration"] = data.duration;

	// キーフレームをJSONに変換して配列に追加
	json keyframesJson = json::array();
	for (auto& kf : data.keyframes)
	{
		json kfJson;
		ToJson(kfJson, kf);
		keyframesJson.push_back(kfJson);
	}
	j["keyframes"] = keyframesJson;

	std::ofstream file(filePath);

	// インデント4で綺麗に整形して出力
	if (file.is_open()) 
		file << j.dump(4);
}

/// @brief JSONからカットシーンデータを読み込む
/// @param filePath 
/// @return 
inline KeyframeCutsceneData LoadCutscene(const std::string& filePath)
{
	KeyframeCutsceneData data;
	std::ifstream file(filePath);

	if (file.is_open())
	{
		json j;
		file >> j;
		data.name = j.value("name", "");
		data.duration = j.value("duration", 0.0f);

		// キーフレームの読み込み
		data.keyframes.clear();
		for (auto& kfJson : j["keyframes"])
		{
			CameraKeyframe kf;
			FromJson(kfJson, kf);
			data.keyframes.push_back(kf);
		}

		// 時間順に並び替え
		std::sort(data.keyframes.begin(), data.keyframes.end(),
			[](const CameraKeyframe& a, const CameraKeyframe& b) { return a.time < b.time; });
	}

	return data;
}