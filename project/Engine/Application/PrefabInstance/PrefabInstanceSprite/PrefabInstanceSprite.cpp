#include "PrefabInstanceSprite.h"

/// @brief コンストラクタ
/// @param drawCall 
PrefabInstanceSprite::PrefabInstanceSprite(std::function<void(const Engine::Prefab::Sprite::Instance::Param*)> drawCall, 
	Engine::Prefab::Sprite::Base::Param* param)
	: drawCall_(drawCall)
{
	param_.transform.scale = param->transform.scale;
	param_.transform.rotate = param->transform.rotate;
	param_.transform.translate = param->transform.translate;

	param_.material.color = param->material.color;
	param_.material.uv.scale = param->material.uv.scale;
	param_.material.uv.rotate = param->material.uv.rotate;
	param_.material.uv.translate = param->material.uv.translate;

	param_.texture.anchor = param->texture.anchor;
	param_.texture.size = param->texture.size;
}

/// @brief 描画処理
void PrefabInstanceSprite::Draw()
{
	// ドロー
	drawCall_(&param_);
}