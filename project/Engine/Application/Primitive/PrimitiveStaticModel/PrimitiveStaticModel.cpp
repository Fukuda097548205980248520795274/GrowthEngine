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

	hPrimitive_ = engine_->LoadPrimitive(this, hModel_, name_);
}

/// @brief 描画処理
void PrimitiveStaticModel::Draw()
{
	engine_->DrawPrimitiveStaticModel(hPrimitive_);
}