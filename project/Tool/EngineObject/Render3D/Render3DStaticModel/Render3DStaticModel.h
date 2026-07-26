#pragma once
#include "../BaseRender3D.h"

class Render3DStaticModel : public Engine::BaseRender3D
{
public:

	/// @brief コンストラクタ
	/// @param hModel
	/// @param name 
	Render3DStaticModel(ModelHandle hModel, const std::string& name);

	/// @brief 描画処理
	void Draw() override;

	/// @brief パラメータ
	Engine::Render3D::StaticModel::Param* param_ = nullptr;
};

