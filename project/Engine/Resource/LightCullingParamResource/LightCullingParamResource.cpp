#include "LightCullingParamResource.h"
#include <cassert>
#include "Log/Log.h"
#include "Func/ResourceFunc/ResourceFunc.h"

/// @brief 初期化
/// @param device 
/// @param log 
void Engine::LightCullingParamResource::Initialize(ID3D12Device* device, Log* log)
{
	// nullptrチェック
	assert(device);

	// リソースの生成
	resource_ = CreateBufferResource(device, sizeof(LightCullingParamForGPU), log);

	// データ割り当て
	resource_->Map(0, nullptr, reinterpret_cast<void**>(&data_));
}

/// @brief コマンドリストに登録する
/// @param commandList 
/// @param rootParameterIndex 
void Engine::LightCullingParamResource::Register(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex)
{
	commandList->SetComputeRootConstantBufferView(rootParameterIndex, resource_->GetGPUVirtualAddress());
}