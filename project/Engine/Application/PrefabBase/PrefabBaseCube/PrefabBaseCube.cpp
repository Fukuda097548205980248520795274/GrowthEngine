#include "PrefabBaseCube.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param hTexture 
/// @param numInstance 
/// @param name 
PrefabBaseCube::PrefabBaseCube(TextureHandle hTexture, uint32_t numInstance, const std::string& name)
	: BasePrefabBase(name)
{
	// 読み込む
	handle_ = engine_->LoadPrefab3D(name_, Engine::Prefab3D::Type::Cube, numInstance, hTexture, 0, 0, 0);

	// パラメータを取得する
	param_ = engine_->GetPrefab3DParam<Engine::Prefab3D::Cube::Base::Param>(handle_);
}

/// @brief インスタンスを生成する
[[nodiscard]]
PrefabInstanceCube* PrefabBaseCube::CreateInstance()
{
	return engine_->CreatePrefab3DInstance<PrefabInstanceCube>(handle_);
}

/// @brief 描画処理
void PrefabBaseCube::Draw()
{
	engine_->DrawPrefab3D(handle_);
}