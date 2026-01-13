#include "ResourceFunc.h"
#include <cassert>
#include <format>
#include "Log/Log.h"

/// @brief トランジションバリアを張る
/// @param resource 
/// @param before 
/// @param after 
/// @param commandList 
void Engine::TransitionBarrier(ID3D12Resource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after, ID3D12GraphicsCommandList* commandList)
{
	// nullptrチェック
	assert(resource);
	assert(commandList);

	// バリアの設定
	D3D12_RESOURCE_BARRIER barrier;
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = resource;
	barrier.Transition.StateBefore = before;
	barrier.Transition.StateAfter = after;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	// バリアを張る
	commandList->ResourceBarrier(1, &barrier);
}

/// @brief バッファリソースを生成する
/// @param device 
/// @param sizeInBytes 
/// @param log 
/// @return 
Microsoft::WRL::ComPtr<ID3D12Resource> Engine::CreateBufferResource(ID3D12Device* device, size_t sizeInBytes, Log* log)
{
	// ヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;

	// 頂点リソースの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = sizeInBytes;

	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;

	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// 設定ログ
	if (log)
	{
		log->Logging("Heap : UPLOAD");
		log->Logging("Dimension : BUFFER");
		log->Logging(std::format("Width sizeInBytes : {}", resourceDesc.Width));
		log->Logging(std::format("Height : {}" , resourceDesc.Height));
		log->Logging(std::format("DepthOrArraySize : {}", resourceDesc.DepthOrArraySize));
		log->Logging(std::format("MipLevels : {}" , resourceDesc.MipLevels));
		log->Logging(std::format("SampleDesc.Count : {}", resourceDesc.SampleDesc.Count));
		log->Logging("Layout : ROW_MAJOR");
	}

	// 頂点リソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> resource;
	HRESULT hr = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
		&resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(hr));

	// 生成成功ログ
	if (log)log->Logging("CreateCommitted BufferResource :  \n");

	return resource;
}

/// @brief UAVリソースを生成する
/// @param device 
/// @param sizeInBytes 
/// @param log 
/// @return 
Microsoft::WRL::ComPtr<ID3D12Resource> Engine::CreateUAVResource(ID3D12Device* device, size_t sizeInBytes, Log* log)
{
	// ヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	// 頂点リソースの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = sizeInBytes;

	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;

	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	// 設定ログ
	if (log)
	{
		log->Logging("Heap : DEFAULT");
		log->Logging("Dimension : BUFFER");
		log->Logging(std::format("Width sizeInBytes : {}", resourceDesc.Width));
		log->Logging(std::format("Height : {}", resourceDesc.Height));
		log->Logging(std::format("DepthOrArraySize : {}", resourceDesc.DepthOrArraySize));
		log->Logging(std::format("MipLevels : {}", resourceDesc.MipLevels));
		log->Logging(std::format("SampleDesc.Count : {}", resourceDesc.SampleDesc.Count));
		log->Logging("Layout : ROW_MAJOR");
		log->Logging("Flags : ALLOW_UNORDERED_ACCESS");
	}

	// 頂点リソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> resource;
	HRESULT hr = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
		&resourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(hr));

	// 生成成功ログ
	if (log)log->Logging("CreateCommitted UAVResource \n");

	return resource;
}