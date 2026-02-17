#include "PrimitiveSkinningModel.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param modelHandle 
/// @param name 
PrimitiveSkinningModel::PrimitiveSkinningModel(ModelHandle hModel, AnimationHandle hAnimation, SkeletonHandle hSkeleton, const std::string& name)
	: hModel_(hModel),hAnimation_(hAnimation), hSkeleton_(hSkeleton), name_(name)
{
	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

	// 種別名
	type_ = Engine::Primitive::Type::SkinningModel;

	// 読み込み
	hPrimitive_ = engine_->LoadPrimitive(hModel_, hAnimation_, hSkeleton_, name_, type_);
	param_ = engine_->GetPrimitiveParam<Engine::Primitive::SkinningModel::Param>(hPrimitive_);
}

/// @brief 描画処理
void PrimitiveSkinningModel::Draw()
{
	engine_->DrawPrimitive(hPrimitive_);
}