#include "PrimitiveStaticModel.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param modelHandle 
/// @param name 
PrimitiveStaticModel::PrimitiveStaticModel(ModelHandle hModel, const std::string& name)
	: hModel_(hModel) , name_(name)
{
	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

	// 種別名
	type_ = "StaticModel";

	// 読み込み
	hPrimitive_ = engine_->LoadPrimitive(hModel_,0,0, name_, type_);
	param_ = engine_->GetPrimitiveParam<Engine::Primitive::StaticModel::Param>(hPrimitive_);
}

/// @brief 描画処理
void PrimitiveStaticModel::Draw()
{
	engine_->DrawPrimitiveModel(hPrimitive_);
}