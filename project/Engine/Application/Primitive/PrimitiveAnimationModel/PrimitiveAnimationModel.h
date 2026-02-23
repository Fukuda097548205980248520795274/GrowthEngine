#pragma once
#include "../BasePrimitive.h"

class PrimitiveAnimationModel : public Engine::BasePrimitive
{
public:

	/// @brief コンストラクタ
	/// @param hModel 
	/// @param hAnimation 
	/// @param name 
	PrimitiveAnimationModel(ModelHandle hModel,AnimationHandle hAnimation, const std::string& name);

	/// @brief 描画処理
	void Draw();

	/// @brief パラメータ
	Engine::Primitive::AnimationModel::Param* param_ = nullptr;
};

