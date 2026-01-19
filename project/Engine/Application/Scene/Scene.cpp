#include "Scene.h"

/// @brief コンストラクタ
Scene::Scene()
{
	// 3Dカメラの生成と初期化
	camera3d_ = std::make_unique<Camera3D>();
}

/// @brief 初期化
void Scene::Initialize()
{
	// カメラの更新処理
	camera3d_->Update();
}

/// @brief 更新処理
void Scene::Update()
{

}

/// @brief 描画処理
void Scene::Draw()
{

}