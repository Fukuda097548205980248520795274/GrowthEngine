#include "ShadowMapTransformationResource.h"
#include "Func/ResourceFunc/ResourceFunc.h"
#include <cassert>

/// @brief 初期化
/// @param device 
/// @param log 
void Engine::ShadowMapTransformationResource::Initialize(ID3D12Device* device, Log* log)
{
	// nullptrチェック
	assert(device);

	// リソースの生成
	resource_ = CreateBufferResource(device, sizeof(Matrix4x4), log);

	// データ割り当て
	resource_->Map(0, nullptr, reinterpret_cast<void**>(&data_));
	*data_ = MakeIdentityMatrix4x4();
}

/// @brief コマンドリストに登録する
/// @param commandList 
/// @param rootParameterIndex 
void Engine::ShadowMapTransformationResource::Register(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex)
{
	commandList->SetGraphicsRootConstantBufferView(rootParameterIndex, resource_->GetGPUVirtualAddress());
}