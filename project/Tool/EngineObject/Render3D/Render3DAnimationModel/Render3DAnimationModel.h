#pragma once
#include "../BaseRender3D.h"

class Render3DAnimationModel : public Engine::BaseRender3D
{
public:

	/// @brief コンストラクタ
	/// @param hModel 
	/// @param hAnimation 
	/// @param name 
	Render3DAnimationModel(ModelHandle hModel,AnimationHandle hAnimation, const std::string& name);

	/// @brief 描画処理
	void Draw() override;

	/// @brief パラメータ
	Engine::Render3D::AnimationModel::Param* param_ = nullptr;
};

