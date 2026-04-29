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

/// @brief UAVバリアを張る
/// @param resource 
/// @param commandList 
void Engine::UAVBarrier(ID3D12Resource* resource, ID3D12GraphicsCommandList* commandList)
{
	// nullptrチェック
	assert(resource);
	assert(commandList);

	// バリアの設定
	D3D12_RESOURCE_BARRIER barrier;
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.UAV.pResource = resource;

	// バリアを張る
	commandList->ResourceBarrier(1, &barrier);
}

/// @brief テクスチャリソースをコピーする
/// @param commandList 
/// @param dstResource 
/// @param desBefore 
/// @param dstAfter 
/// @param srcResource 
/// @param srcBefore 
/// @param srcAfter 
void Engine::CopyTextureResource(ID3D12GraphicsCommandList* commandList,
	ID3D12Resource* dstResource, D3D12_RESOURCE_STATES dstBefore, D3D12_RESOURCE_STATES dstAfter,
	ID3D12Resource* srcResource, D3D12_RESOURCE_STATES srcBefore, D3D12_RESOURCE_STATES srcAfter)
{
	// コピー前のバリア
	D3D12_RESOURCE_BARRIER barriers[2]{};

	// src
	barriers[0].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barriers[0].Transition.pResource = srcResource;
	barriers[0].Transition.StateBefore = srcBefore;
	barriers[0].Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;
	barriers[0].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	// dst
	barriers[1].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barriers[1].Transition.pResource = dstResource;
	barriers[1].Transition.StateBefore = dstBefore;
	barriers[1].Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
	barriers[1].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	commandList->ResourceBarrier(2, barriers);

	// コピー
	commandList->CopyResource(dstResource, srcResource);

	// コピー後のバリア（元に戻す）
	barriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
	barriers[0].Transition.StateAfter = srcAfter;

	barriers[1].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barriers[1].Transition.StateAfter = dstAfter;

	commandList->ResourceBarrier(2, barriers);
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
Microsoft::WRL::ComPtr<ID3D12Resource> Engine::CreateUAVResource(ID3D12Device* device, size_t sizeInBytes, Log* log, ID3D12GraphicsCommandList* commandList)
{
	// ヒープの設定
	D3D12_HEAP_PROPERTIES defaultHeapProperties{};
	defaultHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

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
	HRESULT hr = device->CreateCommittedResource(&defaultHeapProperties, D3D12_HEAP_FLAG_NONE,
		&resourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(hr));

	// 生成成功ログ
	if (log)log->Logging("CreateCommitted UAVResource \n");

	// リソースステート遷移
	Engine::TransitionBarrier(resource.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, commandList);

	return resource;
}

/// @brief 書き込み可能なテクスチャリソースを生成する
/// @param device 
/// @param width 
/// @param height 
/// @param swapChainFormat 
/// @param rtvFormat 
/// @param clearColor 
/// @return 
Microsoft::WRL::ComPtr<ID3D12Resource> Engine::CreateRenderTextureResource(ID3D12Device* device, uint32_t width, uint32_t height,
	DXGI_FORMAT swapChainFormat, DXGI_FORMAT rtvFormat, Vector4 clearColor, Log* log)
{
	/*-----------------------
		リソースの設定を行う
	-----------------------*/

	D3D12_RESOURCE_DESC resourceDesc{};

	resourceDesc.Width = UINT(width);
	resourceDesc.Height = UINT(height);
	resourceDesc.Format = swapChainFormat;

	// 書き込める設定
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;


	/*----------------------
		ヒープの設定を行う
	----------------------*/

	D3D12_HEAP_PROPERTIES heapProperties{};

	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;


	/*----------------------
		クリア最適値の設定
	----------------------*/

	D3D12_CLEAR_VALUE clearValue;
	clearValue.Format = rtvFormat;
	clearValue.Color[0] = clearColor.x;
	clearValue.Color[1] = clearColor.y;
	clearValue.Color[2] = clearColor.z;
	clearValue.Color[3] = clearColor.w;


	if (log)
	{
		log->Logging("Heap : DEFAULT");
		log->Logging("Dimension : TEXTURE2D");
		log->Logging(std::format("Width : {}", resourceDesc.Width));
		log->Logging(std::format("Height : {}", resourceDesc.Height));
		log->Logging(std::format("DepthOrArraySize : {}", resourceDesc.DepthOrArraySize));
		log->Logging(std::format("MipLevels : {}", resourceDesc.MipLevels));
		log->Logging(std::format("SampleDesc.Count : {}", resourceDesc.SampleDesc.Count));
		log->Logging("Layout : UNKNOWN");
		log->Logging("Flags : ALLOW_RENDER_TARGET");
		log->Logging(std::format("Clear : {},{},{},{}", clearColor.x, clearColor.y, clearColor.z, clearColor.w));
	}


	/*-------------------
		リソースの生成
	-------------------*/

	Microsoft::WRL::ComPtr<ID3D12Resource> resource;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		&clearValue,
		IID_PPV_ARGS(&resource)
	);
	assert(SUCCEEDED(hr));

	// 生成成功ログ
	if (log)log->Logging("CreateCommitted RenderTextureResource \n");

	return resource;
}


/// @brief UAVテクスチャリソースを生成する
/// @param device 
/// @param width 
/// @param height 
/// @param log 
/// @return 
Microsoft::WRL::ComPtr<ID3D12Resource> Engine::CreateUAVTextureResource(ID3D12Device* device, ID3D12GraphicsCommandList* commandList,
	uint32_t width, uint32_t height, Log* log)
{
	assert(device);
	assert(commandList);

	/*------------------
	    リソースの設定
	------------------*/

	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Width = width;
	resourceDesc.Height = height;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

	// UAVとして使用できるようにする
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;


	/*----------------
	    ヒープの設定
	----------------*/

	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;


	if (log)
	{
		log->Logging("Heap : DEFAULT");
		log->Logging("Dimension : TEXTURE2D");
		log->Logging(std::format("Width : {}", resourceDesc.Width));
		log->Logging(std::format("Height : {}", resourceDesc.Height));
		log->Logging(std::format("DepthOrArraySize : {}", resourceDesc.DepthOrArraySize));
		log->Logging(std::format("MipLevels : {}", resourceDesc.MipLevels));
		log->Logging(std::format("SampleDesc.Count : {}", resourceDesc.SampleDesc.Count));
		log->Logging("Layout : UNKNOWN");
		log->Logging("Flags : ALLOW_UNORDERED_ACCESS");
	}


	/*----------------------
		リソースを生成する
	----------------------*/

	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;

	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&resource)
	);

	assert(SUCCEEDED(hr));

	// 生成成功ログ
	if (log)log->Logging("CreateCommitted UAVTextureResource \n");

	// リソースステート遷移
	Engine::TransitionBarrier(resource.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, commandList);

	return resource;
}


/// @brief 深度テクスチャリソースを生成する
/// @param device 
/// @param width 
/// @param height 
/// @return 
Microsoft::WRL::ComPtr<ID3D12Resource> Engine::CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height, Log* log)
{
	/*------------------
		リソースの設定
	------------------*/

	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;
	resourceDesc.Height = height;
	resourceDesc.MipLevels = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;


	/*----------------
		ヒープの設定
	----------------*/

	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;


	/*----------------------
		深度値のクリア設定
	----------------------*/

	D3D12_CLEAR_VALUE depthClearValue{};

	// 1.0fでクリアする
	depthClearValue.DepthStencil.Depth = 1.0f;
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;


	if (log)
	{
		log->Logging("Heap : DEFAULT");
		log->Logging("Dimension : TEXTURE2D");
		log->Logging(std::format("Width : {}", resourceDesc.Width));
		log->Logging(std::format("Height : {}", resourceDesc.Height));
		log->Logging(std::format("DepthOrArraySize : {}", resourceDesc.DepthOrArraySize));
		log->Logging(std::format("MipLevels : {}", resourceDesc.MipLevels));
		log->Logging(std::format("SampleDesc.Count : {}", resourceDesc.SampleDesc.Count));
		log->Logging("Format : D24_UNORM_S8_UINT");
		log->Logging("Flags : ALLOW_DEPTH_STENCIL");
		log->Logging(std::format("Clear DepthStencilDepth : {}", depthClearValue.DepthStencil.Depth));
		log->Logging("Clear Format : D24_UNORM_S8_UINT");
	}


	/*----------------------
		リソースを生成する
	----------------------*/

	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		// ヒープの設定
		&heapProperties,

		// ヒープの特殊な設定
		D3D12_HEAP_FLAG_NONE,

		// リソースの設定
		&resourceDesc,

		// 深度値を書き込む設定
		D3D12_RESOURCE_STATE_DEPTH_WRITE,

		// クリア最適地
		&depthClearValue,

		IID_PPV_ARGS(&resource)
	);
	assert(SUCCEEDED(hr));

	// 生成成功ログ
	if (log)log->Logging("CreateCommitted DepthStencilTextureResource \n");

	return resource;
}

/// @brief モーションベクターテクスチャリソースを生成する
/// @param device 
/// @param width 
/// @param height 
/// @param clearColor 
/// @param log 
/// @return 
Microsoft::WRL::ComPtr<ID3D12Resource> Engine::CreateMotionVectorResource(ID3D12Device* device, uint32_t width, uint32_t height, Vector4 clearColor, Log* log)
{
	/*-----------------------
		リソースの設定を行う
	-----------------------*/

	D3D12_RESOURCE_DESC resourceDesc{};

	resourceDesc.Width = UINT(width);
	resourceDesc.Height = UINT(height);
	resourceDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;

	// 書き込める設定
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;


	/*----------------------
		ヒープの設定を行う
	----------------------*/

	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;


	/*----------------------
		クリア最適値の設定
	----------------------*/

	D3D12_CLEAR_VALUE clearValue;
	clearValue.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	clearValue.Color[0] = clearColor.x;
	clearValue.Color[1] = clearColor.y;
	clearValue.Color[2] = clearColor.z;
	clearValue.Color[3] = clearColor.w;


	if (log)
	{
		log->Logging("Heap : DEFAULT");
		log->Logging("Dimension : TEXTURE2D");
		log->Logging(std::format("Width : {}", resourceDesc.Width));
		log->Logging(std::format("Height : {}", resourceDesc.Height));
		log->Logging(std::format("DepthOrArraySize : {}", resourceDesc.DepthOrArraySize));
		log->Logging(std::format("MipLevels : {}", resourceDesc.MipLevels));
		log->Logging(std::format("SampleDesc.Count : {}", resourceDesc.SampleDesc.Count));
		log->Logging("Layout : UNKNOWN");
		log->Logging("Flags : ALLOW_RENDER_TARGET");
		log->Logging(std::format("Clear : {},{},{},{}", clearColor.x, clearColor.y, clearColor.z, clearColor.w));
	}


	/*-------------------
		リソースの生成
	-------------------*/

	Microsoft::WRL::ComPtr<ID3D12Resource> resource;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		&clearValue,
		IID_PPV_ARGS(&resource)
	);
	assert(SUCCEEDED(hr));

	// 生成成功ログ
	if (log)log->Logging("CreateCommitted MotionVectorResource \n");

	return resource;
}