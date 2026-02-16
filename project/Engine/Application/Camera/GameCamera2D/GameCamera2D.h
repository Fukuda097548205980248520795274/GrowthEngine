#pragma once
#include <memory>
#include <string>
#include "Handle/Handle.h"
#include "Data/CameraData/CameraData.h"

class GameCamera2D
{
public:

	/// @brief コンストラクタ
	GameCamera2D(const std::string& name);

	/// @brief カメラを切り替える
	void Switch();

	/// @brief パラメータ
	Engine::Camera2DData::Param* param_ = nullptr;


private:

	// 名前
	std::string name_;

	// カメラハンドル
	Camera2DHandle hCamera_ = 0;
};

