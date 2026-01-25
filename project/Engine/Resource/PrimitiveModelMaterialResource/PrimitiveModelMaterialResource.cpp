#include "PrimitiveModelMaterialResource.h"
#include "Func/ResourceFunc/ResourceFunc.h"
#include <cassert>

/// @brief 初期化
/// @param device 
/// @param log 
void Engine::PrimitiveModelMaterialResource::Initialize(ID3D12Device* device, Log* log)
{
	// nullptrチェック
	assert(device);

	// リソースの生成
	resource_ = CreateBufferResource(device, sizeof(PrimitiveModelMaterialDataForGPU), log);

	// データ割り当て
	resource_->Map(0, nullptr, reinterpret_cast<void**>(&data_));

	// データ初期化
	data_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
}

/// @brief コマンドリストに登録する
/// @param commandList 
/// @param rootParameterIndex 
void Engine::PrimitiveModelMaterialResource::Register(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex)
{
	commandList->SetGraphicsRootConstantBufferView(rootParameterIndex, resource_->GetGPUVirtualAddress());
}