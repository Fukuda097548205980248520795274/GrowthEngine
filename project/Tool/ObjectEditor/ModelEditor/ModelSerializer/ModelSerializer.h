#pragma once
#include <json.hpp>
#include <fstream>
#include <string>
#include <vector>
#include "../ModelData/ModelData.h"

using json = nlohmann::json;

/// @brief モデル要素のデータをJSON形式で保存する
/// @param elements 
/// @return 
inline json ToJsonData(const std::vector<ModelElementData>& elements)
{
	json j = json::array();
	for (const auto& elem : elements)
	{
		json elemJson;
		elemJson["modelName"] = elem.modelName;
		elemJson["modelDirectory"] = elem.modelDirectory;
		elemJson["modelFileName"] = elem.modelFileName;
		elemJson["animationDirectory"] = elem.animationDirectory;
		elemJson["animationFileName"] = elem.animationFileName;
		elemJson["skeletonDirectory"] = elem.skeletonDirectory;
		elemJson["skeletonFileName"] = elem.skeletonFileName;
		elemJson["type"] = static_cast<int>(elem.type);

		if (elem.render3D && elem.type == Engine::Render3D::Type::StaticModel)
		{
			auto staticModel = static_cast<Render3DStaticModel*>(elem.render3D.get());

			// ブレンドモード
			elemJson["blendMode"] = static_cast<int>(staticModel->param_->blendMode);
			elemJson["translate"] = {
				staticModel->param_->modelTransform.translate.x,
				staticModel->param_->modelTransform.translate.y,
				staticModel->param_->modelTransform.translate.z
			};
			elemJson["scale"] = {
				staticModel->param_->modelTransform.scale.x,
				staticModel->param_->modelTransform.scale.y,
				staticModel->param_->modelTransform.scale.z
			};
			elemJson["rotate"] = {
				staticModel->param_->modelTransform.rotate.x,
				staticModel->param_->modelTransform.rotate.y,
				staticModel->param_->modelTransform.rotate.z
			};

			json meshJson = json::array();
			for (int i = 0; i < static_cast<int>(staticModel->param_->meshTransforms.size()); ++i)
			{
				// メッシュごとのJSONオブジェクト
				json meshJsonItem;

				// テクスチャパスを取得
				std::string texturePath = GrowthEngine::GetInstance()->GetTextureFilePath(staticModel->param_->meshMaterial[i].hTexture);
				if (!texturePath.empty())
				{
					meshJsonItem["texturePath"] = texturePath;
				}

				// メッシュトランスフォーム
				meshJsonItem["translate"] = {
					staticModel->param_->meshTransforms[i].translate.x,
					staticModel->param_->meshTransforms[i].translate.y,
					staticModel->param_->meshTransforms[i].translate.z
				};
				meshJsonItem["scale"] = {
					staticModel->param_->meshTransforms[i].scale.x,
					staticModel->param_->meshTransforms[i].scale.y,
					staticModel->param_->meshTransforms[i].scale.z
				};
				meshJsonItem["rotate"] = {
					staticModel->param_->meshTransforms[i].rotate.x,
					staticModel->param_->meshTransforms[i].rotate.y,
					staticModel->param_->meshTransforms[i].rotate.z
				};

				// マテリアル
				meshJsonItem["color"] = {
					staticModel->param_->meshMaterial[i].color.x,
					staticModel->param_->meshMaterial[i].color.y,
					staticModel->param_->meshMaterial[i].color.z,
					staticModel->param_->meshMaterial[i].color.w
				};
				meshJsonItem["uvTranslate"] = {
					staticModel->param_->meshMaterial[i].uv.translate.x,
					staticModel->param_->meshMaterial[i].uv.translate.y
				};
				meshJsonItem["uvScale"] = {
					staticModel->param_->meshMaterial[i].uv.scale.x,
					staticModel->param_->meshMaterial[i].uv.scale.y
				};
				meshJsonItem["uvRotate"] = staticModel->param_->meshMaterial[i].uv.radius;
				meshJsonItem["environment"] = staticModel->param_->meshMaterial[i].environment;
				meshJsonItem["shininess"] = staticModel->param_->meshMaterial[i].shininess;
				meshJsonItem["enableLighting"] = staticModel->param_->meshMaterial[i].enableLighting;
				meshJsonItem["enableDiffuse"] = staticModel->param_->meshMaterial[i].enableDiffuse;
				meshJsonItem["enableHalfLambert"] = staticModel->param_->meshMaterial[i].enableHalfLambert;
				meshJsonItem["enableSpecular"] = staticModel->param_->meshMaterial[i].enableSpecular;
				meshJsonItem["enableBlinnPhong"] = staticModel->param_->meshMaterial[i].enableBlinnPhong;
				meshJsonItem["drawShadowMap"] = staticModel->param_->meshMaterial[i].drawShadowMap;
				meshJsonItem["enableShadow"] = staticModel->param_->meshMaterial[i].enableShadow;

				// ブラー
				meshJsonItem["blurAfterImageMask"] = staticModel->param_->meshBlur[i].afterImageMask;
				meshJsonItem["blurMotionBlurMask"] = staticModel->param_->meshBlur[i].motionBlurMask;

				// アウトライン
				meshJsonItem["outlineEnable"] = staticModel->param_->meshOutline[i].enableOutline;
				meshJsonItem["outlineColor"] = {
					staticModel->param_->meshOutline[i].color.x,
					staticModel->param_->meshOutline[i].color.y,
					staticModel->param_->meshOutline[i].color.z,
					staticModel->param_->meshOutline[i].color.w
				};

				// メッシュごとのJSONを追加
				meshJson.push_back(meshJsonItem);
			}

			// メッシュデータを要素のJSONに追加
			elemJson["meshData"] = meshJson;
		}
		else if (elem.render3D && elem.type == Engine::Render3D::Type::AnimationModel)
		{
			auto animationModel = static_cast<Render3DAnimationModel*>(elem.render3D.get());

			// ブレンドモード
			elemJson["blendMode"] = static_cast<int>(animationModel->param_->blendMode);
			elemJson["translate"] = {
				animationModel->param_->modelTransform.translate.x,
				animationModel->param_->modelTransform.translate.y,
				animationModel->param_->modelTransform.translate.z
			};
			elemJson["scale"] = {
				animationModel->param_->modelTransform.scale.x,
				animationModel->param_->modelTransform.scale.y,
				animationModel->param_->modelTransform.scale.z
			};
			elemJson["rotate"] = {
				animationModel->param_->modelTransform.rotate.x,
				animationModel->param_->modelTransform.rotate.y,
				animationModel->param_->modelTransform.rotate.z
			};

			json meshJson = json::array();
			for (int i = 0; i < static_cast<int>(animationModel->param_->meshTransforms.size()); ++i)
			{
				// メッシュごとのJSONオブジェクト
				json meshJsonItem;

				// テクスチャパスを取得
				std::string texturePath = GrowthEngine::GetInstance()->GetTextureFilePath(animationModel->param_->meshMaterial[i].hTexture);
				if (!texturePath.empty())
				{
					meshJsonItem["texturePath"] = texturePath;
				}

				// メッシュトランスフォーム
				meshJsonItem["translate"] = {
					animationModel->param_->meshTransforms[i].translate.x,
					animationModel->param_->meshTransforms[i].translate.y,
					animationModel->param_->meshTransforms[i].translate.z
				};
				meshJsonItem["scale"] = {
					animationModel->param_->meshTransforms[i].scale.x,
					animationModel->param_->meshTransforms[i].scale.y,
					animationModel->param_->meshTransforms[i].scale.z
				};
				meshJsonItem["rotate"] = {
					animationModel->param_->meshTransforms[i].rotate.x,
					animationModel->param_->meshTransforms[i].rotate.y,
					animationModel->param_->meshTransforms[i].rotate.z
				};

				// マテリアル
				meshJsonItem["color"] = {
					animationModel->param_->meshMaterial[i].color.x,
					animationModel->param_->meshMaterial[i].color.y,
					animationModel->param_->meshMaterial[i].color.z,
					animationModel->param_->meshMaterial[i].color.w
				};
				meshJsonItem["uvTranslate"] = {
					animationModel->param_->meshMaterial[i].uv.translate.x,
					animationModel->param_->meshMaterial[i].uv.translate.y
				};
				meshJsonItem["uvScale"] = {
					animationModel->param_->meshMaterial[i].uv.scale.x,
					animationModel->param_->meshMaterial[i].uv.scale.y
				};
				meshJsonItem["uvRotate"] = animationModel->param_->meshMaterial[i].uv.radius;
				meshJsonItem["environment"] = animationModel->param_->meshMaterial[i].environment;
				meshJsonItem["shininess"] = animationModel->param_->meshMaterial[i].shininess;
				meshJsonItem["enableLighting"] = animationModel->param_->meshMaterial[i].enableLighting;
				meshJsonItem["enableDiffuse"] = animationModel->param_->meshMaterial[i].enableDiffuse;
				meshJsonItem["enableHalfLambert"] = animationModel->param_->meshMaterial[i].enableHalfLambert;
				meshJsonItem["enableSpecular"] = animationModel->param_->meshMaterial[i].enableSpecular;
				meshJsonItem["enableBlinnPhong"] = animationModel->param_->meshMaterial[i].enableBlinnPhong;
				meshJsonItem["drawShadowMap"] = animationModel->param_->meshMaterial[i].drawShadowMap;
				meshJsonItem["enableShadow"] = animationModel->param_->meshMaterial[i].enableShadow;

				// ブラー
				meshJsonItem["blurAfterImageMask"] = animationModel->param_->meshBlur[i].afterImageMask;
				meshJsonItem["blurMotionBlurMask"] = animationModel->param_->meshBlur[i].motionBlurMask;

				// アウトライン
				meshJsonItem["outlineEnable"] = animationModel->param_->meshOutline[i].enableOutline;
				meshJsonItem["outlineColor"] = {
					animationModel->param_->meshOutline[i].color.x,
					animationModel->param_->meshOutline[i].color.y,
					animationModel->param_->meshOutline[i].color.z,
					animationModel->param_->meshOutline[i].color.w
				};

				// メッシュごとのJSONを追加
				meshJson.push_back(meshJsonItem);
			}

			// メッシュデータを要素のJSONに追加
			elemJson["meshData"] = meshJson;
		}
		else if (elem.render3D && elem.type == Engine::Render3D::Type::SkinningModel)
		{
			auto skinningModel = static_cast<Render3DSkinningModel*>(elem.render3D.get());

			// ブレンドモード
			elemJson["blendMode"] = static_cast<int>(skinningModel->param_->blendMode);
			elemJson["translate"] = {
				skinningModel->param_->modelTransform.translate.x,
				skinningModel->param_->modelTransform.translate.y,
				skinningModel->param_->modelTransform.translate.z
			};
			elemJson["scale"] = {
				skinningModel->param_->modelTransform.scale.x,
				skinningModel->param_->modelTransform.scale.y,
				skinningModel->param_->modelTransform.scale.z
			};
			elemJson["rotate"] = {
				skinningModel->param_->modelTransform.rotate.x,
				skinningModel->param_->modelTransform.rotate.y,
				skinningModel->param_->modelTransform.rotate.z
			};

			json meshJson = json::array();
			for (int i = 0; i < static_cast<int>(skinningModel->param_->meshTransforms.size()); ++i)
			{
				// メッシュごとのJSONオブジェクト
				json meshJsonItem;

				// テクスチャパスを取得
				std::string texturePath = GrowthEngine::GetInstance()->GetTextureFilePath(skinningModel->param_->meshMaterial[i].hTexture);
				if (!texturePath.empty())
				{
					meshJsonItem["texturePath"] = texturePath;
				}

				// メッシュトランスフォーム
				meshJsonItem["translate"] = {
					skinningModel->param_->meshTransforms[i].translate.x,
					skinningModel->param_->meshTransforms[i].translate.y,
					skinningModel->param_->meshTransforms[i].translate.z
				};
				meshJsonItem["scale"] = {
					skinningModel->param_->meshTransforms[i].scale.x,
					skinningModel->param_->meshTransforms[i].scale.y,
					skinningModel->param_->meshTransforms[i].scale.z
				};
				meshJsonItem["rotate"] = {
					skinningModel->param_->meshTransforms[i].rotate.x,
					skinningModel->param_->meshTransforms[i].rotate.y,
					skinningModel->param_->meshTransforms[i].rotate.z
				};

				// マテリアル
				meshJsonItem["color"] = {
					skinningModel->param_->meshMaterial[i].color.x,
					skinningModel->param_->meshMaterial[i].color.y,
					skinningModel->param_->meshMaterial[i].color.z,
					skinningModel->param_->meshMaterial[i].color.w
				};
				meshJsonItem["uvTranslate"] = {
					skinningModel->param_->meshMaterial[i].uv.translate.x,
					skinningModel->param_->meshMaterial[i].uv.translate.y
				};
				meshJsonItem["uvScale"] = {
					skinningModel->param_->meshMaterial[i].uv.scale.x,
					skinningModel->param_->meshMaterial[i].uv.scale.y
				};
				meshJsonItem["uvRotate"] = skinningModel->param_->meshMaterial[i].uv.radius;
				meshJsonItem["environment"] = skinningModel->param_->meshMaterial[i].environment;
				meshJsonItem["shininess"] = skinningModel->param_->meshMaterial[i].shininess;
				meshJsonItem["enableLighting"] = skinningModel->param_->meshMaterial[i].enableLighting;
				meshJsonItem["enableDiffuse"] = skinningModel->param_->meshMaterial[i].enableDiffuse;
				meshJsonItem["enableHalfLambert"] = skinningModel->param_->meshMaterial[i].enableHalfLambert;
				meshJsonItem["enableSpecular"] = skinningModel->param_->meshMaterial[i].enableSpecular;
				meshJsonItem["enableBlinnPhong"] = skinningModel->param_->meshMaterial[i].enableBlinnPhong;
				meshJsonItem["drawShadowMap"] = skinningModel->param_->meshMaterial[i].drawShadowMap;
				meshJsonItem["enableShadow"] = skinningModel->param_->meshMaterial[i].enableShadow;

				// ブラー
				meshJsonItem["blurAfterImageMask"] = skinningModel->param_->meshBlur[i].afterImageMask;
				meshJsonItem["blurMotionBlurMask"] = skinningModel->param_->meshBlur[i].motionBlurMask;

				// アウトライン
				meshJsonItem["outlineEnable"] = skinningModel->param_->meshOutline[i].enableOutline;
				meshJsonItem["outlineColor"] = {
					skinningModel->param_->meshOutline[i].color.x,
					skinningModel->param_->meshOutline[i].color.y,
					skinningModel->param_->meshOutline[i].color.z,
					skinningModel->param_->meshOutline[i].color.w
				};

				// メッシュごとのJSONを追加
				meshJson.push_back(meshJsonItem);
			}

			// メッシュデータを要素のJSONに追加
			elemJson["meshData"] = meshJson;
		}
		else if (elem.render3D && elem.type == Engine::Render3D::Type::UVSphere)
		{
			auto uvSphere = static_cast<Render3DUVSphere*>(elem.render3D.get());

			// ブレンドモード
			elemJson["blendMode"] = static_cast<int>(uvSphere->param_->blendMode);

			// テクスチャパスを取得
			std::string texturePath = GrowthEngine::GetInstance()->GetTextureFilePath(uvSphere->param_->material.hTexture);
			if (!texturePath.empty())
			{
				elemJson["texturePath"] = texturePath;
			}

			// トランスフォーム
			elemJson["translate"] = {
				uvSphere->param_->transform.translate.x,
				uvSphere->param_->transform.translate.y,
				uvSphere->param_->transform.translate.z
			};
			elemJson["scale"] = {
				uvSphere->param_->transform.scale.x,
				uvSphere->param_->transform.scale.y,
				uvSphere->param_->transform.scale.z
			};
			elemJson["rotate"] = {
				uvSphere->param_->transform.rotate.x,
				uvSphere->param_->transform.rotate.y,
				uvSphere->param_->transform.rotate.z
			};

			// マテリアル
			elemJson["color"] = {
				uvSphere->param_->material.color.x,
				uvSphere->param_->material.color.y,
				uvSphere->param_->material.color.z,
				uvSphere->param_->material.color.w
			};
			elemJson["uvTranslate"] = {
				uvSphere->param_->material.uv.translate.x,
				uvSphere->param_->material.uv.translate.y
			};
			elemJson["uvScale"] = {
				uvSphere->param_->material.uv.scale.x,
				uvSphere->param_->material.uv.scale.y
			};
			elemJson["uvRotate"] = uvSphere->param_->material.uv.radius;
			elemJson["environment"] = uvSphere->param_->material.environment;
			elemJson["shininess"] = uvSphere->param_->material.shininess;
			elemJson["enableLighting"] = uvSphere->param_->material.enableLighting;
			elemJson["enableDiffuse"] = uvSphere->param_->material.enableDiffuse;
			elemJson["enableHalfLambert"] = uvSphere->param_->material.enableHalfLambert;
			elemJson["enableSpecular"] = uvSphere->param_->material.enableSpecular;
			elemJson["enableBlinnPhong"] = uvSphere->param_->material.enableBlinnPhong;
			elemJson["drawShadowMap"] = uvSphere->param_->material.drawShadowMap;
			elemJson["enableShadow"] = uvSphere->param_->material.enableShadow;

			// 分割
			elemJson["rings"] = uvSphere->param_->division.rings;
			elemJson["slices"] = uvSphere->param_->division.slices;

			// ブラー
			elemJson["blurAfterImageMask"] = uvSphere->param_->blur.afterImageMask;
			elemJson["blurMotionBlurMask"] = uvSphere->param_->blur.motionBlurMask;

			// アウトライン
			elemJson["outlineEnable"] = uvSphere->param_->outline.enableOutline;
			elemJson["outlineColor"] = {
				uvSphere->param_->outline.color.x,
				uvSphere->param_->outline.color.y,
				uvSphere->param_->outline.color.z,
				uvSphere->param_->outline.color.w
			};
		}
		else if (elem.render3D && elem.type == Engine::Render3D::Type::Ring)
		{
			auto ring = static_cast<Render3DRing*>(elem.render3D.get());

			// ブレンドモード
			elemJson["blendMode"] = static_cast<int>(ring->param_->blendMode);

			// テクスチャパスを取得
			std::string texturePath = GrowthEngine::GetInstance()->GetTextureFilePath(ring->param_->material.hTexture);
			if (!texturePath.empty())
			{
				elemJson["texturePath"] = texturePath;
			}

			// トランスフォーム
			elemJson["translate"] = {
				ring->param_->transform.translate.x,
				ring->param_->transform.translate.y,
				ring->param_->transform.translate.z
			};
			elemJson["scale"] = {
				ring->param_->transform.scale.x,
				ring->param_->transform.scale.y,
				ring->param_->transform.scale.z
			};
			elemJson["rotate"] = {
				ring->param_->transform.rotate.x,
				ring->param_->transform.rotate.y,
				ring->param_->transform.rotate.z
			};

			// マテリアル
			elemJson["color"] = {
				ring->param_->material.color.x,
				ring->param_->material.color.y,
				ring->param_->material.color.z,
				ring->param_->material.color.w
			};
			elemJson["uvTranslate"] = {
				ring->param_->material.uv.translate.x,
				ring->param_->material.uv.translate.y
			};
			elemJson["uvScale"] = {
				ring->param_->material.uv.scale.x,
				ring->param_->material.uv.scale.y
			};
			elemJson["uvRotate"] = ring->param_->material.uv.radius;
			elemJson["environment"] = ring->param_->material.environment;
			elemJson["shininess"] = ring->param_->material.shininess;
			elemJson["enableLighting"] = ring->param_->material.enableLighting;
			elemJson["enableDiffuse"] = ring->param_->material.enableDiffuse;
			elemJson["enableHalfLambert"] = ring->param_->material.enableHalfLambert;
			elemJson["enableSpecular"] = ring->param_->material.enableSpecular;
			elemJson["enableBlinnPhong"] = ring->param_->material.enableBlinnPhong;
			elemJson["drawShadowMap"] = ring->param_->material.drawShadowMap;
			elemJson["enableShadow"] = ring->param_->material.enableShadow;

			// 分割
			elemJson["slices"] = ring->param_->division.slices;

			// サイズ
			elemJson["startAngle"] = ring->param_->size.startAngle;
			elemJson["endAngle"] = ring->param_->size.endAngle;
			elemJson["startInRadius"] = ring->param_->size.startInRadius;
			elemJson["startOutRadius"] = ring->param_->size.startOutRadius;
			elemJson["endInRadius"] = ring->param_->size.endInRadius;
			elemJson["endOutRadius"] = ring->param_->size.endOutRadius;

			// ブラー
			elemJson["blurAfterImageMask"] = ring->param_->blur.afterImageMask;
			elemJson["blurMotionBlurMask"] = ring->param_->blur.motionBlurMask;

			// アウトライン
			elemJson["outlineEnable"] = ring->param_->outline.enableOutline;
			elemJson["outlineColor"] = {
				ring->param_->outline.color.x,
				ring->param_->outline.color.y,
				ring->param_->outline.color.z,
				ring->param_->outline.color.w
			};
		}
		else if (elem.render3D && elem.type == Engine::Render3D::Type::Cylinder)
		{
			auto cylinder = static_cast<Render3DCylinder*>(elem.render3D.get());

			// ブレンドモード
			elemJson["blendMode"] = static_cast<int>(cylinder->param_->blendMode);

			// テクスチャパスを取得
			std::string texturePath = GrowthEngine::GetInstance()->GetTextureFilePath(cylinder->param_->material.hTexture);
			if (!texturePath.empty())
			{
				elemJson["texturePath"] = texturePath;
			}

			// トランスフォーム
			elemJson["translate"] = {
				cylinder->param_->transform.translate.x,
				cylinder->param_->transform.translate.y,
				cylinder->param_->transform.translate.z
			};
			elemJson["scale"] = {
				cylinder->param_->transform.scale.x,
				cylinder->param_->transform.scale.y,
				cylinder->param_->transform.scale.z
			};
			elemJson["rotate"] = {
				cylinder->param_->transform.rotate.x,
				cylinder->param_->transform.rotate.y,
				cylinder->param_->transform.rotate.z
			};

			// マテリアル
			elemJson["color"] = {
				cylinder->param_->material.color.x,
				cylinder->param_->material.color.y,
				cylinder->param_->material.color.z,
				cylinder->param_->material.color.w
			};
			elemJson["uvTranslate"] = {
				cylinder->param_->material.uv.translate.x,
				cylinder->param_->material.uv.translate.y
			};
			elemJson["uvScale"] = {
				cylinder->param_->material.uv.scale.x,
				cylinder->param_->material.uv.scale.y
			};
			elemJson["uvRotate"] = cylinder->param_->material.uv.radius;
			elemJson["environment"] = cylinder->param_->material.environment;
			elemJson["shininess"] = cylinder->param_->material.shininess;
			elemJson["enableLighting"] = cylinder->param_->material.enableLighting;
			elemJson["enableDiffuse"] = cylinder->param_->material.enableDiffuse;
			elemJson["enableHalfLambert"] = cylinder->param_->material.enableHalfLambert;
			elemJson["enableSpecular"] = cylinder->param_->material.enableSpecular;
			elemJson["enableBlinnPhong"] = cylinder->param_->material.enableBlinnPhong;
			elemJson["drawShadowMap"] = cylinder->param_->material.drawShadowMap;
			elemJson["enableShadow"] = cylinder->param_->material.enableShadow;

			// 分割
			elemJson["slices"] = cylinder->param_->division.slices;

			// サイズ
			elemJson["topRadius"] = cylinder->param_->size.topRadius;
			elemJson["bottomRadius"] = cylinder->param_->size.bottomRadius;
			elemJson["height"] = cylinder->param_->size.height;

			// ブラー
			elemJson["blurAfterImageMask"] = cylinder->param_->blur.afterImageMask;
			elemJson["blurMotionBlurMask"] = cylinder->param_->blur.motionBlurMask;

			// アウトライン
			elemJson["outlineEnable"] = cylinder->param_->outline.enableOutline;
			elemJson["outlineColor"] = {
				cylinder->param_->outline.color.x,
				cylinder->param_->outline.color.y,
				cylinder->param_->outline.color.z,
				cylinder->param_->outline.color.w
			};
		}

		j.push_back(elemJson);
	}
	return j;
}

/// @brief JSONデータからモデル要素のリストを復元する
/// @param 
/// @return 
inline std::vector<ModelElementData> FromJsonData(const json& j,
	const std::map<std::pair<std::string, std::string>, ModelHandle>& loadedModels,
	const std::map<std::pair<std::string, std::string>, AnimationHandle>& loadedAnimations,
	const std::map<std::pair<std::string, std::string>, SkeletonHandle>& loadedSkeletons)
{
	std::vector<ModelElementData> elements;

	for (const auto& elemJson : j)
	{
		ModelElementData elem;
		elem.modelName = elemJson.value("modelName", "");
		elem.modelDirectory = elemJson.value("modelDirectory", "");
		elem.modelFileName = elemJson.value("modelFileName", "");
		elem.animationDirectory = elemJson.value("animationDirectory", "");
		elem.animationFileName = elemJson.value("animationFileName", "");
		elem.skeletonDirectory = elemJson.value("skeletonDirectory", "");
		elem.skeletonFileName = elemJson.value("skeletonFileName", "");
		elem.type = static_cast<Engine::Render3D::Type>(elemJson.value("type", 0));

		// なしの場合はスキップ
		if (elem.type == Engine::Render3D::Type::None)continue;

		// 3D描画オブジェクトを生成
		if (elem.type == Engine::Render3D::Type::StaticModel)
		{
			ModelHandle hModel = 0;
			auto itModel = loadedModels.find({ elem.modelDirectory, elem.modelFileName });
			if (itModel != loadedModels.end()) hModel = itModel->second;

			elem.render3D = std::make_unique<Render3DStaticModel>(hModel, elem.modelName);
			auto staticModel = static_cast<Render3DStaticModel*>(elem.render3D.get());
			
			// ブレンドモードを復元
			staticModel->param_->blendMode = static_cast<BlendMode>(elemJson.value("blendMode", 0));

			// トランスフォームを復元
			staticModel->param_->modelTransform.translate.x = elemJson.value("translate", std::vector<float>{0.0f, 0.0f, 0.0f})[0];
			staticModel->param_->modelTransform.translate.y = elemJson.value("translate", std::vector<float>{0.0f, 0.0f, 0.0f})[1];
			staticModel->param_->modelTransform.translate.z = elemJson.value("translate", std::vector<float>{0.0f, 0.0f, 0.0f})[2];

			staticModel->param_->modelTransform.scale.x = elemJson.value("scale", std::vector<float>{1.0f, 1.0f, 1.0f})[0];
			staticModel->param_->modelTransform.scale.y = elemJson.value("scale", std::vector<float>{1.0f, 1.0f, 1.0f})[1];
			staticModel->param_->modelTransform.scale.z = elemJson.value("scale", std::vector<float>{1.0f, 1.0f, 1.0f})[2];

			staticModel->param_->modelTransform.rotate.x = elemJson.value("rotate", std::vector<float>{0.0f, 0.0f, 0.0f})[0];
			staticModel->param_->modelTransform.rotate.y = elemJson.value("rotate", std::vector<float>{0.0f, 0.0f, 0.0f})[1];
			staticModel->param_->modelTransform.rotate.z = elemJson.value("rotate", std::vector<float>{0.0f, 0.0f, 0.0f})[2];

			// メッシュデータを復元
			size_t meshCount = elemJson["meshData"].size();
			staticModel->param_->meshTransforms.resize(meshCount);
			staticModel->param_->meshMaterial.resize(meshCount);
			staticModel->param_->meshBlur.resize(meshCount);
			staticModel->param_->meshOutline.resize(meshCount);

			
			for (int i = 0; i < static_cast<int>(staticModel->param_->meshTransforms.size()); ++i)
			{
				staticModel->param_->meshTransforms[i].translate.x = elemJson["meshData"][i].value("translate", std::vector<float>{0.0f, 0.0f, 0.0f})[0];
				staticModel->param_->meshTransforms[i].translate.y = elemJson["meshData"][i].value("translate", std::vector<float>{0.0f, 0.0f, 0.0f})[1];
				staticModel->param_->meshTransforms[i].translate.z = elemJson["meshData"][i].value("translate", std::vector<float>{0.0f, 0.0f, 0.0f})[2];

				staticModel->param_->meshTransforms[i].scale.x = elemJson["meshData"][i].value("scale", std::vector<float>{1.0f, 1.0f, 1.0f})[0];
				staticModel->param_->meshTransforms[i].scale.y = elemJson["meshData"][i].value("scale", std::vector<float>{1.0f, 1.0f, 1.0f})[1];
				staticModel->param_->meshTransforms[i].scale.z = elemJson["meshData"][i].value("scale", std::vector<float>{1.0f, 1.0f, 1.0f})[2];

				staticModel->param_->meshTransforms[i].rotate.x = elemJson["meshData"][i].value("rotate", std::vector<float>{0.0f, 0.0f, 0.0f})[0];
				staticModel->param_->meshTransforms[i].rotate.y = elemJson["meshData"][i].value("rotate", std::vector<float>{0.0f, 0.0f, 0.0f})[1];
				staticModel->param_->meshTransforms[i].rotate.z = elemJson["meshData"][i].value("rotate", std::vector<float>{0.0f, 0.0f, 0.0f})[2];

				// テクスチャパスを取得
				std::string texturePath = elemJson["meshData"][i].value("texturePath", "");
				if (!texturePath.empty())
				{
					// パスからテクスチャをロード（または取得）し、新しいハンドルを発行する
					staticModel->param_->meshMaterial[i].hTexture = GrowthEngine::GetInstance()->LoadTexture(texturePath);
				}
				else
				{
					staticModel->param_->meshMaterial[i].hTexture = GrowthEngine::GetInstance()->LoadTexture("./Assets/Textures/white2x2.png");
				}

				// マテリアルを復元
				staticModel->param_->meshMaterial[i].color.x = elemJson["meshData"][i].value("color", std::vector<float>{1.0f, 1.0f, 1.0f, 1.0f})[0];
				staticModel->param_->meshMaterial[i].color.y = elemJson["meshData"][i].value("color", std::vector<float>{1.0f, 1.0f, 1.0f, 1.0f})[1];
				staticModel->param_->meshMaterial[i].color.z = elemJson["meshData"][i].value("color", std::vector<float>{1.0f, 1.0f, 1.0f, 1.0f})[2];
				staticModel->param_->meshMaterial[i].color.w = elemJson["meshData"][i].value("color", std::vector<float>{1.0f, 1.0f, 1.0f, 1.0f})[3];

				staticModel->param_->meshMaterial[i].uv.translate.x = elemJson["meshData"][i].value("uvTranslate", std::vector<float>{0.0f, 0.0f})[0];
				staticModel->param_->meshMaterial[i].uv.translate.y = elemJson["meshData"][i].value("uvTranslate", std::vector<float>{0.0f, 0.0f})[1];


				staticModel->param_->meshMaterial[i].uv.scale = {
					elemJson["meshData"][i].value("uvScale", std::vector<float>{1.0f, 1.0f})[0],
					elemJson["meshData"][i].value("uvScale", std::vector<float>{1.0f, 1.0f})[1]
				};

				staticModel->param_->meshMaterial[i].uv.radius = elemJson["meshData"][i].value("uvRotate", 0.0f);
				staticModel->param_->meshMaterial[i].environment = elemJson["meshData"][i].value("environment", 0.0f);
				staticModel->param_->meshMaterial[i].shininess = elemJson["meshData"][i].value("shininess", 0.0f);
				staticModel->param_->meshMaterial[i].enableLighting = elemJson["meshData"][i].value("enableLighting", false);
				staticModel->param_->meshMaterial[i].enableDiffuse = elemJson["meshData"][i].value("enableDiffuse", false);
				staticModel->param_->meshMaterial[i].enableHalfLambert = elemJson["meshData"][i].value("enableHalfLambert", false);
				staticModel->param_->meshMaterial[i].enableSpecular = elemJson["meshData"][i].value("enableSpecular", false);
				staticModel->param_->meshMaterial[i].enableBlinnPhong = elemJson["meshData"][i].value("enableBlinnPhong", false);
				staticModel->param_->meshMaterial[i].drawShadowMap = elemJson["meshData"][i].value("drawShadowMap", false);
				staticModel->param_->meshMaterial[i].enableShadow = elemJson["meshData"][i].value("enableShadow", false);

				// ブラーを復元
				staticModel->param_->meshBlur[i].afterImageMask = elemJson["meshData"][i].value("blurAfterImageMask", 0.0f);
				staticModel->param_->meshBlur[i].motionBlurMask = elemJson["meshData"][i].value("blurMotionBlurMask", 0.0f);

				// アウトラインを復元
				staticModel->param_->meshOutline[i].enableOutline = elemJson["meshData"][i].value("outlineEnable", false);
				staticModel->param_->meshOutline[i].color = {
					elemJson["meshData"][i].value("outlineColor", std::vector<float>{0.0f, 0.0f, 0.0f, 1.0f})[0],
					elemJson["meshData"][i].value("outlineColor", std::vector<float>{0.0f, 0.0f, 0.0f, 1.0f})[1],
					elemJson["meshData"][i].value("outlineColor", std::vector<float>{0.0f, 0.0f, 0.0f, 1.0f})[2],
					elemJson["meshData"][i].value("outlineColor", std::vector<float>{0.0f, 0.0f, 0.0f, 1.0f})[3]
				};
			}
		}
		else if (elem.type == Engine::Render3D::Type::AnimationModel)
		{
			// モデルハンドルを取得
			ModelHandle hModel = 0;
			auto itModel = loadedModels.find({ elem.modelDirectory, elem.modelFileName });
			if (itModel != loadedModels.end()) hModel = itModel->second;

			// アニメーションハンドルを取得
			AnimationHandle hAnimation = 0;
			auto itAnimation = loadedAnimations.find({ elem.animationDirectory, elem.animationFileName });
			if (itAnimation != loadedAnimations.end()) hAnimation = itAnimation->second;

			elem.render3D = std::make_unique<Render3DAnimationModel>(hModel, hAnimation, elem.modelName);
			auto animationModel = static_cast<Render3DAnimationModel*>(elem.render3D.get());

			// ブレンドモードを復元
			animationModel->param_->blendMode = static_cast<BlendMode>(elemJson.value("blendMode", 0));

			// トランスフォームを復元
			animationModel->param_->modelTransform.translate = {
				elemJson.value("translate", std::vector<float>{0.0f, 0.0f, 0.0f})[0],
				elemJson.value("translate", std::vector<float>{0.0f, 0.0f, 0.0f})[1],
				elemJson.value("translate", std::vector<float>{0.0f, 0.0f, 0.0f})[2]
			};
			animationModel->param_->modelTransform.scale = {
				elemJson.value("scale", std::vector<float>{1.0f, 1.0f, 1.0f})[0],
				elemJson.value("scale", std::vector<float>{1.0f, 1.0f, 1.0f})[1],
				elemJson.value("scale", std::vector<float>{1.0f, 1.0f, 1.0f})[2]
			};
			animationModel->param_->modelTransform.rotate = {
				elemJson.value("rotate", std::vector<float>{0.0f, 0.0f, 0.0f})[0],
				elemJson.value("rotate", std::vector<float>{0.0f, 0.0f, 0.0f})[1],
				elemJson.value("rotate", std::vector<float>{0.0f, 0.0f, 0.0f})[2]
			};

			// メッシュデータを復元
			size_t meshCount = elemJson["meshData"].size();
			animationModel->param_->meshTransforms.resize(meshCount);
			animationModel->param_->meshMaterial.resize(meshCount);
			animationModel->param_->meshBlur.resize(meshCount);
			animationModel->param_->meshOutline.resize(meshCount);

			for (int i = 0; i < static_cast<int>(animationModel->param_->meshTransforms.size()); ++i)
			{
				// メッシュトランスフォームを復元
				animationModel->param_->meshTransforms[i].translate = {
					elemJson["meshData"][i].value("translate", std::vector<float>{0.0f, 0.0f, 0.0f})[0],
					elemJson["meshData"][i].value("translate", std::vector<float>{0.0f, 0.0f, 0.0f})[1],
					elemJson["meshData"][i].value("translate", std::vector<float>{0.0f, 0.0f, 0.0f})[2]
				};
				animationModel->param_->meshTransforms[i].scale = {
					elemJson["meshData"][i].value("scale", std::vector<float>{1.0f, 1.0f, 1.0f})[0],
					elemJson["meshData"][i].value("scale", std::vector<float>{1.0f, 1.0f, 1.0f})[1],
					elemJson["meshData"][i].value("scale", std::vector<float>{1.0f, 1.0f, 1.0f})[2]
				};
				animationModel->param_->meshTransforms[i].rotate = {
					elemJson["meshData"][i].value("rotate", std::vector<float>{0.0f, 0.0f, 0.0f})[0],
					elemJson["meshData"][i].value("rotate", std::vector<float>{0.0f, 0.0f, 0.0f})[1],
					elemJson["meshData"][i].value("rotate", std::vector<float>{0.0f, 0.0f, 0.0f})[2]
				};

				// テクスチャパスを取得
				std::string texturePath = elemJson["meshData"][i].value("texturePath", "");
				if (!texturePath.empty())
				{
					// パスからテクスチャをロード（または取得）し、新しいハンドルを発行する
					animationModel->param_->meshMaterial[i].hTexture = GrowthEngine::GetInstance()->LoadTexture(texturePath);
				}
				else
				{
					animationModel->param_->meshMaterial[i].hTexture = GrowthEngine::GetInstance()->LoadTexture("./Assets/Textures/white2x2.png");
				}

				// マテリアルを復元
				animationModel->param_->meshMaterial[i].color = {
					elemJson["meshData"][i].value("color", std::vector<float>{1.0f, 1.0f, 1.0f, 1.0f})[0],
					elemJson["meshData"][i].value("color", std::vector<float>{1.0f, 1.0f, 1.0f, 1.0f})[1],
					elemJson["meshData"][i].value("color", std::vector<float>{1.0f, 1.0f, 1.0f, 1.0f})[2],
					elemJson["meshData"][i].value("color", std::vector<float>{1.0f, 1.0f, 1.0f, 1.0f})[3]
				};
				animationModel->param_->meshMaterial[i].uv.translate = {
					elemJson["meshData"][i].value("uvTranslate", std::vector<float>{0.0f, 0.0f})[0],
					elemJson["meshData"][i].value("uvTranslate", std::vector<float>{0.0f, 0.0f})[1]
				};
				animationModel->param_->meshMaterial[i].uv.scale = {
					elemJson["meshData"][i].value("uvScale", std::vector<float>{1.0f, 1.0f})[0],
					elemJson["meshData"][i].value("uvScale", std::vector<float>{1.0f, 1.0f})[1]
				};
				animationModel->param_->meshMaterial[i].uv.radius = elemJson["meshData"][i].value("uvRotate", 0.0f);
				animationModel->param_->meshMaterial[i].environment = elemJson["meshData"][i].value("environment", 0.0f);
				animationModel->param_->meshMaterial[i].shininess = elemJson["meshData"][i].value("shininess", 0.0f);
				animationModel->param_->meshMaterial[i].enableLighting = elemJson["meshData"][i].value("enableLighting", false);
				animationModel->param_->meshMaterial[i].enableDiffuse = elemJson["meshData"][i].value("enableDiffuse", false);
				animationModel->param_->meshMaterial[i].enableHalfLambert = elemJson["meshData"][i].value("enableHalfLambert", false);
				animationModel->param_->meshMaterial[i].enableSpecular = elemJson["meshData"][i].value("enableSpecular", false);
				animationModel->param_->meshMaterial[i].enableBlinnPhong = elemJson["meshData"][i].value("enableBlinnPhong", false);
				animationModel->param_->meshMaterial[i].drawShadowMap = elemJson["meshData"][i].value("drawShadowMap", false);
				animationModel->param_->meshMaterial[i].enableShadow = elemJson["meshData"][i].value("enableShadow", false);

				// ブラーを復元
				animationModel->param_->meshBlur[i].afterImageMask = elemJson["meshData"][i].value("blurAfterImageMask", 0.0f);
				animationModel->param_->meshBlur[i].motionBlurMask = elemJson["meshData"][i].value("blurMotionBlurMask", 0.0f);

				// アウトラインを復元
				animationModel->param_->meshOutline[i].enableOutline = elemJson["meshData"][i].value("outlineEnable", false);
				animationModel->param_->meshOutline[i].color = {
					elemJson["meshData"][i].value("outlineColor", std::vector<float>{0.0f, 0.0f, 0.0f, 1.0f})[0],
					elemJson["meshData"][i].value("outlineColor", std::vector<float>{0.0f, 0.0f, 0.0f, 1.0f})[1],
					elemJson["meshData"][i].value("outlineColor", std::vector<float>{0.0f, 0.0f, 0.0f, 1.0f})[2],
					elemJson["meshData"][i].value("outlineColor", std::vector<float>{0.0f, 0.0f, 0.0f, 1.0f})[3]
				};
			}
		}
		else if (elem.type == Engine::Render3D::Type::SkinningModel)
		{
			// モデルハンドルを取得
			ModelHandle hModel = 0;
			auto itModel = loadedModels.find({ elem.modelDirectory, elem.modelFileName });
			if (itModel != loadedModels.end()) hModel = itModel->second;

			// アニメーションハンドルを取得
			AnimationHandle hAnimation = 0;
			auto itAnimation = loadedAnimations.find({ elem.animationDirectory, elem.animationFileName });
			if (itAnimation != loadedAnimations.end()) hAnimation = itAnimation->second;

			// スケルトンハンドルを取得
			SkeletonHandle hSkeleton = 0;
			auto itSkeleton = loadedSkeletons.find({ elem.skeletonDirectory, elem.skeletonFileName });
			if (itSkeleton != loadedSkeletons.end()) hSkeleton = itSkeleton->second;

			elem.render3D = std::make_unique<Render3DSkinningModel>(hModel, hAnimation, hSkeleton, elem.modelName);
			auto skinningModel = static_cast<Render3DSkinningModel*>(elem.render3D.get());

			// ブレンドモードを復元
			skinningModel->param_->blendMode = static_cast<BlendMode>(elemJson.value("blendMode", 0));

			// トランスフォームを復元
			skinningModel->param_->modelTransform.translate = {
				elemJson.value("translate", std::vector<float>{0.0f, 0.0f, 0.0f})[0],
				elemJson.value("translate", std::vector<float>{0.0f, 0.0f, 0.0f})[1],
				elemJson.value("translate", std::vector<float>{0.0f, 0.0f, 0.0f})[2]
			};
			skinningModel->param_->modelTransform.scale = {
				elemJson.value("scale", std::vector<float>{1.0f, 1.0f, 1.0f})[0],
				elemJson.value("scale", std::vector<float>{1.0f, 1.0f, 1.0f})[1],
				elemJson.value("scale", std::vector<float>{1.0f, 1.0f, 1.0f})[2]
			};
			skinningModel->param_->modelTransform.rotate = {
				elemJson.value("rotate", std::vector<float>{0.0f, 0.0f, 0.0f})[0],
				elemJson.value("rotate", std::vector<float>{0.0f, 0.0f, 0.0f})[1],
				elemJson.value("rotate", std::vector<float>{0.0f, 0.0f, 0.0f})[2]
			};

			// メッシュデータを復元
			size_t meshCount = elemJson["meshData"].size();
			skinningModel->param_->meshTransforms.resize(meshCount);
			skinningModel->param_->meshMaterial.resize(meshCount);
			skinningModel->param_->meshBlur.resize(meshCount);
			skinningModel->param_->meshOutline.resize(meshCount);

			for (int i = 0; i < static_cast<int>(skinningModel->param_->meshTransforms.size()); ++i)
			{
				// メッシュトランスフォームを復元
				skinningModel->param_->meshTransforms[i].translate = {
					elemJson["meshData"][i].value("translate", std::vector<float>{0.0f, 0.0f, 0.0f})[0],
					elemJson["meshData"][i].value("translate", std::vector<float>{0.0f, 0.0f, 0.0f})[1],
					elemJson["meshData"][i].value("translate", std::vector<float>{0.0f, 0.0f, 0.0f})[2]
				};
				skinningModel->param_->meshTransforms[i].scale = {
					elemJson["meshData"][i].value("scale", std::vector<float>{1.0f, 1.0f, 1.0f})[0],
					elemJson["meshData"][i].value("scale", std::vector<float>{1.0f, 1.0f, 1.0f})[1],
					elemJson["meshData"][i].value("scale", std::vector<float>{1.0f, 1.0f, 1.0f})[2]
				};
				skinningModel->param_->meshTransforms[i].rotate = {
					elemJson["meshData"][i].value("rotate", std::vector<float>{0.0f, 0.0f, 0.0f})[0],
					elemJson["meshData"][i].value("rotate", std::vector<float>{0.0f, 0.0f, 0.0f})[1],
					elemJson["meshData"][i].value("rotate", std::vector<float>{0.0f, 0.0f, 0.0f})[2]
				};

				// テクスチャパスを取得
				std::string texturePath = elemJson["meshData"][i].value("texturePath", "");
				if (!texturePath.empty())
				{
					// パスからテクスチャをロード（または取得）し、新しいハンドルを発行する
					skinningModel->param_->meshMaterial[i].hTexture = GrowthEngine::GetInstance()->LoadTexture(texturePath);
				}
				else
				{
					skinningModel->param_->meshMaterial[i].hTexture = GrowthEngine::GetInstance()->LoadTexture("./Assets/Textures/white2x2.png");
				}

				// マテリアルを復元
				skinningModel->param_->meshMaterial[i].color = {
					elemJson["meshData"][i].value("color", std::vector<float>{1.0f, 1.0f, 1.0f, 1.0f})[0],
					elemJson["meshData"][i].value("color", std::vector<float>{1.0f, 1.0f, 1.0f, 1.0f})[1],
					elemJson["meshData"][i].value("color", std::vector<float>{1.0f, 1.0f, 1.0f, 1.0f})[2],
					elemJson["meshData"][i].value("color", std::vector<float>{1.0f, 1.0f, 1.0f, 1.0f})[3]
				};
				skinningModel->param_->meshMaterial[i].uv.translate = {
					elemJson["meshData"][i].value("uvTranslate", std::vector<float>{0.0f, 0.0f})[0],
					elemJson["meshData"][i].value("uvTranslate", std::vector<float>{0.0f, 0.0f})[1]
				};
				skinningModel->param_->meshMaterial[i].uv.scale = {
					elemJson["meshData"][i].value("uvScale", std::vector<float>{1.0f, 1.0f})[0],
					elemJson["meshData"][i].value("uvScale", std::vector<float>{1.0f, 1.0f})[1]
				};
				skinningModel->param_->meshMaterial[i].uv.radius = elemJson["meshData"][i].value("uvRotate", 0.0f);
				skinningModel->param_->meshMaterial[i].environment = elemJson["meshData"][i].value("environment", 0.0f);
				skinningModel->param_->meshMaterial[i].shininess = elemJson["meshData"][i].value("shininess", 0.0f);
				skinningModel->param_->meshMaterial[i].enableLighting = elemJson["meshData"][i].value("enableLighting", false);
				skinningModel->param_->meshMaterial[i].enableDiffuse = elemJson["meshData"][i].value("enableDiffuse", false);
				skinningModel->param_->meshMaterial[i].enableHalfLambert = elemJson["meshData"][i].value("enableHalfLambert", false);
				skinningModel->param_->meshMaterial[i].enableSpecular = elemJson["meshData"][i].value("enableSpecular", false);
				skinningModel->param_->meshMaterial[i].enableBlinnPhong = elemJson["meshData"][i].value("enableBlinnPhong", false);
				skinningModel->param_->meshMaterial[i].drawShadowMap = elemJson["meshData"][i].value("drawShadowMap", false);
				skinningModel->param_->meshMaterial[i].enableShadow = elemJson["meshData"][i].value("enableShadow", false);

				// ブラーを復元
				skinningModel->param_->meshBlur[i].afterImageMask = elemJson["meshData"][i].value("blurAfterImageMask", 0.0f);
				skinningModel->param_->meshBlur[i].motionBlurMask = elemJson["meshData"][i].value("blurMotionBlurMask", 0.0f);

				// アウトラインを復元
				skinningModel->param_->meshOutline[i].enableOutline = elemJson["meshData"][i].value("outlineEnable", false);
				skinningModel->param_->meshOutline[i].color = {
					elemJson["meshData"][i].value("outlineColor", std::vector<float>{0.0f, 0.0f, 0.0f, 1.0f})[0],
					elemJson["meshData"][i].value("outlineColor", std::vector<float>{0.0f, 0.0f, 0.0f, 1.0f})[1],
					elemJson["meshData"][i].value("outlineColor", std::vector<float>{0.0f, 0.0f, 0.0f, 1.0f})[2],
					elemJson["meshData"][i].value("outlineColor", std::vector<float>{0.0f, 0.0f, 0.0f, 1.0f})[3]
				};
			}
		}
		else if (elem.type == Engine::Render3D::Type::UVSphere)
		{
			elem.render3D = std::make_unique<Render3DUVSphere>(elem.modelName);
			auto uvSphere = static_cast<Render3DUVSphere*>(elem.render3D.get());

			// ブレンドモードを復元
			uvSphere->param_->blendMode = static_cast<BlendMode>(elemJson.value("blendMode", 0));

			// トランスフォームを復元
			uvSphere->param_->transform.translate = {
				elemJson.value("translate", std::vector<float>{0.0f, 0.0f, 0.0f})[0],
				elemJson.value("translate", std::vector<float>{0.0f, 0.0f, 0.0f})[1],
				elemJson.value("translate", std::vector<float>{0.0f, 0.0f, 0.0f})[2]
			};
			uvSphere->param_->transform.scale = {
				elemJson.value("scale", std::vector<float>{1.0f, 1.0f, 1.0f})[0],
				elemJson.value("scale", std::vector<float>{1.0f, 1.0f, 1.0f})[1],
				elemJson.value("scale", std::vector<float>{1.0f, 1.0f, 1.0f})[2]
			};
			uvSphere->param_->transform.rotate = {
				elemJson.value("rotate", std::vector<float>{0.0f, 0.0f, 0.0f})[0],
				elemJson.value("rotate", std::vector<float>{0.0f, 0.0f, 0.0f})[1],
				elemJson.value("rotate", std::vector<float>{0.0f, 0.0f, 0.0f})[2]
			};

			// テクスチャパスを取得
			std::string texturePath = elemJson.value("texturePath", "");
			if (!texturePath.empty())
			{
				// パスからテクスチャをロード（または取得）し、新しいハンドルを発行する
				uvSphere->param_->material.hTexture = GrowthEngine::GetInstance()->LoadTexture(texturePath);
			}
			else
			{
				uvSphere->param_->material.hTexture = GrowthEngine::GetInstance()->LoadTexture("./Assets/Textures/white2x2.png");
			}

			// マテリアルを復元
			uvSphere->param_->material.color = {
				elemJson.value("color", std::vector<float>{1.0f, 1.0f, 1.0f, 1.0f})[0],
				elemJson.value("color", std::vector<float>{1.0f, 1.0f, 1.0f, 1.0f})[1],
				elemJson.value("color", std::vector<float>{1.0f, 1.0f, 1.0f, 1.0f})[2],
				elemJson.value("color", std::vector<float>{1.0f, 1.0f, 1.0f, 1.0f})[3]
			};
			uvSphere->param_->material.uv.translate = {
				elemJson.value("uvTranslate", std::vector<float>{0.0f, 0.0f})[0],
				elemJson.value("uvTranslate", std::vector<float>{0.0f, 0.0f})[1]
			};
			uvSphere->param_->material.uv.scale = {
				elemJson.value("uvScale", std::vector<float>{1.0f, 1.0f})[0],
				elemJson.value("uvScale", std::vector<float>{1.0f, 1.0f})[1]
			};
			uvSphere->param_->material.uv.radius = elemJson.value("uvRotate", 0.0f);
			uvSphere->param_->material.environment = elemJson.value("environment", 0.0f);
			uvSphere->param_->material.shininess = elemJson.value("shininess", 0.0f);
			uvSphere->param_->material.enableLighting = elemJson.value("enableLighting", false);
			uvSphere->param_->material.enableDiffuse = elemJson.value("enableDiffuse", false);
			uvSphere->param_->material.enableHalfLambert = elemJson.value("enableHalfLambert", false);
			uvSphere->param_->material.enableSpecular = elemJson.value("enableSpecular", false);
			uvSphere->param_->material.enableBlinnPhong = elemJson.value("enableBlinnPhong", false);
			uvSphere->param_->material.drawShadowMap = elemJson.value("drawShadowMap", false);
			uvSphere->param_->material.enableShadow = elemJson.value("enableShadow", false);

			// 分割を復元
			uvSphere->param_->division.rings = elemJson.value("rings", 0);
			uvSphere->param_->division.slices = elemJson.value("slices", 0);

			// ブラーを復元
			uvSphere->param_->blur.afterImageMask = elemJson.value("blurAfterImageMask", 0.0f);
			uvSphere->param_->blur.motionBlurMask = elemJson.value("blurMotionBlurMask", 0.0f);

			// アウトラインを復元
			uvSphere->param_->outline.enableOutline = elemJson.value("outlineEnable", false);
			uvSphere->param_->outline.color = {
				elemJson.value("outlineColor", std::vector<float>{0.0f, 0.0f, 0.0f, 1.0f})[0],
				elemJson.value("outlineColor", std::vector<float>{0.0f, 0.0f, 0.0f, 1.0f})[1],
				elemJson.value("outlineColor", std::vector<float>{0.0f, 0.0f, 0.0f, 1.0f})[2],
				elemJson.value("outlineColor", std::vector<float>{0.0f, 0.0f, 0.0f, 1.0f})[3]
			};
		}
		else if (elem.type == Engine::Render3D::Type::Ring)
		{
			elem.render3D = std::make_unique<Render3DRing>(elem.modelName);
			auto ring = static_cast<Render3DRing*>(elem.render3D.get());

			// ブレンドモードを復元
			ring->param_->blendMode = static_cast<BlendMode>(elemJson.value("blendMode", 0));

			// トランスフォームを復元
			ring->param_->transform.translate = {
				elemJson.value("translate", std::vector<float>{0.0f, 0.0f, 0.0f})[0],
				elemJson.value("translate", std::vector<float>{0.0f, 0.0f, 0.0f})[1],
				elemJson.value("translate", std::vector<float>{0.0f, 0.0f, 0.0f})[2]
			};
			ring->param_->transform.scale = {
				elemJson.value("scale", std::vector<float>{1.0f, 1.0f, 1.0f})[0],
				elemJson.value("scale", std::vector<float>{1.0f, 1.0f, 1.0f})[1],
				elemJson.value("scale", std::vector<float>{1.0f, 1.0f, 1.0f})[2]
			};
			ring->param_->transform.rotate = {
				elemJson.value("rotate", std::vector<float>{0.0f, 0.0f, 0.0f})[0],
				elemJson.value("rotate", std::vector<float>{0.0f, 0.0f, 0.0f})[1],
				elemJson.value("rotate", std::vector<float>{0.0f, 0.0f, 0.0f})[2]
			};

			// テクスチャパスを取得
			std::string texturePath = elemJson.value("texturePath", "");
			if (!texturePath.empty())
			{
				// パスからテクスチャをロード（または取得）し、新しいハンドルを発行する
				ring->param_->material.hTexture = GrowthEngine::GetInstance()->LoadTexture(texturePath);
			}
			else
			{
				ring->param_->material.hTexture = GrowthEngine::GetInstance()->LoadTexture("./Assets/Textures/white2x2.png");
			}

			// マテリアルを復元
			ring->param_->material.color = {
				elemJson.value("color", std::vector<float>{1.0f, 1.0f, 1.0f, 1.0f})[0],
				elemJson.value("color", std::vector<float>{1.0f, 1.0f, 1.0f, 1.0f})[1],
				elemJson.value("color", std::vector<float>{1.0f, 1.0f, 1.0f, 1.0f})[2],
				elemJson.value("color", std::vector<float>{1.0f, 1.0f, 1.0f, 1.0f})[3]
			};
			ring->param_->material.uv.translate = {
				elemJson.value("uvTranslate", std::vector<float>{0.0f, 0.0f})[0],
				elemJson.value("uvTranslate", std::vector<float>{0.0f, 0.0f})[1]
			};
			ring->param_->material.uv.scale = {
				elemJson.value("uvScale", std::vector<float>{1.0f, 1.0f})[0],
				elemJson.value("uvScale", std::vector<float>{1.0f, 1.0f})[1]
			};
			ring->param_->material.uv.radius = elemJson.value("uvRotate", 0.0f);
			ring->param_->material.environment = elemJson.value("environment", 0.0f);
			ring->param_->material.shininess = elemJson.value("shininess", 0.0f);
			ring->param_->material.enableLighting = elemJson.value("enableLighting", false);
			ring->param_->material.enableDiffuse = elemJson.value("enableDiffuse", false);
			ring->param_->material.enableHalfLambert = elemJson.value("enableHalfLambert", false);
			ring->param_->material.enableSpecular = elemJson.value("enableSpecular", false);
			ring->param_->material.enableBlinnPhong = elemJson.value("enableBlinnPhong", false);
			ring->param_->material.drawShadowMap = elemJson.value("drawShadowMap", false);
			ring->param_->material.enableShadow = elemJson.value("enableShadow", false);

			// 分割を復元
			ring->param_->division.slices = elemJson.value("slices", 0);

			// サイズを復元
			ring->param_->size.startAngle = elemJson.value("startAngle", 0.0f);
			ring->param_->size.endAngle = elemJson.value("endAngle", 0.0f);
			ring->param_->size.startInRadius = elemJson.value("startInRadius", 0.0f);
			ring->param_->size.startOutRadius = elemJson.value("startOutRadius", 0.0f);
			ring->param_->size.endInRadius = elemJson.value("endInRadius", 0.0f);
			ring->param_->size.endOutRadius = elemJson.value("endOutRadius", 0.0f);

			// ブラーを復元
			ring->param_->blur.afterImageMask = elemJson.value("blurAfterImageMask", 0.0f);
			ring->param_->blur.motionBlurMask = elemJson.value("blurMotionBlurMask", 0.0f);

			// アウトラインを復元
			ring->param_->outline.enableOutline = elemJson.value("outlineEnable", false);
			ring->param_->outline.color = {
				elemJson.value("outlineColor", std::vector<float>{0.0f, 0.0f, 0.0f, 1.0f})[0],
				elemJson.value("outlineColor", std::vector<float>{0.0f, 0.0f, 0.0f, 1.0f})[1],
				elemJson.value("outlineColor", std::vector<float>{0.0f, 0.0f, 0.0f, 1.0f})[2],
				elemJson.value("outlineColor", std::vector<float>{0.0f, 0.0f, 0.0f, 1.0f})[3]
			};
		}
		else if (elem.type == Engine::Render3D::Type::Cylinder)
		{
			elem.render3D = std::make_unique<Render3DCylinder>(elem.modelName);
			auto cylinder = static_cast<Render3DCylinder*>(elem.render3D.get());

			// ブレンドモードを復元
			cylinder->param_->blendMode = static_cast<BlendMode>(elemJson.value("blendMode", 0));

			// トランスフォームを復元
			cylinder->param_->transform.translate = {
				elemJson.value("translate", std::vector<float>{0.0f, 0.0f, 0.0f})[0],
				elemJson.value("translate", std::vector<float>{0.0f, 0.0f, 0.0f})[1],
				elemJson.value("translate", std::vector<float>{0.0f, 0.0f, 0.0f})[2]
			};
			cylinder->param_->transform.scale = {
				elemJson.value("scale", std::vector<float>{1.0f, 1.0f, 1.0f})[0],
				elemJson.value("scale", std::vector<float>{1.0f, 1.0f, 1.0f})[1],
				elemJson.value("scale", std::vector<float>{1.0f, 1.0f, 1.0f})[2]
			};
			cylinder->param_->transform.rotate = {
				elemJson.value("rotate", std::vector<float>{0.0f, 0.0f, 0.0f})[0],
				elemJson.value("rotate", std::vector<float>{0.0f, 0.0f, 0.0f})[1],
				elemJson.value("rotate", std::vector<float>{0.0f, 0.0f, 0.0f})[2]
			};

			// テクスチャパスを取得
			std::string texturePath = elemJson.value("texturePath", "");
			if (!texturePath.empty())
			{
				// パスからテクスチャをロード（または取得）し、新しいハンドルを発行する
				cylinder->param_->material.hTexture = GrowthEngine::GetInstance()->LoadTexture(texturePath);
			}
			else
			{
				cylinder->param_->material.hTexture = GrowthEngine::GetInstance()->LoadTexture("./Assets/Textures/white2x2.png");
			}

			// マテリアルを復元
			cylinder->param_->material.color = {
				elemJson.value("color", std::vector<float>{1.0f, 1.0f, 1.0f, 1.0f})[0],
				elemJson.value("color", std::vector<float>{1.0f, 1.0f, 1.0f, 1.0f})[1],
				elemJson.value("color", std::vector<float>{1.0f, 1.0f, 1.0f, 1.0f})[2],
				elemJson.value("color", std::vector<float>{1.0f, 1.0f, 1.0f, 1.0f})[3]
			};
			cylinder->param_->material.uv.translate = {
				elemJson.value("uvTranslate", std::vector<float>{0.0f, 0.0f})[0],
				elemJson.value("uvTranslate", std::vector<float>{0.0f, 0.0f})[1]
			};
			cylinder->param_->material.uv.scale = {
				elemJson.value("uvScale", std::vector<float>{1.0f, 1.0f})[0],
				elemJson.value("uvScale", std::vector<float>{1.0f, 1.0f})[1]
			};
			cylinder->param_->material.uv.radius = elemJson.value("uvRotate", 0.0f);
			cylinder->param_->material.environment = elemJson.value("environment", 0.0f);
			cylinder->param_->material.shininess = elemJson.value("shininess", 0.0f);
			cylinder->param_->material.enableLighting = elemJson.value("enableLighting", false);
			cylinder->param_->material.enableDiffuse = elemJson.value("enableDiffuse", false);
			cylinder->param_->material.enableHalfLambert = elemJson.value("enableHalfLambert", false);
			cylinder->param_->material.enableSpecular = elemJson.value("enableSpecular", false);
			cylinder->param_->material.enableBlinnPhong = elemJson.value("enableBlinnPhong", false);
			cylinder->param_->material.drawShadowMap = elemJson.value("drawShadowMap", false);
			cylinder->param_->material.enableShadow = elemJson.value("enableShadow", false);

			// 分割を復元
			cylinder->param_->division.slices = elemJson.value("slices", 0);

			// サイズを復元
			cylinder->param_->size.topRadius = elemJson.value("topRadius", 0.0f);
			cylinder->param_->size.bottomRadius = elemJson.value("bottomRadius", 0.0f);
			cylinder->param_->size.height = elemJson.value("height", 0.0f);

			// ブラーを復元
			cylinder->param_->blur.afterImageMask = elemJson.value("blurAfterImageMask", 0.0f);
			cylinder->param_->blur.motionBlurMask = elemJson.value("blurMotionBlurMask", 0.0f);

			// アウトラインを復元
			cylinder->param_->outline.enableOutline = elemJson.value("outlineEnable", false);
			cylinder->param_->outline.color = {
				elemJson.value("outlineColor", std::vector<float>{0.0f, 0.0f, 0.0f, 0.0f})[0],
				elemJson.value("outlineColor", std::vector<float>{0.0f, 0.0f, 0.0f, 0.0f})[1],
				elemJson.value("outlineColor", std::vector<float>{0.0f, 0.0f, 0.0f, 0.0f})[2],
				elemJson.value("outlineColor", std::vector<float>{0.0f, 0.0f, 0.0f, 0.0f})[3]
			};
		}

		elements.push_back(std::move(elem));
	}
	return elements;
}

/// @brief モデル要素のデータをJSON形式で保存する
/// @param filePath 
/// @param elements 
inline void ToJson(const std::string& filePath, const std::vector<ModelElementData>& elements)
{
	std::ofstream file(filePath);
	if (file.is_open()) file << ToJsonData(elements).dump(4);
}

/// @brief JSONからモデル要素のデータを読み込む
/// @param filePath 
/// @param loadedTextures 
/// @return 
inline std::vector<ModelElementData> FromJson(const std::string& filePath,
	const std::map<std::pair<std::string, std::string>, ModelHandle>& loadedModels,
	const std::map<std::pair<std::string, std::string>, AnimationHandle>& loadedAnimations,
	const std::map<std::pair<std::string, std::string>, SkeletonHandle>& loadedSkeletons)
{
	std::ifstream file(filePath);
	if (file.is_open())
	{
		json j;
		file >> j;
		auto elements = FromJsonData(j, loadedModels, loadedAnimations, loadedSkeletons);

		file.close();

		return elements;
	}
	return {};
}