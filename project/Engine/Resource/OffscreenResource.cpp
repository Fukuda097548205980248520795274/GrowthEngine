#include "OffscreenResource.h"
#include <cassert>
#include "Func/ResourceFunc/ResourceFunc.h"

/// @brief 初期化
/// @param device 
/// @param buffering 
/// @param heap 
/// @param log 
void Engine::OffscreenResource::Initialize(ID3D12Device* device, DX12Buffering* buffering, DX12Heap* heap, Log* log)
{
	// nullptrチェック
	assert(device);
	assert(buffering);
	assert(heap);

	// 書き込み可能なリソーステクスチャを生成する
	
}