#include "FontStore.h"
#include <Windows.h>
#include <cassert>

/// @brief コンストラクタ
Engine::FontStore::FontStore()
{
	// FreeType初期化
	HRESULT hr = FT_Init_FreeType(&ft_);
	assert(SUCCEEDED(hr));
}

/// @brief 読み込み
/// @param text 
/// @param pixel 
/// @return 
FontHandle Engine::FontStore::Load(const std::string& text, const std::string& fontName, int32_t pixel)
{
	// 同じデータがあるかどうかを探す
	for (auto& data : fontTable_)
	{
		if (data->text == text && data->fontName == fontName && data->pixel == pixel)
			return data->handle;
	}

	// ハンドルを取得する
	FontHandle handle = static_cast<FontHandle>(fontTable_.size());

	// フォントデータの生成と初期化
	std::unique_ptr<FontData> fontData = std::make_unique<FontData>();
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
		fontData->hCharTable[index] = Load(c, fontData->fontName, fontData->pixel);

		// 次のインデックス
		++index;
	}

	// テーブルに記録する
	fontTable_.push_back(std::move(fontData));

	return handle;
}

/// @brief 読み込み
/// @param c 
/// @param fontName 
/// @param pixel 
/// @return 
CharHandle Engine::FontStore::Load(char c, const std::string& fontName, int32_t pixel)
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

	// フォント読み込み
	HRESULT hr = FT_New_Face(ft_, charData->fontName.c_str(), 0, &charData->face);
	assert(SUCCEEDED(hr));

	// 文字生成
	hr = FT_Load_Char(charData->face, 0, charData->pixel);
	assert(SUCCEEDED(hr));

	// テーブルに記録
	charTable_.push_back(std::move(charData));

	return handle;
}