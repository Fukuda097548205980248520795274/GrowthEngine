#pragma once
#include "Handle/Handle.h"
#include <string>
#include <vector>
#include <memory>
#include <ft2build.h>
#include FT_FREETYPE_H

namespace Engine
{
	/// @brief 文字データ
	struct CharData
	{
		/// @brief 文字
		char c;


		/// @brief フォント名
		std::string fontName;

		// ピクセル
		int32_t pixel;


		// FT_Face
		FT_Face face;

		// ハンドル
		CharHandle handle;
	};

	/// @brief フォントデータ
	struct FontData
	{
		/// @brief テキスト
		std::string text;


		/// @brief フォント名
		std::string fontName;

		// ピクセル
		int32_t pixel;


		// ハンドル
		FontHandle handle;

		// 文字データテーブル
		std::vector<CharHandle> hCharTable;
	};

	class FontStore
	{
	public:

		/// @brief コンストラクタ
		FontStore();

		/// @brief 読み込み
		/// @param text 
		/// @param fontName 
		/// @param pixel 
		/// @return 
		FontHandle Load(const std::string& text,const std::string& fontName, int32_t pixel);


	private:

		/// @brief 読み込み
		/// @param c 
		/// @param fontName 
		/// @param pixel 
		/// @return 
		CharHandle Load(char c, const std::string& fontName, int32_t pixel);


	private:

		// FreeType
		FT_Library ft_;


		/// @brief フォントテーブル
		std::vector<std::unique_ptr<FontData>> fontTable_;

		/// @brief 文字テーブル
		std::vector<std::unique_ptr<CharData>> charTable_;
	};
}