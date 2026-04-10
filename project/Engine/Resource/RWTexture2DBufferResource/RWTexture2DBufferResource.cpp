#include "RWTexture2DBufferResource.h"

#include "Log/Log.h"
#include "Func/ResourceFunc/ResourceFunc.h"
#include "RenderContext/DX12Heap/DX12Heap.h"
#include "RenderContext/DX12Buffering/DX12Buffering.h"
#include <cassert>
#include <format>

/// @brief 初期化
/// @param device 
/// @param heap 
/// @param num 
/// @param log 
void Engine::RWTexture2DBufferResource::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, DX12Heap* heap,
	uint32_t width, uint32_t height, Log* log)
{
	// nullptrチェック
	assert(device);
	assert(heap);

	// 引数を受け取る
	width_ = width;
	height_ = height;

	// リソース作成
	resource_ = CreateUAVTextureResource(device, commandList, width_, height_, log);



	// UAVの設定
	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;

	// ハンドルを取得する
	uavHandle_.first = heap->GetSrvCPUDescriptorHandle();
	uavHandle_.second = heap->GetSrvGPUDescriptorHandle();

	// ビューの生成
	device->CreateUnorderedAccessView(resource_.Get(), nullptr, &uavDesc, uavHandle_.first);

	// ログ出力
	if (log)
	{
		log->Logging("UAV Format : UNKNOWN");
		log->Logging("UAV ViewDimension : DIMENSION_TEXTURE2D");
		log->Logging(std::format("UAV Texture2D MipSlice : {} byte", uavDesc.Texture2D.MipSlice));
		log->Logging("Creat UnorderedAccessView \n");
	}


	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	// ハンドルを取得する
	srvHandle_.first = heap->GetSrvCPUDescriptorHandle();
	srvHandle_.second = heap->GetSrvGPUDescriptorHandle();

	// ビューの生成
	device->CreateShaderResourceView(resource_.Get(), &srvDesc, srvHandle_.first);

	// ログ出力
	if (log)
	{
		log->Logging("SRV hader4ComponentMapping : D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING");
		log->Logging("SRV Format : R8G8B8A8_UNORM");
		log->Logging("SRV ViewDimension : DIMENSION_TEXTURE2D");
		log->Logging(std::format("SRV Texture2D MipLevels : {} byte", srvDesc.Texture2D.MipLevels));
		log->Logging("Creat ShaderResourceView \n");
	}
}

/// @brief コマンドリストに登録する
/// @param commandList 
/// @param rootParameterIndex 
void Engine::RWTexture2DBufferResource::RegisterGraphicsSRV(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex)
{
	commandList->SetGraphicsRootDescriptorTable(rootParameterIndex, srvHandle_.second);
}

/// @brief コマンドリストに登録する
/// @param commandList 
/// @param rootParameterIndex 
void Engine::RWTexture2DBufferResource::RegisterComputeSRV(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex)
{
	commandList->SetComputeRootDescriptorTable(rootParameterIndex, srvHandle_.second);
}

/// @brief コマンドリストに登録する
/// @param commandList 
/// @param rootParameterIndex 
void Engine::RWTexture2DBufferResource::RegisterGraphicsUAV(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex)
{
	commandList->SetGraphicsRootDescriptorTable(rootParameterIndex, uavHandle_.second);
}

/// @brief コマンドリストに登録する
/// @param commandList 
/// @param rootParameterIndex 
void Engine::RWTexture2DBufferResource::RegisterComputeUAV(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex)
{
	commandList->SetComputeRootDescriptorTable(rootParameterIndex, uavHandle_.second);
}

/// @brief サイズを作り直す
/// @param device 
/// @param width 
/// @param height 
void Engine::RWTexture2DBufferResource::Resize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, uint32_t width, uint32_t height)
{
	// nullptrチェック
	assert(device);
	assert(commandList);

	// サイズを更新する
	width_ = width;
	height_ = height;

	// リソースを作り直す
	resource_ = CreateUAVTextureResource(device, commandList, width_, height_, nullptr);


	// UAVの設定
	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;

	// ビューの生成
	device->CreateUnorderedAccessView(resource_.Get(), nullptr, &uavDesc, uavHandle_.first);


	// SRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	// ビューの生成
	device->CreateShaderResourceView(resource_.Get(), &srvDesc, srvHandle_.first);
}

/// @brief バリアを張る
/// @param commandList 
/// @param before 
/// @param after
void Engine::RWTexture2DBufferResource::Barrier(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after)
{
	TransitionBarrier(resource_.Get(), before, after, commandList);
}