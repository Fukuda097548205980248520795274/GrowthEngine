#pragma once
#include <memory>
#include <string>
#include "Handle/Handle.h"
#include "Data/CameraData/CameraData.h"

class GameCamera3D
{
public:

	/// @brief コンストラクタ
	GameCamera3D(const std::string& name);

	/// @brief カメラを切り替える
	void Switch();

	/// @brief パラメータ
	Engine::Camera3DData::Param* param_ = nullptr;


private:

	// 名前
	std::string name_;

	// カメラハンドル
	Camera3DHandle hCamera_ = 0;
};

