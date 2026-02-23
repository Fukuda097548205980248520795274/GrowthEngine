#include "PrimitiveStaticModel.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param modelHandle 
/// @param name 
PrimitiveStaticModel::PrimitiveStaticModel(ModelHandle hModel, const std::string& name) : BasePrimitive(name)
{
	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

	// 種別名
	type_ = Engine::Primitive::Type::StaticModel;

	// 読み込み
	hPrimitive_ = engine_->LoadPrimitive(hModel,0,0, name_, type_);

	// パラメータを取得する
	param_ = engine_->GetPrimitiveParam<Engine::Primitive::StaticModel::Param>(hPrimitive_);
}

/// @brief 描画処理
void PrimitiveStaticModel::Draw()
{
	engine_->DrawPrimitive(hPrimitive_);
}