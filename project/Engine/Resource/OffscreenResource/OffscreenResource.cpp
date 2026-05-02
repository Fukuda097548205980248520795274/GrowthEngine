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
void Engine::OffscreenResource::Initialize(ID3D12Device* device, DX12Heap* heap, int32_t width, int32_t height, Log* log)
{
	// nullptrチェック
	assert(device);
	assert(heap);

	// 書き込み可能なリソーステクスチャを生成する
	resource_ = CreateRenderTextureResource(device, width, height, Vector4(0.1f, 0.1f, 0.1f, 1.0f), log);

	/*----------------
		RTVの設定
	----------------*/

	// スワップチェーンのRTV設定を反映させる
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
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
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
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

/// @brief サイズを作り直す
/// @param device 
/// @param buffering 
void Engine::OffscreenResource::Resize(ID3D12Device* device, int32_t width, int32_t height)
{
	assert(device);

	// リソース開放
	resource_.Reset();

	// 新たなサイズで作り直す
	resource_ = CreateRenderTextureResource(device, width, height, Vector4(0.1f, 0.1f, 0.1f, 1.0f), nullptr);

	// スワップチェーンのRTV設定を反映させる
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.Texture2D.PlaneSlice = 0;

	// RTV再生成
	device->CreateRenderTargetView(resource_.Get(), &rtvDesc, rtvCpuHandle_);

	// SRV設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	// SRV再生成
	device->CreateShaderResourceView(resource_.Get(), &srvDesc, srvHandle_.first);
}

/// @brief バリアを張る
/// @param commandList 
/// @param before 
/// @param after 
void Engine::OffscreenResource::Barrier(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after)
{
	TransitionBarrier(resource_.Get(), before, after, commandList);
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

/// @brief レンダーターゲットの設定
/// @param commandList 
/// @param dsvHandle 
void Engine::OffscreenResource::SetRenderTarget(ID3D12GraphicsCommandList* commandList, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle)
{
	// nullptrチェック
	assert(commandList);

	// 設定
	commandList->OMSetRenderTargets(1, &rtvCpuHandle_, false, &dsvHandle);
}

/// @brief コマンドリストに登録する
/// @param commandList 
/// @param rootParameterIndex 
void Engine::OffscreenResource::RegisterGraphics(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex)
{
	// nullptrチェック
	assert(commandList);

	// テクスチャ
	commandList->SetGraphicsRootDescriptorTable(rootParameterIndex, srvHandle_.second);
}

/// @brief コマンドリストに登録する
/// @param commandList 
/// @param rootParameterIndex 
void Engine::OffscreenResource::RegisterCompute(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex)
{
	// nullptrチェック
	assert(commandList);

	// テクスチャ
	commandList->SetComputeRootDescriptorTable(rootParameterIndex, srvHandle_.second);
}