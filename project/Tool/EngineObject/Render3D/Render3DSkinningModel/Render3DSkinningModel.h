#pragma once
#include "../BaseRender3D.h"
#include "Math/Matrix/Matrix4x4/Matrix4x4.h"

class Render3DSkinningModel : public Engine::BaseRender3D
{
public:

	/// @brief コンストラクタ
	/// @param hModel 
	/// @param hSkeleton 
	/// @param name 
	Render3DSkinningModel(ModelHandle hModel, AnimationHandle hAnimation, SkeletonHandle hSkeleton, const std::string& name);

	/// @brief ボーンのワールド行列を取得する
	/// @param boneName 
	/// @return 
	Matrix4x4 GetBoneWorldMatrix(const std::string& boneName);

	/// @brief 描画処理
	void Draw() override;

	/// @brief パラメータ
	Engine::Render3D::SkinningModel::Param* param_ = nullptr;
};

