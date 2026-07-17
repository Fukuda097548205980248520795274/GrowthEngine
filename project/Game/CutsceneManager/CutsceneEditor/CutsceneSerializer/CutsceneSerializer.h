#pragma once
#include <json.hpp>
#include "../CutsceneKeyframe/CutsceneKeyframe.h"

using json = nlohmann::json;

/// @brief カットシーンデータをJSONに変換して保存する
/// @param filePath 
/// @param data 
inline void SaveCutscene(const std::string& filePath, const KeyframeCutsceneData& data)
{
	json j;
	j["name"] = data.name;
	j["duration"] = data.duration;

	// 位置のキーフレームを保存
	json positionKeysJson = json::array();
	for (auto& key : data.positionKeys)
	{
		json keyJson;
		keyJson["time"] = key.time;
		keyJson["value"] = { key.value.x, key.value.y, key.value.z };
		positionKeysJson.push_back(keyJson);
	}
	j["positionKeys"] = positionKeysJson;

	// 回転のキーフレームを保存
	json rotationKeysJson = json::array();
	for (auto& key : data.rotationKeys)
	{
		json keyJson;
		keyJson["time"] = key.time;
		keyJson["value"] = { key.value.x, key.value.y, key.value.z };
		rotationKeysJson.push_back(keyJson);
	}
	j["rotationKeys"] = rotationKeysJson;

	// FOVのキーフレームを保存
	json fovKeysJson = json::array();
	for (auto& key : data.fovKeys)
	{
		json keyJson;
		keyJson["time"] = key.time;
		keyJson["value"] = key.value;
		fovKeysJson.push_back(keyJson);
	}
	j["fovKeys"] = fovKeysJson;

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

		// 位置のキーフレームを読み込む
		data.positionKeys.clear();
		for (auto& keyJson : j["positionKeys"])
		{
			Key<Vector3> key;
			key.time = keyJson.value("time", 0.0f);
			auto valueArray = keyJson["value"];
			if (valueArray.is_array() && valueArray.size() == 3)
			{
				key.value.x = valueArray[0].get<float>();
				key.value.y = valueArray[1].get<float>();
				key.value.z = valueArray[2].get<float>();
			}
			data.positionKeys.push_back(key);
		}

		// 回転のキーフレームを読み込む
		data.rotationKeys.clear();
		for (auto& keyJson : j["rotationKeys"])
		{
			Key<Vector3> key;
			key.time = keyJson.value("time", 0.0f);
			auto valueArray = keyJson["value"];
			if (valueArray.is_array() && valueArray.size() == 3)
			{
				key.value.x = valueArray[0].get<float>();
				key.value.y = valueArray[1].get<float>();
				key.value.z = valueArray[2].get<float>();
			}
			data.rotationKeys.push_back(key);
		}

		// FOVのキーフレームを読み込む
		data.fovKeys.clear();
		for (auto& keyJson : j["fovKeys"])
		{
			Key<float> key;
			key.time = keyJson.value("time", 0.0f);
			key.value = keyJson.value("value", 0.0f);
			data.fovKeys.push_back(key);
		}

		// 時間順に並び替え
		auto sortByTime = [](auto& a, auto& b){return a.time < b.time;};
		std::sort(data.positionKeys.begin(), data.positionKeys.end(), sortByTime);
		std::sort(data.rotationKeys.begin(), data.rotationKeys.end(), sortByTime);
		std::sort(data.fovKeys.begin(), data.fovKeys.end(), sortByTime);
	}

	return data;
}