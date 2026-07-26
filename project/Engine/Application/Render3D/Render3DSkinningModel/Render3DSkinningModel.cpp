#include "Render3DSkinningModel.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param modelHandle 
/// @param name 
Render3DSkinningModel::Render3DSkinningModel(ModelHandle hModel, AnimationHandle hAnimation, SkeletonHandle hSkeleton, const std::string& name) : BaseRender3D(name)
{
	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

	// 種別名
	type_ = Engine::Render3D::Type::SkinningModel;

	// 読み込み
	hRender3D_ = engine_->LoadRender3D(hModel, hAnimation, hSkeleton, name_, type_);

	// パラメータを取得する
	param_ = engine_->GetRender3DParam<Engine::Render3D::SkinningModel::Param>(hRender3D_);
}

/// @brief ボーンのワールド行列を取得する
/// @param boneName 
/// @return 
Matrix4x4 Render3DSkinningModel::GetBoneWorldMatrix(const std::string& boneName)
{
	return engine_->GetBoneWorldMatrix(hRender3D_, boneName);
}

/// @brief 描画処理
void Render3DSkinningModel::Draw()
{
	engine_->DrawRender3D(hRender3D_);
}