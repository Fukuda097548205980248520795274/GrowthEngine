#include "ShadowMapTextureResource.h"
#include "Func/ResourceFunc/ResourceFunc.h"
#include "RenderContext/DX12Heap/DX12Heap.h"
#include "Log/Log.h"
#include <cassert>

/// @brief 初期化
/// @param directXHeap 
/// @param device 
void Engine::ShadowMapTextureResource::Initialize(DX12Heap* heap, ID3D12Device* device, int32_t width, int32_t height, Log* log)
{
	// nullptrチェック
	assert(heap);
	assert(device);



	// シャドウマップのリソースを作成する
	resource_ = CreateShadowMapTextureResource(device, width, height, log);


	/*------------------
		DSVを設定する
	------------------*/

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};

	// 深度のみ
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

	// DSVハンドルを取得する
	dsvHandle_ = heap->GetDsvCPUDescriptorHandle();

	device->CreateDepthStencilView(resource_.Get(), &dsvDesc, dsvHandle_);

	// ログ出力
	if (log)
	{
		log->Logging("DSV Format : D24_UNORM_S8_UINT");
		log->Logging("DSV ViewDimension : DIMENSION_TEXTURE2D");
		log->Logging("DSV Flags : NONE");
		log->Logging("Creat DepthStencilView \n");
	}


	/*------------------
		SRVを設定する
	------------------*/

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};

	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;


	// SRVハンドルを取得する
	srvHandle_.first = heap->GetSrvCPUDescriptorHandle();
	srvHandle_.second = heap->GetSrvGPUDescriptorHandle();

	device->CreateShaderResourceView(resource_.Get(), &srvDesc, srvHandle_.first);

	// ログ出力
	if (log)
	{
		log->Logging("SRV Format : R24_UNORM_X8_TYPELES");
		log->Logging("SRV Shader4ComponentMapping : DEFAULT_SHADER_4_COMPONENT_MAPPING");
		log->Logging("SRV ViewDimension : DIMENSION_TEXTURE2D");
		log->Logging("SRV Flags : NONE");
		log->Logging("Creat ShaderResourceView \n");
	}
}

/// @brief デプスステンシルのクリア
/// @param commandList 
void Engine::ShadowMapTextureResource::ClearDepthStencil(ID3D12GraphicsCommandList* commandList)
{
	commandList->ClearDepthStencilView(dsvHandle_, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}