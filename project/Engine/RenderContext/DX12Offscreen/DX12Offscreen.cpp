#include "DX12Offscreen.h"
#include "Log/Log.h"
#include "RenderContext/DX12Heap/DX12Heap.h"
#include <cassert>

/// @brief 初期化
/// @param device 
/// @param heap 
/// @param log 
void Engine::DX12Offscreen::Initialize(ID3D12Device* device, DX12Heap* heap, Log* log)
{
	// nullptrチェック
	assert(device);
	assert(heap);

	// オフスクリーンのリソースを生成する
	for (int32_t i = 0; i < kMaxOffscreenCount; ++i)
	{

	}
}