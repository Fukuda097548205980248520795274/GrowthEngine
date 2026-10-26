#include "TextureStore.h"
#include "Func/TextureFunc/TextureFunc.h"
#include "RenderContext/DX12Heap/DX12Heap.h"
#include "Log//Log.h"
#include <format>
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <cstdlib>

#include "RenderContext/ImGuiRender/ImGuiRender.h"

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

	// 名前 (元のパスを保持するか、DDSのパスにするかは仕様に応じて調整)
	textureData->name = filePath;

	std::string targetFilePath = filePath;
	std::filesystem::path pathObj(targetFilePath);
	std::string ext = pathObj.extension().string();
	std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

	// PNG等が指定された場合、同階層のDDSファイルを確認し、無ければtexconvで変換する
	if (ext != ".dds")
	{
		std::filesystem::path ddsPath = pathObj;
		ddsPath.replace_extension(".dds");

		// ddsファイルが存在しない場合のみ変換処理を実行
		if (!std::filesystem::exists(ddsPath))
		{
			if (log) log->Logging(std::format("Convert to DDS : {}", targetFilePath));

			// texconv.exeの相対パスを絶対パスに変換
			std::filesystem::path texconvRelativePath = "./Externals/DirectXTex/texconv.exe";
			std::string texconvPath = std::filesystem::absolute(texconvRelativePath).string();

			// 出力先ディレクトリを絶対パスに変換
			std::filesystem::path outDirRelative = pathObj.parent_path();
			if (outDirRelative.empty()) outDirRelative = ".";
			std::string outDir = std::filesystem::absolute(outDirRelative).string();

			// ターゲットファイルパスも絶対パスに変換
			std::string targetAbsPath = std::filesystem::absolute(targetFilePath).string();

			// ★修正ポイント: Windowsのシステムコマンドの仕様対策として、全体をさらにダブルクォーテーションで囲む
			std::string command = std::format("\"\"{}\" -m 0 -f BC7_UNORM -y -o \"{}\" \"{}\"\"", texconvPath, outDir, targetAbsPath);

			// コマンドを実行
			int result = std::system(command.c_str());
			if (result != 0)
			{
				if (log) log->Logging("Failed to run texconv.exe");
			}
		}

		// 読み込み対象のパスを生成したddsファイルに差し替える
		targetFilePath = ddsPath.string();
	}

	// テクスチャファイルを読んで、プログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(targetFilePath);

	HRESULT hr;

	// パスを差し替えたため、常にddsとして読み込む処理に統一可能ですが、念のため拡張子チェックを残します
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
		// texconvの実行に失敗した場合のフォールバックとして残す
		textureData->type_ = TextureType::Texture2D;
		hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
		assert(SUCCEEDED(hr));
		if (log)log->Logging("ext : .png");
	}

	// ハッシュ値を取得する
	size_t hash1 = CalculateTextureHash(*image.GetImages());

	auto normalizeExtension = [](const std::string& path) {
		std::string ext = std::filesystem::path(path).extension().string();
		std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
		return ext;
	};

	const std::string kExtension1 = normalizeExtension(filePath);

	// 過去に取得したミップイメージと被っているかどうかを判断する
	for (std::unique_ptr<TextureData>& data : dataTable_)
	{
		const DirectX::Image* image2 = data->mipImages.GetImages();
		size_t hash2 = CalculateTextureHash(*image2);
		const std::string kExtension2 = normalizeExtension(data->name);

     if (hash1 == hash2 && kExtension1 == kExtension2)
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

/// @brief ファイルパスを取得する
/// @param hTexture 
/// @return 
std::string Engine::TextureStore::GetFilePath(TextureHandle hTexture)const
{
	auto it = std::find_if(dataTable_.begin(), dataTable_.end(),
		[hTexture](const std::unique_ptr<TextureData>& data) {return data->handle == hTexture;});

	// ハンドルが見つかった場合、名前を返す
	if (it != dataTable_.end())
		return (*it)->name;

	return "";
}

/// @brief ハンドルを取得する
/// @param filePath 
/// @return 
TextureHandle Engine::TextureStore::GetHandle(const std::string& filePath)
{
	std::string targetFilePath = filePath;
	std::filesystem::path pathObj(targetFilePath);

	// PNG等の場合、Load時にDDS化されている前提でDDSのパスを検索対象にする
	if (pathObj.extension().string() != ".dds")
	{
		pathObj.replace_extension(".dds");
		if (std::filesystem::exists(pathObj))
		{
			targetFilePath = pathObj.string();
		}
	}

	// テクスチャファイルを読んで、プログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(targetFilePath);

	HRESULT hr;

	// ddsファイルかどうか
	if (filePathW.ends_with(L".dds"))
	{
		hr = DirectX::LoadFromDDSFile(filePathW.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);
		assert(SUCCEEDED(hr));
	}
	else
	{
		// pngとか
		hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
		assert(SUCCEEDED(hr));
	}

	// ハッシュ値を取得する
	size_t hash1 = CalculateTextureHash(*image.GetImages());

	auto normalizeExtension = [](const std::string& path) {
		std::string ext = std::filesystem::path(path).extension().string();
		std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
		return ext;
	};

	const std::string kExtension1 = normalizeExtension(filePath);

	// 過去に取得したミップイメージと被っているかどうかを判断する
	for (std::unique_ptr<TextureData>& data : dataTable_)
	{
		const DirectX::Image* image2 = data->mipImages.GetImages();
		size_t hash2 = CalculateTextureHash(*image2);
		const std::string kExtension2 = normalizeExtension(data->name);

     if (hash1 == hash2 && kExtension1 == kExtension2)
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

	// サムネイルのサイズ
	const int kThumbSize = 32;
	const int kPadding = 8;
	const int kColumns = 4;

	int count = 0;

	ImGui::Begin("Texture");

	for (int i = 0; i < dataTable_.size(); i++)
	{
		const auto& tex = dataTable_[i];

		// Texture2Dのみ
		if (tex->type_ != TextureType::Texture2D)
			continue;

		ImGui::PushID(i);

		bool clicked =
			ImGui::ImageButton(tex->name.c_str(), tex->srvHandle.second.ptr, ImVec2((float)kThumbSize, (float)kThumbSize),
				ImVec2(0, 0), ImVec2(1, 1), ImVec4(0.2f, 0.2f, 0.2f, 1.0f), ImVec4(1, 1, 1, 1));

		// クリックされたら選択状態にする
		if (clicked)
			selected = i;

		// ドラッグ処理
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
		{
			// ペイロードとして index を渡す
			ImGui::SetDragDropPayload("TEXTURE_ID", &i, sizeof(int));

			// ドラッグ中に表示されるプレビュー
			ImGui::Image(tex->srvHandle.second.ptr, ImVec2((float)kThumbSize, (float)kThumbSize));
			ImGui::TextUnformatted(tex->name.c_str());

			ImGui::EndDragDropSource();
		}

		ImGui::PopID();

		count++;
		if (count % kColumns != 0)
			ImGui::SameLine();
	}


	ImGui::End();
}