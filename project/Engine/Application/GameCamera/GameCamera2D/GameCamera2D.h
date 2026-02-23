#pragma once
#include "../BaseGameCamera.h"

class GameCamera2D : public Engine::BaseGameCamera
{
public:

	/// @brief コンストラクタ
	GameCamera2D(const std::string& name);

	/// @brief カメラを切り替える
	void Switch() override;

	/// @brief パラメータ
	Engine::Camera2DData::Param* param_ = nullptr;


private:

	// カメラハンドル
	Camera2DHandle hCamera_ = 0;
};

