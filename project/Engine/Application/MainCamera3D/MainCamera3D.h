#pragma once
#include "Handle/Handle.h"
#include "Data/CameraData/CameraData.h"
#include <string>

class GrowthEngine;

class MainCamera3D
{
public:

	/// @brief コンストラクタ
	/// @param name 
	MainCamera3D(const std::string& name);

	/// @brief 切り替え
	void Switch();

	/// @brief パラメータ
	Engine::Camera3DData::Param* param_ = nullptr;


private:

	/// @brief エンジン
	GrowthEngine* engine_ = nullptr;

	/// @brief ハンドル
	Camera3DHandle handle_ = 0;

	/// @brief 名前
	std::string name_{};
};

