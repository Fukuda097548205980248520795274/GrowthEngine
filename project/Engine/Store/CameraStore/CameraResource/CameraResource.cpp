#include "CameraResource.h"
#include "Application/Camera/GameCamera/GameCamera.h"

/// @brief コンストラクタ
/// @param name 
/// @param hCamera 
Engine::CameraResource::CameraResource(const std::string& name, CameraHandle hCamera)
	: name_(name), hCamera_(hCamera)
{
	// カメラの生成と初期化
	camera3d_ = std::make_unique<Camera3D>();
}

/// @brief 更新処理
void Engine::CameraResource::Update()
{
	// カメラの更新
	camera3d_->Update();
}

/// @brief データを反映させる
/// @param gameCamera 
void Engine::CameraResource::DataReflect(GameCamera* gameCamera)
{
	gameCamera->transform_.rotation = &camera3d_->rotate_;
	gameCamera->transform_.translate = &camera3d_->translate_;
}