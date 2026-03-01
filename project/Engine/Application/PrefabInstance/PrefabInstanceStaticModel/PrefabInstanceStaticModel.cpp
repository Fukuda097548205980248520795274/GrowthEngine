#include "PrefabInstanceStaticModel.h"

/// @brief コンストラクタ
/// @param drawCall 
PrefabInstanceStaticModel::PrefabInstanceStaticModel(std::function<void(const Engine::Prefab::StaticModel::Instance::Param*)> drawCall,
	Engine::Prefab::StaticModel::Base::Param* param)
	: drawCall_(drawCall)
{
	param_.modelTransform.scale = param->modelTransform.scale;
	param_.modelTransform.rotate = param->modelTransform.rotate;
	param_.modelTransform.translate = param->modelTransform.translate;

	// 領域確保
	param_.meshMaterial.resize(static_cast<int32_t>(param->meshMaterial.size()));
	param_.meshTransforms.resize(static_cast<int32_t>(param->meshMaterial.size()));

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
	}
}

/// @brief 描画処理
void PrefabInstanceStaticModel::Draw()
{
	// ドロー
	drawCall_(&param_);
}