#include "PrefabBaseTube.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param hTexture 
/// @param numInstance 
/// @param name 
PrefabBaseTube::PrefabBaseTube(TextureHandle hTexture, uint32_t numInstance, const std::string& name)
	: BasePrefabBase(name)
{
	// 読み込む
	handle_ = engine_->LoadPrefab3D(name_, Engine::Prefab3D::Type::Tube, numInstance, hTexture, 0, 0, 0);

	// パラメータを取得する
	param_ = engine_->GetPrefab3DParam<Engine::Prefab3D::Tube::Base::Param>(handle_);
}

/// @brief インスタンスを生成する
[[nodiscard]]
PrefabInstanceTube* PrefabBaseTube::CreateInstance()
{
	return engine_->CreatePrefab3DInstance<PrefabInstanceTube>(handle_);
}

/// @brief 描画処理
void PrefabBaseTube::Draw()
{
	engine_->DrawPrefab3D(handle_);
}