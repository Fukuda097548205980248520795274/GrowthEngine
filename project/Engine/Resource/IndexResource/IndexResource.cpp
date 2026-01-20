#include "IndexResource.h"
#include <cassert>
#include "Func/ResourceFunc/ResourceFunc.h"


/// @brief 初期化
/// @param device デバイス
/// @param numSurface 面の数
void Engine::IndexResource::Initialize(ID3D12Device* device, uint32_t numSurface, Log* log)
{
	// nullptrチェック
	assert(device);
	assert(numSurface > 0);

	numVertex_ = numSurface * 6;

	// リソースを生成する
	resource_ = CreateBufferResource(device, sizeof(uint32_t) * numVertex_, log);

	// データを割り当てる
	resource_->Map(0, nullptr, reinterpret_cast<void**>(&data_));
}

/// @brief コマンドリストに登録する
/// @param commandList 
void Engine::IndexResource::Register(ID3D12GraphicsCommandList* commandList)
{
	commandList->IASetIndexBuffer(&view_);
}