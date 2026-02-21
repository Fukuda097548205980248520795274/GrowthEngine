#include "PrefabBaseStaticModel.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param hTexture 
/// @param numInstance 
/// @param name 
PrefabBaseStaticModel::PrefabBaseStaticModel(ModelHandle hModel, uint32_t numInstance, const std::string& name)	: name_(name)
{
	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

	// 読み込む
	handle_ = engine_->LoadPrefabPrimitive(name_, Engine::Prefab::Type::StaticModel, numInstance, hModel, 0, 0);

	// パラメータを取得する
	//param_ = engine_->GetPrefabSpriteParam(handle_);
}

/// @brief インスタンスを生成する
[[nodiscard]]
PrefabInstanceStaticModel* PrefabBaseStaticModel::CreateInstance()
{
	return engine_->CreatePrimitiveInstance<PrefabInstanceStaticModel>(handle_);
}

/// @brief 描画処理
void PrefabBaseStaticModel::Draw()
{
	engine_->DrawPrefabPrimitive(handle_);
}