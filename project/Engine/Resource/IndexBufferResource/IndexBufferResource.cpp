#include "IndexBufferResource.h"
#include <cassert>
#include "Func/ResourceFunc/ResourceFunc.h"


/// @brief 初期化
/// @param device デバイス
/// @param numIndex インデックス数
void Engine::IndexBufferResource::Initialize(ID3D12Device* device, uint32_t numIndex, Log* log)
{
	// nullptrチェック
	assert(device);
	assert(numIndex > 0);

	// リソースを生成する
	resource_ = CreateBufferResource(device, sizeof(uint32_t) * numIndex, log);

	// ビューの設定
	view_.BufferLocation = resource_->GetGPUVirtualAddress();
	view_.Format = DXGI_FORMAT_R32_UINT;
	view_.SizeInBytes = sizeof(uint32_t) * numIndex;

	// データを割り当てる
	resource_->Map(0, nullptr, reinterpret_cast<void**>(&data_));
}

/// @brief コマンドリストに登録する
/// @param commandList 
void Engine::IndexBufferResource::Register(ID3D12GraphicsCommandList* commandList)
{
	commandList->IASetIndexBuffer(&view_);
}