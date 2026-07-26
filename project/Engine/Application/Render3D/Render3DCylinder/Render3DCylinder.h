#pragma once
#include "../BaseRender3D.h"

class Render3DCylinder : public Engine::BaseRender3D
{
public:

	/// @brief コンストラクタ
	/// @param hModel
	/// @param name 
	Render3DCylinder(const std::string& name);

	/// @brief 描画処理
	void Draw() override;

	/// @brief パラメータ
	Engine::Render3D::Cylinder::Param* param_ = nullptr;
};

