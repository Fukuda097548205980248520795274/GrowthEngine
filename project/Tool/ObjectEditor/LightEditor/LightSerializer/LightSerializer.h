#pragma once
#include <json.hpp>
#include <fstream>
#include <string>
#include <vector>
#include "../LightData/LightData.h"

using json = nlohmann::json;

/// @brief ライト要素のデータをJSON形式で保存する
/// @param elements 
/// @return 
inline json ToJsonData(const std::vector<LightElementData>& elements)
{
	json j = json::array();
	for (const auto& elem : elements)
	{
		json elemJson;
		elemJson["name"] = elem.name;
		elemJson["lightType"] = static_cast<int>(elem.lightType);
		
		// ライトの種類に応じてパラメータを保存
		if (elem.light && elem.lightType == Engine::Light::Type::Directional)
		{
			auto* dirLight = dynamic_cast<LightDirectional*>(elem.light.get());
			elemJson["direction"] = { dirLight->param_->direction.x, dirLight->param_->direction.y, dirLight->param_->direction.z };
			elemJson["intensity"] = dirLight->param_->intensity;
			elemJson["color"] = { dirLight->param_->color.x, dirLight->param_->color.y, dirLight->param_->color.z };
			elemJson["position"] = { dirLight->param_->position.x, dirLight->param_->position.y, dirLight->param_->position.z };
			elemJson["size"] = { dirLight->param_->size.x, dirLight->param_->size.y };
			elemJson["minDepth"] = dirLight->param_->minDepth;
			elemJson["maxDepth"] = dirLight->param_->maxDepth;
		}
		else if (elem.light && elem.lightType == Engine::Light::Type::Point)
		{
			auto* pointLight = dynamic_cast<LightPoint*>(elem.light.get());
			elemJson["position"] = { pointLight->param_->position.x, pointLight->param_->position.y, pointLight->param_->position.z };
			elemJson["intensity"] = pointLight->param_->intensity;
			elemJson["color"] = { pointLight->param_->color.x, pointLight->param_->color.y, pointLight->param_->color.z };
			elemJson["radius"] = pointLight->param_->radius;
			elemJson["decay"] = pointLight->param_->decay;

		}
		else if (elem.light && elem.lightType == Engine::Light::Type::Spot)
		{
			auto* spotLight = dynamic_cast<LightSpot*>(elem.light.get());
			elemJson["position"] = { spotLight->param_->position.x, spotLight->param_->position.y, spotLight->param_->position.z };
			elemJson["direction"] = { spotLight->param_->direction.x, spotLight->param_->direction.y, spotLight->param_->direction.z };
			elemJson["intensity"] = spotLight->param_->intensity;
			elemJson["color"] = { spotLight->param_->color.x, spotLight->param_->color.y, spotLight->param_->color.z };
			elemJson["distance"] = spotLight->param_->distance;
			elemJson["decay"] = spotLight->param_->decay;
			elemJson["cosAngle"] = spotLight->param_->cosAngle;
			elemJson["cosFalloffStart"] = spotLight->param_->cosFalloffStart;
		}

		j.push_back(elemJson);
	}
	return j;
}

/// @brief JSONからライト要素のデータを読み込む
/// @param j 
/// @return 
inline std::vector<LightElementData> FromJsonData(const json& j)
{
	std::vector<LightElementData> elements;
	for (auto& elemJson : j)
	{
		LightElementData data;
		data.name = elemJson.value("name", "Unnamed");
		data.lightType = static_cast<Engine::Light::Type>(elemJson.value("lightType", 0));

		// 種類がNoneの場合はスキップ
		if (data.lightType == Engine::Light::Type::None)continue;

		// ライトの種類に応じてパラメータを復元
		if (data.lightType == Engine::Light::Type::Directional)
		{
			data.light = std::make_unique<LightDirectional>(data.name);
			auto* dirLight = dynamic_cast<LightDirectional*>(data.light.get());
			dirLight->param_->direction = { elemJson["direction"][0], elemJson["direction"][1], elemJson["direction"][2] };
			dirLight->param_->intensity = elemJson.value("intensity", 1.0f);
			dirLight->param_->color = { elemJson["color"][0], elemJson["color"][1], elemJson["color"][2] };
			dirLight->param_->position = { elemJson["position"][0], elemJson["position"][1], elemJson["position"][2] };
			dirLight->param_->size = { elemJson["size"][0], elemJson["size"][1] };
			dirLight->param_->minDepth = elemJson.value("minDepth", 0.1f);
			dirLight->param_->maxDepth = elemJson.value("maxDepth", 100.0f);
		}
		else if (data.lightType == Engine::Light::Type::Point)
		{
			data.light = std::make_unique<LightPoint>(data.name);
			auto* pointLight = dynamic_cast<LightPoint*>(data.light.get());
			pointLight->param_->position = { elemJson["position"][0], elemJson["position"][1], elemJson["position"][2] };
			pointLight->param_->intensity = elemJson.value("intensity", 1.0f);
			pointLight->param_->color = { elemJson["color"][0], elemJson["color"][1], elemJson["color"][2] };
			pointLight->param_->radius = elemJson.value("radius", 10.0f);
			pointLight->param_->decay = elemJson.value("decay", 1.0f);
		}
		else if (data.lightType == Engine::Light::Type::Spot)
		{
			data.light = std::make_unique<LightSpot>(data.name);
			auto* spotLight = dynamic_cast<LightSpot*>(data.light.get());
			spotLight->param_->position = { elemJson["position"][0], elemJson["position"][1], elemJson["position"][2] };
			spotLight->param_->direction = { elemJson["direction"][0], elemJson["direction"][1], elemJson["direction"][2] };
			spotLight->param_->intensity = elemJson.value("intensity", 1.0f);
			spotLight->param_->color = { elemJson["color"][0], elemJson["color"][1], elemJson["color"][2] };
			spotLight->param_->distance = elemJson.value("distance", 10.0f);
			spotLight->param_->decay = elemJson.value("decay", 1.0f);
			spotLight->param_->cosAngle = elemJson.value("cosAngle", 0.5f);
			spotLight->param_->cosFalloffStart = elemJson.value("cosFalloffStart", 0.7f);
		}

		// リストに追加
		elements.push_back(std::move(data));
	}

	return elements;
}

/// @brief ライト要素のデータをJSON形式で保存する
/// @param filePath 
/// @param elements 
inline void ToJson(const std::string& filePath, const std::vector<LightElementData>& elements)
{
	std::ofstream file(filePath);
	if (file.is_open()) file << ToJsonData(elements).dump(4);
}

/// @brief JSONからライト要素のデータを読み込む
/// @param filePath 
/// @return 
inline std::vector<LightElementData> FromJson(const std::string& filePath)
{
	std::ifstream file(filePath);
	if (file.is_open())
	{
		json j;
		file >> j;
		return FromJsonData(j);
	}
	return{};
}