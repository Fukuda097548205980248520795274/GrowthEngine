#include "PrefabInstanceStaticModel.h"

/// @brief コンストラクタ
/// @param drawCall 
PrefabInstanceStaticModel::PrefabInstanceStaticModel(std::function<void(const Engine::Prefab3D::StaticModel::Instance::Param*)> drawCall,
	Engine::Prefab3D::StaticModel::Base::Param* param)
	: drawCall_(drawCall)
{
	param_.modelTransform.scale = param->modelTransform.scale;
	param_.modelTransform.rotate = param->modelTransform.rotate;
	param_.modelTransform.translate = param->modelTransform.translate;

	// 領域確保
	param_.meshMaterial.resize(static_cast<int32_t>(param->meshMaterial.size()));
	param_.meshTransforms.resize(static_cast<int32_t>(param->meshMaterial.size()));
	param_.meshBlur.resize(static_cast<int32_t>(param->meshMaterial.size()));

	for (int32_t meshIndex = 0; meshIndex < static_cast<int32_t>(param->meshMaterial.size()); ++meshIndex)
	{
		// メッシュトランスフォーム
		param_.meshTransforms[meshIndex].scale = param->meshTransforms[meshIndex].scale;
		param_.meshTransforms[meshIndex].rotate = param->meshTransforms[meshIndex].rotate;
		param_.meshTransforms[meshIndex].translate = param->meshTransforms[meshIndex].translate;

		// メッシュマテリアル
		param_.meshMaterial[meshIndex].color = param->meshMaterial[meshIndex].color;
		param_.meshMaterial[meshIndex].uv.scale = param->meshMaterial[meshIndex].uv.scale;
		param_.meshMaterial[meshIndex].uv.radius = param->meshMaterial[meshIndex].uv.radius;
		param_.meshMaterial[meshIndex].uv.translate = param->meshMaterial[meshIndex].uv.translate;
		param_.meshMaterial[meshIndex].environment = param->meshMaterial[meshIndex].environment;
		param_.meshMaterial[meshIndex].shininess = param->meshMaterial[meshIndex].shininess;
		param_.meshMaterial[meshIndex].enableLighting = param->meshMaterial[meshIndex].enableLighting;
		param_.meshMaterial[meshIndex].enableDiffuse = param->meshMaterial[meshIndex].enableDiffuse;
		param_.meshMaterial[meshIndex].enableHalfLambert = param->meshMaterial[meshIndex].enableHalfLambert;
		param_.meshMaterial[meshIndex].enableSpecular = param->meshMaterial[meshIndex].enableSpecular;
		param_.meshMaterial[meshIndex].enableBlinnPhong = param->meshMaterial[meshIndex].enableBlinnPhong;
		param_.meshMaterial[meshIndex].enableShadow = param->meshMaterial[meshIndex].enableShadow;

		// ブラー
		param_.meshBlur[meshIndex].afterImageMask = param->meshBlur[meshIndex].afterImageMask;
		param_.meshBlur[meshIndex].motionBlurMask = param->meshBlur[meshIndex].motionBlurMask;
	}
}

/// @brief 描画処理
void PrefabInstanceStaticModel::Draw()
{
	// ドロー
	drawCall_(&param_);
}