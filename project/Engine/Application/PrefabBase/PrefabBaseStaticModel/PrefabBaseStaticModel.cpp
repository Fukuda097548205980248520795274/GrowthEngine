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
	handle_ = engine_->LoadPrefab3D(name_, Engine::Prefab3D::Type::StaticModel, numInstance, 0, hModel, 0, 0);

	// パラメータを取得する
	param_ = engine_->GetPrefab3DParam<Engine::Prefab3D::StaticModel::Base::Param>(handle_);
}

/// @brief インスタンスを生成する
[[nodiscard]]
PrefabInstanceStaticModel* PrefabBaseStaticModel::CreateInstance()
{
	return engine_->CreatePrefab3DInstance<PrefabInstanceStaticModel>(handle_);
}

/// @brief 描画処理
void PrefabBaseStaticModel::Draw()
{
	engine_->DrawPrefab3D(handle_);
}