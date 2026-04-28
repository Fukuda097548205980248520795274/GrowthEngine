#include "PrefabInstanceCube.h"

/// @brief コンストラクタ
/// @param drawCall 
PrefabInstanceCube::PrefabInstanceCube(std::function<void(const Engine::Prefab3D::Cube::Instance::Param*)> drawCall, Engine::Prefab3D::Cube::Base::Param* param)
	: drawCall_(drawCall)
{
	// トランスフォーム
	param_.transform.scale = param->transform.scale;
	param_.transform.rotate = param->transform.rotate;
	param_.transform.translate = param->transform.translate;

	// マテリアル
	param_.material.color = param->material.color;
	param_.material.uv.scale = param->material.uv.scale;
	param_.material.uv.radius = param->material.uv.radius;
	param_.material.uv.translate = param->material.uv.translate;
	param_.material.environment = param->material.environment;
	param_.material.shininess = param->material.shininess;
	param_.material.enableLighting = param->material.enableLighting;
	param_.material.enableDiffuse = param->material.enableDiffuse;
	param_.material.enableHalfLambert = param->material.enableHalfLambert;
	param_.material.enableSpecular = param->material.enableSpecular;
	param_.material.enableBlinnPhong = param->material.enableBlinnPhong;
	param_.material.enableShadow = param->material.enableShadow;
}

/// @brief 描画処理
void PrefabInstanceCube::Draw()
{
	// ドロー
	drawCall_(&param_);
}