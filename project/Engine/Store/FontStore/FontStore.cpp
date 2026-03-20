#include "FontStore.h"
#include <Windows.h>
#include <cassert>
#include "Func/TextureFunc/TextureFunc.h"
#include "RenderContext/DX12Heap/DX12Heap.h"

/// @brief コンストラクタ
Engine::FontStore::FontStore()
{
	// FreeType初期化
	if (FT_Init_FreeType(&ft_))
	{
		assert(false);
	}
}

/// @brief 読み込み
/// @param text 
/// @param pixel 
/// @return 
TextHandle Engine::FontStore::Load(const std::string& text, const std::string& fontName, int32_t pixel,
	ID3D12Device* device, ID3D12GraphicsCommandList* commandList, DX12Heap* heap, Log* log)
{
	assert(device);
	assert(commandList);

	// 同じデータがあるかどうかを探す
	for (auto& data : textTable_)
	{
		if (data->text == text && data->fontName == fontName && data->pixel == pixel)
			return data->handle;
	}


	FT_Face face;

	// 同じフォントがあるかどうか
	if (fontTable_.contains(fontName.c_str()))
	{
		face = fontTable_[fontName];
	}
	else
	{
		// フォント読み込み
		if (FT_New_Face(ft_, fontName.c_str(), 0, &face))
			assert(false);

		fontTable_[fontName] = face;
	}

	// ハンドルを取得する
	TextHandle handle = static_cast<TextHandle>(textTable_.size());

	// フォントデータの生成と初期化
	std::unique_ptr<TextData> fontData = std::make_unique<TextData>();
	fontData->text = text;
	fontData->fontName = fontName;
	fontData->pixel = pixel;
	fontData->handle = handle;
	fontData->hCharTable.resize(static_cast<int32_t>(text.size()));

	int index = 0;

	// 文字ごとに読み込む
	for (auto c : text)
	{
		// 文字読み込み
		fontData->hCharTable[index] = Load(c, fontData->fontName, fontData->pixel, face, device, commandList, heap, log);

		// 次のインデックス
		++index;
	}

	// テーブルに記録する
	textTable_.push_back(std::move(fontData));

	return handle;
}

/// @brief 読み込み
/// @param c 
/// @param fontName 
/// @param pixel 
/// @return 
CharHandle Engine::FontStore::Load(char c, const std::string& fontName, int32_t pixel, FT_Face face,
	ID3D12Device* device, ID3D12GraphicsCommandList* commandList, DX12Heap* heap, Log* log)
{
	// 同じデータがあるかどうかを探す
	for (auto& data : charTable_)
	{
		if (data->c == c && data->fontName == fontName && data->pixel == pixel)
			return data->handle;
	}

	// ハンドルを取得する
	CharHandle handle = static_cast<CharHandle>(charTable_.size());

	// 文字データの生成と初期化
	std::unique_ptr<CharData> charData = std::make_unique<CharData>();
	charData->c = c;
	charData->fontName = fontName;
	charData->pixel = pixel;
	charData->handle = handle;

	// ピクセル
	FT_Set_Pixel_Sizes(face, 0, charData->pixel);

	// 文字生成
	if (FT_Load_Char(face, c, FT_LOAD_RENDER))
	{
		assert(false);
	}


	// テクスチャ用リソースを生成する
	charData->textureResource = CreateFontTextureResource(device, face->glyph, log);

	// 転送用リソースを作成し、GPU転送命令を送る
	charData->uploadResource = UploadFontTextureData(charData->textureResource.Get(), face->glyph, device, commandList, log);


	// SRVを設定する
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R8_UNORM;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;


	// CPU・GPUハンドルを取得する
	charData->srvHandle.first = heap->GetSrvCPUDescriptorHandle();
	charData->srvHandle.second = heap->GetSrvGPUDescriptorHandle();

	// テクスチャリソースにSRVの設定を付与する
	device->CreateShaderResourceView(charData->textureResource.Get(), &srvDesc, charData->srvHandle.first);


	// テーブルに記録
	charTable_.push_back(std::move(charData));

	return handle;
}