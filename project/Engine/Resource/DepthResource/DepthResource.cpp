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
void Engine::DepthResource::Initialize(ID3D12Device* device, int32_t width, int32_t height, DX12Heap* heap, Log* log)
{
	// nullptrチェック
	assert(device);
	assert(heap);

	// 深度テクスチャリソースを生成する
	resource_ = CreateDepthStencilTextureResource(device, width, height, log);

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

	// DSV生成
	device->CreateDepthStencilView(resource_.Get(), &dsvDesc, dsvCPUHandle_);

	// 読み取り専用のDSVを生成する
	dsvDesc.Flags = D3D12_DSV_FLAG_READ_ONLY_DEPTH; // 深度書き込みを禁止するフラグ
	dsvReadOnlyCPUHandle_ = heap->GetDsvCPUDescriptorHandle();
	device->CreateDepthStencilView(resource_.Get(), &dsvDesc, dsvReadOnlyCPUHandle_);


	/*---------------
	    SRVの設定
	---------------*/

	// SRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	// ログ出力
	if (log)
	{
		log->Logging("SRV Format : R24_UNORM_X8_TYPELESS");
		log->Logging("SRV Shader4ComponentMapping : D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING");
		log->Logging("SRV ViewDimension : DIMENSION_TEXTURE2D");
		log->Logging("Creat ShaderResourceView \n");
	}

	// SRV用CPUハンドルを取得する
	srvHandle_.first = heap->GetSrvCPUDescriptorHandle();
	srvHandle_.second = heap->GetSrvGPUDescriptorHandle();

	// SRV生成
	device->CreateShaderResourceView(resource_.Get(), &srvDesc, srvHandle_.first);
}

/// @brief サイズを作り直す
/// @param device 
/// @param buffering 
void Engine::DepthResource::Resize(ID3D12Device* device, int32_t width, int32_t height)
{
	assert(device);

	// リソース開放
	resource_.Reset();

	// 新たなサイズで再生成
	resource_ = CreateDepthStencilTextureResource(device, width, height, nullptr);


	// DSV 同じ設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

	// DSV再生成
	device->CreateDepthStencilView(resource_.Get(), &dsvDesc, dsvCPUHandle_);


	// 読み取り専用のDSVを生成する
	dsvDesc.Flags = D3D12_DSV_FLAG_READ_ONLY_DEPTH; // 深度書き込みを禁止するフラグ
	device->CreateDepthStencilView(resource_.Get(), &dsvDesc, dsvReadOnlyCPUHandle_);


	// SRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	// SRV再生成
	device->CreateShaderResourceView(resource_.Get(), &srvDesc, srvHandle_.first);
}

/// @brief バリアを張る
/// @param commandList 
/// @param before 
/// @param after 
void Engine::DepthResource::Barrier(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after)
{
	TransitionBarrier(resource_.Get(), before, after, commandList);
}

/// @brief デプスステンシルのクリア
/// @param commandList 
void Engine::DepthResource::ClearDepthStencil(ID3D12GraphicsCommandList* commandList)
{
	// クリア
	commandList->ClearDepthStencilView(dsvCPUHandle_, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

/// @brief コマンドリストに登録する
/// @param commandList
/// @param rootParameterIndex
void Engine::DepthResource::Register(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex)
{
	assert(commandList);
	commandList->SetGraphicsRootDescriptorTable(rootParameterIndex, srvHandle_.second);
}

/// @brief SRVをコマンドリストに登録する
/// @param commandList 
/// @param rootParameterIndex 
void Engine::DepthResource::RegisterComputeSRV(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex)
{
	assert(commandList);
	commandList->SetComputeRootDescriptorTable(rootParameterIndex, srvHandle_.second);
}