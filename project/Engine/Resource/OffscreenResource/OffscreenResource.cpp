#include "OffscreenResource.h"
#include <cassert>
#include "Func/ResourceFunc/ResourceFunc.h"
#include "RenderContext/DX12Buffering/DX12Buffering.h"
#include "RenderContext/DX12Heap/DX12Heap.h"
#include "Log/Log.h"
#include <format>

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
	resource_ = CreateRenderTextureResource(device, buffering->GetSwapChainDesc().Width, buffering->GetSwapChainDesc().Height,
		buffering->GetSwapChainDesc().Format, buffering->GetRtvDesc().Format, Vector4(0.1f, 0.1f, 0.1f, 1.0f), log);

	/*----------------
		RTVの設定
	----------------*/

	// スワップチェーンのRTV設定を反映させる
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;
	rtvDesc.Format = buffering->GetRtvDesc().Format;
	rtvDesc.ViewDimension = buffering->GetRtvDesc().ViewDimension;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.Texture2D.PlaneSlice = 0;

	// ログ出力
	if (log)
	{
		log->Logging(std::format("RTV Texture2D MipSlice : {}" , rtvDesc.Texture2D.MipSlice));
		log->Logging(std::format("RTV Texture2D PlaneSlice : {}", rtvDesc.Texture2D.PlaneSlice));
		log->Logging("Creat RenderTargetView \n");
	}

	// ディスクリプタハンドルを取得する
	rtvCpuHandle_ = heap->GetRtvCPUDescriptorHandle();

	device->CreateRenderTargetView(resource_.Get(), &rtvDesc, rtvCpuHandle_);



	/*---------------
		SRVの設定
	---------------*/

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = buffering->GetRtvDesc().Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.PlaneSlice = 0;

	// ログ出力
	if (log)
	{
		log->Logging("SRV Shader4ComponentMapping : DEFAULT_SHADER_4_COMPONENT_MAPPING");
		log->Logging("SRV ViewDimension : DIMENSION_TEXTURE2D");
		log->Logging(std::format("SRV Texture2D MostDetailedMip : {}", srvDesc.Texture2D.MostDetailedMip));
		log->Logging(std::format("SRV Texture2D MipLevels : {}", srvDesc.Texture2D.MipLevels));
		log->Logging(std::format("SRV Texture2D PlaneSlice : {}", srvDesc.Texture2D.PlaneSlice));
		log->Logging("Creat ShaderResourceView \n");
	}

	// ディスクリプタハンドルを取得する
	srvHandle_.first = heap->GetSrvCPUDescriptorHandle();
	srvHandle_.second = heap->GetSrvGPUDescriptorHandle();

	device->CreateShaderResourceView(resource_.Get(), &srvDesc, srvHandle_.first);
}

/// @brief レンダーターゲットの設定とクリア
/// @param commandList 
/// @param dsvHandle 
void Engine::OffscreenResource::ClearRenderTarget(ID3D12GraphicsCommandList* commandList, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle)
{
	// nullptrチェック
	assert(commandList);

	// 設定
	commandList->OMSetRenderTargets(1, &rtvCpuHandle_, false, &dsvHandle);

	// クリア
	float clearColor[] = { 0.1f, 0.1f ,0.1f, 1.0f };
	commandList->ClearRenderTargetView(rtvCpuHandle_, clearColor, 0, nullptr);
}

/// @brief コマンドリストに登録する
/// @param commandList 
/// @param rootParameterIndex 
void Engine::OffscreenResource::Register(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex)
{
	// nullptrチェック
	assert(commandList);

	// テクスチャ
	commandList->SetGraphicsRootDescriptorTable(rootParameterIndex, srvHandle_.second);
}