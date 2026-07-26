#pragma once
#include <json.hpp>
#include <fstream>
#include <string>
#include <vector>
#include "../UIData/UIData.h"

using json = nlohmann::json;

/// @brief UI要素のデータをJSON形式で保存する
/// @param elements 
inline json ToJsonData(const std::vector<UIElementData>& elements)
{
	json j = json::array();
	for (const auto& elem : elements)
	{
		json elemJson;
		elemJson["name"] = elem.name;
		elemJson["textureFilename"] = elem.textureFilename;

		if (elem.sprite && elem.sprite->param_)
		{
			auto* param = elem.sprite->param_;
			elemJson["blendMode"] = static_cast<int>(param->blendMode);
			elemJson["screenAnchor"] = static_cast<int>(param->screenAnchor);
			elemJson["transform"] = {
				{"translate", {param->transform.translate.x, param->transform.translate.y}},
				{"scale", {param->transform.scale.x, param->transform.scale.y}},
				{"rotate", param->transform.rotate}
			};
			elemJson["color"] = {
				param->material.color.x, param->material.color.y,
				param->material.color.z, param->material.color.w
			};
		}
		j.push_back(elemJson);
	}
	return j;
}

/// @brief JSONからUI要素のデータを読み込む
/// @param j 
/// @param loadedTextures 
/// @return 
inline std::vector<UIElementData> FromJsonData(const json& j, const std::unordered_map<std::string, TextureHandle>& loadedTextures)
{
	std::vector<UIElementData> elements;
	for (const auto& elemJson : j)
	{
		UIElementData data;
		data.name = elemJson.value("name", "Unnamed");
		data.textureFilename = elemJson.value("textureFilename", "");

		TextureHandle hTexture = 0;
		auto it = loadedTextures.find(data.textureFilename);
		if (it != loadedTextures.end()) hTexture = it->second;

		data.sprite = std::make_unique<Sprite>(hTexture, data.name);
		auto* param = data.sprite->param_;

		if (param)
		{
			param->blendMode = static_cast<BlendMode>(elemJson.value("blendMode", 0));
			param->screenAnchor = static_cast<Engine::Render2D::ScreenAnchor>(elemJson.value("screenAnchor", 0));
			if (elemJson.contains("transform"))
			{
				auto transform = elemJson["transform"];
				auto trans = transform["translate"];
				auto sc = transform["scale"];
				param->transform.translate = { trans[0].get<float>(), trans[1].get<float>() };
				param->transform.scale = { sc[0].get<float>(), sc[1].get<float>() };
				param->transform.rotate = transform.value("rotate", 0.0f);
			}
			if (elemJson.contains("color"))
			{
				auto color = elemJson["color"];
				param->material.color = { color[0].get<float>(), color[1].get<float>(), color[2].get<float>(), color[3].get<float>() };
			}
		}
		elements.push_back(std::move(data));
	}
	return elements;
}

/// @brief UI要素のデータをJSON形式で保存する
/// @param filePath 
/// @param elements 
inline void ToJson(const std::string& filePath, const std::vector<UIElementData>& elements)
{
	std::ofstream file(filePath);
	if (file.is_open()) file << ToJsonData(elements).dump(4);
}

/// @brief JSONからUI要素のデータを読み込む
/// @param filePath 
/// @param loadedTextures 
/// @return 
inline std::vector<UIElementData> FromJson(const std::string& filePath, const std::unordered_map<std::string, TextureHandle>& loadedTextures)
{
	std::ifstream file(filePath);
	if (file.is_open())
	{
		json j;
		file >> j;
		return FromJsonData(j, loadedTextures);
	}
	return {};
}