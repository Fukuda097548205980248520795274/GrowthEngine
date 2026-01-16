#include "TextureFunc.h"
#include "Log/Log.h"
#include <format>
#include "Func/ResourceFunc/ResourceFunc.h"

/// <summary>
/// テクスチャを読み込む
/// </summary>
/// <param name="filePath"></param>
/// <returns></returns>
DirectX::ScratchImage Engine::LoadTextureGetMipImages(const std::string& filePath, Log* log)
{
	if (log)log->Logging(std::format("Load Texture : {}", filePath));

	// テクスチャファイルを読んで、プログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);

	HRESULT hr;

	// ddsファイルかどうか
	if (filePathW.ends_with(L".dds"))
	{
		hr = DirectX::LoadFromDDSFile(filePathW.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);
		assert(SUCCEEDED(hr));
		if (log)log->Logging("ext : .dds");
	}
	else
	{
		// pngとか
		hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
		assert(SUCCEEDED(hr));
		if (log)log->Logging("ext : .png");
	}


	// ミップマップの作成
	DirectX::ScratchImage mipImages{};

	// 圧縮フォーマットであるとき
	if (DirectX::IsCompressed(image.GetMetadata().format))
	{
		mipImages = std::move(image);
		if (log)log->Logging("CompressionFormat : true");
	}
	else
	{
		// 圧縮フォーマットではないとき
		hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
		assert(SUCCEEDED(hr));
		if (log)log->Logging("CompressionFormat : false");
	}

	if (log)log->Logging("Succeeded LoadTextureFile \n");

	// ミップマップ付きデータを返す
	return mipImages;
}

/// <summary>
/// テクスチャ用リソースを作成する
/// </summary>
/// <param name="device"></param>
/// <param name="metadata"></param>
/// <returns></returns>
Microsoft::WRL::ComPtr<ID3D12Resource> Engine::CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata, Log* log)
{
	// nullptrチェック
	assert(device);


	/*---------------------------------
		メタデータを元にリソースを作成
	---------------------------------*/

	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width);
	resourceDesc.Height = UINT(metadata.height);
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);
	resourceDesc.Format = metadata.format;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);


	/*------------------------
		利用するヒープの設定
	------------------------*/

	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;


	if (log)
	{
		log->Logging("Heap : DEFAULT");
		log->Logging(std::format("Width : {}", resourceDesc.Width));
		log->Logging(std::format("Height : {}", resourceDesc.Height));
		log->Logging(std::format("MipLevels : {}", resourceDesc.MipLevels));
		log->Logging(std::format("DepthOrArraySize : {}", resourceDesc.DepthOrArraySize));
		log->Logging(std::format("SampleDesc.Count : {}", resourceDesc.SampleDesc.Count));
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

		// データ転送できる設定
		D3D12_RESOURCE_STATE_COPY_DEST,

		// クリア最適値
		nullptr,

		IID_PPV_ARGS(&resource)
	);
	assert(SUCCEEDED(hr));

	// 生成成功ログ
	if (log)log->Logging("CreateCommitted TextureResource \n");

	return resource;
}

/// <summary>
/// テクスチャデータをGPUに転送するためのリソースを生成する
/// </summary>
/// <param name="texture"></param>
/// <param name="mipImages"></param>
/// <param name="device"></param>
/// <param name="commandList"></param>
/// <returns></returns>
[[nodiscard]]
Microsoft::WRL::ComPtr<ID3D12Resource> Engine::UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages,
	ID3D12Device* device, ID3D12GraphicsCommandList* commandList, Log* log)
{
	// nullptrチェック
	assert(texture);
	assert(device);
	assert(commandList);


	std::vector<D3D12_SUBRESOURCE_DATA> subResources;
	DirectX::PrepareUpload(device, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subResources);
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subResources.size()));
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = CreateBufferResource(device, intermediateSize, log);
	UpdateSubresources(commandList, texture, intermediateResource.Get(), 0, 0, UINT(subResources.size()), subResources.data());
	if (log)log->Logging("UpdateSubresources \n");

	// テクスチャを転送するために、リソースステートを変更する
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = texture;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;

	if (log)
	{
		log->Logging("Barrier Type : TRANSITION");
		log->Logging("Barrier Type : NONE");
		log->Logging("Barrier Transition Subresources : ALL_SUBRESOURCES");
		log->Logging("Barrier Transition StateBefore : COPY_DEST");
		log->Logging("Barrier Transition StateAfter : GENERIC_READ");
	}

	commandList->ResourceBarrier(1, &barrier);
	if (log)log->Logging("Register : ResourceBarrier \n");

	return intermediateResource;
}