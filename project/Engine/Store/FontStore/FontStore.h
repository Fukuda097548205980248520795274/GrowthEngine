#pragma once
#include "Handle/Handle.h"
#include "Math/Vector/VectorInt2/VectorInt2.h"
#include <string>
#include <vector>
#include <memory>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <unordered_map>
#include <ft2build.h>
#include FT_FREETYPE_H

namespace Engine
{
	class Log;
	class DX12Heap;

	/// @brief 文字データ
	struct CharData
	{
		/// @brief 文字
		char c;

		// 文字列
		std::string str;


		/// @brief フォント名
		std::string fontName;

		// ピクセル
		int32_t pixel;

		/// @brief サイズ
		VectorInt2 size;

		/// @brief 余白
		VectorInt2 bearing;

		/// @brief 文字送り幅
		int32_t advance;

		// ハンドル
		CharHandle handle;


		/// @brief テクスチャリソース
		Microsoft::WRL::ComPtr<ID3D12Resource> textureResource = nullptr;

		/// @brief SRVハンドル
		std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> srvHandle;

		/// @brief 転送用リソース
		Microsoft::WRL::ComPtr<ID3D12Resource> uploadResource = nullptr;
	};

	/// @brief テキストデータ
	struct TextData
	{
		/// @brief テキスト
		std::string text;


		/// @brief フォント名
		std::string fontName;

		// ピクセル
		int32_t pixel;


		// ハンドル
		TextHandle handle;

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
		/// @param device 
		/// @param commandList 
		/// @return 
		TextHandle Load(const std::string& text, const std::string& fontName, int32_t pixel,
			ID3D12Device* device, ID3D12GraphicsCommandList* commandList, DX12Heap* heap, Log* log);

		/// @brief テキストデータを取得する
		/// @param hText 
		/// @return 
		TextData* GetTextData(TextHandle hText) const { return textTable_[hText].get(); }

		/// @brief 文字データを取得する
		/// @param hChar 
		/// @return 
		CharData* GetCharData(CharHandle hChar) const { return charTable_[hChar].get(); }


	private:

		/// @brief 読み込み
		/// @param c 
		/// @param fontName 
		/// @param pixel 
		/// @param device 
		/// @param commandList 
		/// @return 
		CharHandle Load(char c, const std::string& fontName, int32_t pixel, FT_Face face,
			ID3D12Device* device, ID3D12GraphicsCommandList* commandList, DX12Heap* heap, Log* log);


	private:

		// FreeType
		FT_Library ft_;


		/// @brief テキストテーブル
		std::vector<std::unique_ptr<TextData>> textTable_;

		/// @brief 文字テーブル
		std::vector<std::unique_ptr<CharData>> charTable_;

		/// @brief フォントテーブル
		std::unordered_map<std::string, FT_Face> fontTable_;
	};
}