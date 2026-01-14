#include "DX12Offscreen.h"
#include "Log/Log.h"
#include "RenderContext/DX12Heap/DX12Heap.h"
#include "RenderContext/DX12Buffering/DX12Buffering.h"
#include <cassert>

/// @brief 初期化
/// @param device 
/// @param heap 
/// @param buffering 
/// @param log 
void Engine::DX12Offscreen::Initialize(ID3D12Device* device, DX12Heap* heap, DX12Buffering* buffering, Log* log)
{
	// nullptrチェック
	assert(device);
	assert(heap);

	// オフスクリーンのリソースを生成する
	for (int32_t i = 0; i < kMaxOffscreenCount; ++i)
	{
		offscreenResource_[i] = std::make_unique<OffscreenResource>();
		offscreenResource_[i]->Initialize(device, buffering, heap, log);
	}

	// 深度リソースを生成する
	depthResource_ = std::make_unique<DepthResource>();
	depthResource_->Initialize(device, buffering, heap, log);
}

/// @brief クリア
/// @param commandList 
void Engine::DX12Offscreen::Clear(ID3D12GraphicsCommandList* commandList)
{
	// 値を初期化
	currentOffscreen_ = 0;

	// オフスクリーンのレンダーターゲット・デプスステンシルの設定とクリア
	ClearRenderTarget(commandList);
	ClearDepthStencil(commandList);
}

/// @brief レンダーターゲットのクリア
/// @param commandList 
void Engine::DX12Offscreen::ClearRenderTarget(ID3D12GraphicsCommandList* commandList)
{
	// レンダーターゲットのクリアと設定
	offscreenResource_[currentOffscreen_]->ClearRenderTarget(commandList, depthResource_->GetDsvCpuHandle());
}

/// @brief デプスステンシルのクリア
/// @param commandList 
void Engine::DX12Offscreen::ClearDepthStencil(ID3D12GraphicsCommandList* commandList)
{
	// てぷスステンシルのクリア
	depthResource_->ClearDepthStencil(commandList);
}