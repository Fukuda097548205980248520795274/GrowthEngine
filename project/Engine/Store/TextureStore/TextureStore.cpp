#include "TextureStore.h"
#include "Func/TextureFunc/TextureFunc.h"
#include "Func/ResourceFunc/ResourceFunc.h"
#include "RenderContext/DX12Heap/DX12Heap.h"
#include "Log//Log.h"
#include <format>

#include "RenderContext/ImGuiRender/ImGuiRender.h"

#include <ft2build.h>
#include FT_FREETYPE_H

/// @brief コンストラクタ
Engine::TextureStore::TextureStore(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, DX12Heap* heap)
{
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		// 初期化失敗
		assert(false);
	}

	FT_Face face;
	if (FT_New_Face(ft, "C:/Windows/Fonts/arial.ttf", 0, &face))
	{
		// 読み込み失敗
		assert(false);
	}

	FT_Set_Pixel_Sizes(face, 0, 48);

	if (FT_Load_Char(face, 'A', FT_LOAD_RENDER))
	{
		// 失敗
		assert(false);
	}

	FT_GlyphSlot g = face->glyph;



	/*---------------------------------
		メタデータを元にリソースを作成
	---------------------------------*/

	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Width = g->bitmap.width;
	resourceDesc.Height = g->bitmap.rows;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.Format = DXGI_FORMAT_R8_UNORM;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	/*------------------------
		利用するヒープの設定
	------------------------*/

	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;


	/*----------------------
		リソースを生成する
	----------------------*/

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

		IID_PPV_ARGS(&fontResource_)
	);
	assert(SUCCEEDED(hr));




	UINT64 uploadBufferSize = 0;

	device->GetCopyableFootprints(
		&resourceDesc,
		0,
		1,
		0,
		nullptr,
		nullptr,
		nullptr,
		&uploadBufferSize
	);


	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;

	D3D12_RESOURCE_DESC bufDesc{};
	bufDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	bufDesc.Width = uploadBufferSize;
	bufDesc.Height = 1;
	bufDesc.DepthOrArraySize = 1;
	bufDesc.MipLevels = 1;
	bufDesc.SampleDesc.Count = 1;
	bufDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&bufDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&uploadFontResource_));


	// データを割り当てる
	UINT8* mappedData = nullptr;
	uploadFontResource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedData));


	D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint;
	UINT numRows;
	UINT64 rowSize;
	UINT64 totalBytes;

	device->GetCopyableFootprints(
		&resourceDesc,
		0,
		1,
		0,
		&footprint,
		&numRows,
		&rowSize,
		&totalBytes
	);

	UINT rowPitch = footprint.Footprint.RowPitch;

	for (int y = 0; y < static_cast<int32_t>(g->bitmap.rows); y++)
	{
		memcpy(
			mappedData + y * rowPitch,
			g->bitmap.buffer + y * g->bitmap.width,
			g->bitmap.width
		);
	}




	D3D12_TEXTURE_COPY_LOCATION dst{};
	dst.pResource = fontResource_.Get();
	dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	dst.SubresourceIndex = 0;

	D3D12_TEXTURE_COPY_LOCATION src{};
	src.pResource = uploadFontResource_.Get();
	src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	src.PlacedFootprint.Footprint.Format = DXGI_FORMAT_R8_UNORM;
	src.PlacedFootprint.Footprint.Width = g->bitmap.width;
	src.PlacedFootprint.Footprint.Height = g->bitmap.rows;
	src.PlacedFootprint.Footprint.Depth = 1;
	src.PlacedFootprint.Footprint.RowPitch = rowPitch;

	commandList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);


	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource = fontResource_.Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	commandList->ResourceBarrier(1, &barrier);

	uploadFontResource_->Unmap(0, nullptr);


	// SRVを設定する
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R8_UNORM;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;


	// CPU・GPUハンドルを取得する
	fontSrvHandle_.first = heap->GetSrvCPUDescriptorHandle();
	fontSrvHandle_.second = heap->GetSrvGPUDescriptorHandle();

	// テクスチャリソースにSRVの設定を付与する
	device->CreateShaderResourceView(fontResource_.Get(), &srvDesc, fontSrvHandle_.first);
}

/// @brief 読み込み
/// @param filePath 
/// @param heap 
/// @param device 
/// @param commandList 
/// @param log 
TextureHandle Engine::TextureStore::Load(const std::string& filePath, DX12Heap* heap, ID3D12Device* device, ID3D12GraphicsCommandList* commandList, Log* log)
{
	// nullptrチェック
	assert(heap);
	assert(device);
	assert(commandList);


	// テクスチャデータ
	std::unique_ptr<TextureData> textureData = nullptr;
	textureData = std::make_unique<TextureData>();

	// 名前
	textureData->name = filePath;


	// テクスチャファイルを読んで、プログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);

	HRESULT hr;

	// ddsファイルかどうか
	if (filePathW.ends_with(L".dds"))
	{
		// キューブマップ
		textureData->type_ = TextureType::Cubemap;

		hr = DirectX::LoadFromDDSFile(filePathW.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);
		assert(SUCCEEDED(hr));
		if (log)log->Logging("ext : .dds");
	}
	else
	{
		// 2Dテクスチャ
		textureData->type_ = TextureType::Texture2D;

		// pngとか
		hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
		assert(SUCCEEDED(hr));
		if (log)log->Logging("ext : .png");
	}

	// ハッシュ値を取得する
	size_t hash1 = CalculateTextureHash(*image.GetImages());

	// 過去に取得したミップイメージと被っているかどうかを判断する
	for (std::unique_ptr<TextureData>& data : dataTable_)
	{
		const DirectX::Image* image2 = data->mipImages.GetImages();
		size_t hash2 = CalculateTextureHash(*image2);

		if (hash1 == hash2)
		{
			return data->handle;
		}
	}

	// ログ出力
	if (log)log->Logging(std::format("Load Texture : {}", filePath));

	// 圧縮フォーマットであるとき
	if (DirectX::IsCompressed(image.GetMetadata().format))
	{
		textureData->mipImages = std::move(image);
		if (log)log->Logging("CompressionFormat : true");
	}
	else
	{
		// 圧縮フォーマットではないとき
		hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, textureData->mipImages);
		assert(SUCCEEDED(hr));
		if (log)log->Logging("CompressionFormat : false");
	}

	// ログ出力
	if (log)log->Logging("Succeeded LoadTextureFile \n");




	// テクスチャリソースを取得する
	const DirectX::TexMetadata& metadata = textureData->mipImages.GetMetadata();
	textureData->resource = CreateTextureResource(device, metadata,log);

	// 中間リソースを取得する
	textureData->subResource = UploadTextureData(textureData->resource.Get(), textureData->mipImages, device, commandList , log);

	// SRVを設定する
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	if (log)log->Logging("SRV Shader4ComponentMapping : D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING");

	// キューブマップ
	if (metadata.IsCubemap())
	{
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MostDetailedMip = 0;
		srvDesc.TextureCube.MipLevels = UINT_MAX;
		srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;

		if (log)
		{
			log->Logging("SRV ViewDimension : DIMENSION_TEXTURECUBE");
			log->Logging(std::format("SRV MostDetailedMip : {}", srvDesc.TextureCube.MostDetailedMip));
			log->Logging("SRV MipLevels : UINT_MAX");
			log->Logging(std::format("SRV ResourceMinLODClamp : {}", srvDesc.TextureCube.ResourceMinLODClamp));
		}

	} 
	else
	{
		// 通常のテクスチャ
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

		if (log)
		{
			log->Logging("SRV ViewDimension : DIMENSION_TEXTURECUBE");
			log->Logging(std::format("SRV Texture2D MipLevels : {}", srvDesc.Texture2D.MipLevels));
		}
	}

	if (log)log->Logging("Create ShaderResourceView \n");



	// CPU・GPUハンドルを取得する
	textureData->srvHandle.first = heap->GetSrvCPUDescriptorHandle();
	textureData->srvHandle.second = heap->GetSrvGPUDescriptorHandle();

	// テクスチャリソースにSRVの設定を付与する
	device->CreateShaderResourceView(textureData->resource.Get(), &srvDesc, textureData->srvHandle.first);

	// ハンドルを取得する
	TextureHandle handle;
	handle = static_cast<uint32_t>(dataTable_.size());
	textureData->handle = handle;
	if (log)log->Logging(std::format("TextureHandle : {} \n", textureData->handle));

	// 登録する
	dataTable_.push_back(std::move(textureData));

	return handle;
}

/// @brief ハンドルを取得する
/// @param filePath 
/// @return 
TextureHandle Engine::TextureStore::GetHandle(const std::string& filePath)
{
	// テクスチャファイルを読んで、プログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);

	HRESULT hr;

	// ddsファイルかどうか
	if (filePathW.ends_with(L".dds"))
	{
		hr = DirectX::LoadFromDDSFile(filePathW.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);
		assert(SUCCEEDED(hr));
	} else
	{
		// pngとか
		hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
		assert(SUCCEEDED(hr));
	}

	// ハッシュ値を取得する
	size_t hash1 = CalculateTextureHash(*image.GetImages());

	// 過去に取得したミップイメージと被っているかどうかを判断する
	for (std::unique_ptr<TextureData>& data : dataTable_)
	{
		const DirectX::Image* image2 = data->mipImages.GetImages();
		size_t hash2 = CalculateTextureHash(*image2);

		if (hash1 == hash2)
		{
			return data->handle;
		}
	}

	return 0;
}

/// @brief テクスチャのハッシュ値を計算する
/// @param image 
/// @return 
size_t Engine::TextureStore::CalculateTextureHash(const DirectX::Image& image)
{
	size_t hash = 0;
	const uint8_t* data = image.pixels;
	for (size_t i = 0; i < image.slicePitch; ++i)
	{
		hash = (hash * 31) + data[i];
	}
	return hash;
}


/// @brief UIを描画する
void Engine::TextureStore::DrawUI()
{
	static int selected = -1;

	const int thumbSize = 32;
	const int padding = 8;
	const int columns = 4;

	int count = 0;

	ImGui::Begin("Texture");

	for (int i = 0; i < dataTable_.size(); i++)
	{
		const auto& tex = dataTable_[i];

		// Texture2Dのみ
		if (tex->type_ != TextureType::Texture2D)
			continue;

		ImGui::PushID(i);

		bool clicked = ImGui::ImageButton(
			tex->name.c_str(),
			tex->srvHandle.second.ptr,
			ImVec2((float)thumbSize, (float)thumbSize),
			ImVec2(0, 0),
			ImVec2(1, 1),
			ImVec4(0.2f, 0.2f, 0.2f, 1.0f),
			ImVec4(1, 1, 1, 1)
		);

		if (clicked)
		{
			selected = i;
		}

		// --- ここからドラッグ元処理 ---
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
		{
			// ペイロードとして index を渡す
			ImGui::SetDragDropPayload("TEXTURE_ID", &i, sizeof(int));

			// ドラッグ中に表示されるプレビュー
			ImGui::Image(
				tex->srvHandle.second.ptr,
				ImVec2((float)thumbSize, (float)thumbSize)
			);
			ImGui::TextUnformatted(tex->name.c_str());

			ImGui::EndDragDropSource();
		}
		// --- ここまで ---

		ImGui::PopID();

		count++;
		if (count % columns != 0)
			ImGui::SameLine();
	}


	ImGui::End();
}