#include "TextureStore.h"
#include "Func/TextureFunc/TextureFunc.h"
#include "RenderContext/DX12Heap/DX12Heap.h"
#include "Log//Log.h"
#include <format>

#include <imgui.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>

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

	// ミップイメージを取得する
	textureData->mipImages = LoadTextureGetMipImages(filePath,log);

	// ハッシュ値を取得する
	const DirectX::Image* image1 = textureData->mipImages.GetImages();
	size_t hash1 = CalculateTextureHash(*image1);

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