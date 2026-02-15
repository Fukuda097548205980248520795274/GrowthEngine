#include "PrimitiveAnimationModel.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param modelHandle 
/// @param name 
PrimitiveAnimationModel::PrimitiveAnimationModel(ModelHandle hModel, AnimationHandle hAnimation, const std::string& name)
	: hModel_(hModel), hAnimation_(hAnimation), name_(name)
{
	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

	// 種別名
	type_ = "AnimationModel";

	// 読み込み
	hPrimitive_ = engine_->LoadPrimitive(hModel_, hAnimation_,0, name_, type_);
	param_ = engine_->GetPrimitiveParam<Engine::AnimationModel::Param>(hPrimitive_);
}

/// @brief 描画処理
void PrimitiveAnimationModel::Draw()
{
	engine_->DrawPrimitiveModel(hPrimitive_);
}