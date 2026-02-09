#include "DirectionalLightData.h"
#include <cassert>

/// @brief コンストラクタ
/// @param name 
/// @param handle 
Engine::DirectionalLightData::DirectionalLightData(const std::string& name, LightHandle handle) 
	: BaseLightData(name, handle) 
{
	// パラメータ生成
	param_ = std::make_unique<DirectionalLightParam>();
	param_->direction = Vector3(0.0f, -1.0f, 0.0f);
	param_->intensity = 1.0f;
	param_->color = Vector3(1.0f, 1.0f, 1.0f);
}

/// @brief 初期化
/// @param heap 
/// @param device 
/// @param log 
void Engine::DirectionalLightData::Initialize(DX12Heap* heap, ID3D12Device* device, Log* log)
{
	// nullptrチェック
	assert(heap);
	assert(device);

	// シャドウマップ用テクスチャリソースの生成と初期化
	shadowMapTextureResource_ = std::make_unique<ShadowMapTextureResource>();
	shadowMapTextureResource_->Initialize(heap, device, 1536, 1536, log);
}