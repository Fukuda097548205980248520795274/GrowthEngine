#include "PrefabBaseStaticModel.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param hTexture 
/// @param numInstance 
/// @param name 
PrefabBaseStaticModel::PrefabBaseStaticModel(ModelHandle hModel, uint32_t numInstance, const std::string& name)
	: BasePrefabBase(name)
{
	// 読み込む
	handle_ = engine_->LoadPrefabPrimitive(name_, Engine::Prefab::Type::StaticModel, numInstance, 0, hModel, 0, 0);

	// パラメータを取得する
	param_ = engine_->GetPrefabPrimitiveParam<Engine::Prefab::StaticModel::Base::Param>(handle_);
}

/// @brief インスタンスを生成する
[[nodiscard]]
PrefabInstanceStaticModel* PrefabBaseStaticModel::CreateInstance()
{
	return engine_->CreatePrimitiveInstance<PrefabInstanceStaticModel>(handle_);
}