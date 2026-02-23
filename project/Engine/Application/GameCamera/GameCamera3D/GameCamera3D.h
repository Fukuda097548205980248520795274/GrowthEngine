#pragma once
#include "../BaseGameCamera.h"

class GameCamera3D : public Engine::BaseGameCamera
{
public:

	/// @brief コンストラクタ
	GameCamera3D(const std::string& name);

	/// @brief カメラを切り替える
	void Switch() override;

	/// @brief パラメータ
	Engine::Camera3DData::Param* param_ = nullptr;


private:

	// カメラハンドル
	Camera3DHandle hCamera_ = 0;
};

