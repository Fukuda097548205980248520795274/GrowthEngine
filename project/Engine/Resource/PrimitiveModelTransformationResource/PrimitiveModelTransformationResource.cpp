#include "PrimitiveModelTransformationResource.h"
#include "Func/ResourceFunc/ResourceFunc.h"
#include <cassert>

/// @brief 初期化
/// @param device 
/// @param log 
void Engine::PrimitiveModelTransformationResource::Initialize(ID3D12Device* device, Log* log)
{
	// nullptrチェック
	assert(device);

	// リソースの生成
	resource_ = CreateBufferResource(device, sizeof(PrimitiveModelTransformationDataForGPU), log);

	// データ割り当て
	resource_->Map(0, nullptr, reinterpret_cast<void**>(&data_));
	data_->worldViewProjectionMatrix = MakeIdentityMatrix4x4();
	data_->worldMatrix = MakeIdentityMatrix4x4();
	data_->worldInverseTransposeMatrix = MakeIdentityMatrix4x4();
}

/// @brief コマンドリストに登録する
/// @param commandList 
/// @param rootParameterIndex 
void Engine::PrimitiveModelTransformationResource::Register(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex)
{
	commandList->SetGraphicsRootConstantBufferView(rootParameterIndex, resource_->GetGPUVirtualAddress());
}