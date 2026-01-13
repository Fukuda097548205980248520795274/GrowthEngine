#include "DX12Buffering.h"
#include <cassert>
#include <format>
#include "Log/Log.h"
#include "RenderContext/DX12Heap/DX12Heap.h"
#include "WinApp/WinApp.h"

/// @brief 初期化
/// @param log 
/// @param heap 
/// @param winApp 
/// @param dxgiFactory 
/// @param device 
/// @param commandQueue 
void Engine::DX12Buffering::Initialize(Log* log, DX12Heap* heap, WinApp* winApp,
	IDXGIFactory7* dxgiFactory, ID3D12Device* device, ID3D12CommandQueue* commandQueue)
{
	// nullptrチェック
	assert(log);
	assert(heap);
	assert(dxgiFactory);
	assert(device);
	assert(winApp);
	assert(commandQueue);


	/*-----------------------------
		スワップチェーンを生成する
	-----------------------------*/

	// クライアント領域
	swapChainDesc_.Width = winApp->GetClientWidth();
	swapChainDesc_.Height = winApp->GetClientHeight();

	// 色の形式
	swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// マルチサンプルしない
	swapChainDesc_.SampleDesc.Count = 1;

	// 描画のターゲットとして利用する
	swapChainDesc_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// ダブルバッファ
	swapChainDesc_.BufferCount = 2;

	// モニタに移したら中身を破棄
	swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	// 生成する
	HRESULT hr = dxgiFactory->CreateSwapChainForHwnd(commandQueue, winApp->GetHwnd(),
		&swapChainDesc_, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(swapChain_.GetAddressOf()));
	assert(SUCCEEDED(hr));

	// スワップチェーン生成成功のログ
	if (log)
	{
		log->Logging(std::format("clientSize : width = {} , height = {}", swapChainDesc_.Width, swapChainDesc_.Height));
		log->Logging("format : R8G8B8A8_UNORM");
		log->Logging(std::format("SampleDesc Count : {}", swapChainDesc_.SampleDesc.Count));
		log->Logging("BufferUsage : RENDER_TARGET_OUTPUT");
		log->Logging(std::format("BufferCount : {}", swapChainDesc_.BufferCount));
		log->Logging("SwapEffect : FLIP_DISCARD");
		log->Logging("CreateSwapChainForHwnd \n");
	}



	/*------------------------------------
		スワップチェーンのリソースを取得する
	------------------------------------*/

	// 1つめ
	hr = swapChain_->GetBuffer(0, IID_PPV_ARGS(&swapChainResource_[0]));
	assert(SUCCEEDED(hr));
	if (log)log->Logging("GetSwapChainBuffer : 0 \n");

	// 2つめ
	hr = swapChain_->GetBuffer(1, IID_PPV_ARGS(&swapChainResource_[1]));
	assert(SUCCEEDED(hr));
	if (log)log->Logging("GetSwapChainBuffer : 1 \n");



	/*-------------------
		RTVを作成する
	-------------------*/

	// 出力結果をSRGBに変換して書き込む
	rtvDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	// 2Dテクスチャとして書き込む
	rtvDesc_.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;


	// ログ出力
	if (log)
	{
		log->Logging("RTV Format : R8G8B8A8_UNORM_SRGB");
		log->Logging("RTV ViewDimension : DIMENSION_TEXTURE2D");
		log->Logging("CreateRenderTargetView \n");
	}


	// 1つめのリソースにRTVを設定する
	rtvCPUHandle_[0] = heap->GetRtvCPUDescriptorHandle();
	device->CreateRenderTargetView(swapChainResource_[0].Get(), &rtvDesc_, rtvCPUHandle_[0]);

	// 2つめのリソースにRTVを設定する
	rtvCPUHandle_[1] = heap->GetRtvCPUDescriptorHandle();
	device->CreateRenderTargetView(swapChainResource_[1].Get(), &rtvDesc_, rtvCPUHandle_[1]);
}