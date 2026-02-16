#include "Camera3DResource.h"

/// @brief コンストラクタ
/// @param name 
/// @param hCamera 
Engine::Camera3DResource::Camera3DResource(const std::string& name, Camera3DHandle hCamera)
	: name_(name), hCamera_(hCamera)
{
	// カメラの生成と初期化
	camera3d_ = std::make_unique<Camera3D>();
}

/// @brief 更新処理
void Engine::Camera3DResource::Update()
{
	// カメラの更新
	camera3d_->Update();
}