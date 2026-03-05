#include "DepthResource.h"
#include "RenderContext/DX12Buffering/DX12Buffering.h"
#include "RenderContext/DX12Heap/DX12Heap.h"
#include "Log/Log.h"
#include <cassert>
#include "Func/ResourceFunc/ResourceFunc.h"

/// @brief 初期化
/// @param device 
/// @param buffering 
/// @param heap 
/// @param log 
void Engine::DepthResource::Initialize(ID3D12Device* device, DX12Buffering* buffering, DX12Heap* heap, Log* log)
{
	// nullptrチェック
	assert(device);
	assert(buffering);
	assert(heap);

	// 深度テクスチャリソースを生成する
	resource_ = CreateDepthStencilTextureResource(device, buffering->GetSwapChainDesc().Width, buffering->GetSwapChainDesc().Height, log);

	/*---------------
		DSVの設定
	---------------*/

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

	// ログ出力
	if (log)
	{
		log->Logging("DSV Format : D24_UNORM_S8_UINT");
		log->Logging("DSV ViewDimension : DIMENSION_TEXTURE2D");
		log->Logging("Creat DepthStencilView \n");
	}

	// CPUハンドルを取得する
	dsvCPUHandle_ = heap->GetDsvCPUDescriptorHandle();

	// ヒープの先頭にDSVを作る
	device->CreateDepthStencilView(resource_.Get(), &dsvDesc, dsvCPUHandle_);
}

/// @brief サイズを作り直す
/// @param device 
/// @param buffering 
void Engine::DepthResource::Resize(ID3D12Device* device, DX12Buffering* buffering)
{
	assert(device);
	assert(buffering);

	// リソース開放
	resource_.Reset();

	// 新たなサイズで再生成
	resource_ = CreateDepthStencilTextureResource(device, buffering->GetSwapChainDesc().Width, buffering->GetSwapChainDesc().Height, nullptr);

	// DSV 同じ設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

	// DSV再生成
	device->CreateDepthStencilView(resource_.Get(), &dsvDesc, dsvCPUHandle_);
}

/// @brief デプスステンシルのクリア
/// @param commandList 
void Engine::DepthResource::ClearDepthStencil(ID3D12GraphicsCommandList* commandList)
{
	// クリア
	commandList->ClearDepthStencilView(dsvCPUHandle_, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}