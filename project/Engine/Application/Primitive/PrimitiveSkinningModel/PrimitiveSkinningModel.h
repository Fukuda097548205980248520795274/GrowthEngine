#pragma once
#include "../BasePrimitive.h"

class PrimitiveSkinningModel : public Engine::BasePrimitive
{
public:

	/// @brief コンストラクタ
	/// @param hModel 
	/// @param hSkeleton 
	/// @param name 
	PrimitiveSkinningModel(ModelHandle hModel, AnimationHandle hAnimation, SkeletonHandle hSkeleton, const std::string& name);

	/// @brief 描画処理
	void Draw() override;

	/// @brief パラメータ
	Engine::Primitive::SkinningModel::Param* param_ = nullptr;
};

