#include "Camera2DResource.h"

/// @brief コンストラクタ
/// @param name 
/// @param hCamera 
Engine::Camera2DResource::Camera2DResource(const std::string& name, Camera2DHandle hCamera)
	: name_(name), hCamera_(hCamera)
{
	// カメラの生成と初期化
	camera2d_ = std::make_unique<Camera2D>();
}

/// @brief 更新処理
void Engine::Camera2DResource::Update()
{
	// カメラの更新
	camera2d_->Update();
}