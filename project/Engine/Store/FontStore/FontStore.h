#pragma once
#include "Handle/Handle.h"
#include <string>
#include <vector>
#include <memory>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
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


		/// @brief テクスチャリソース
		Microsoft::WRL::ComPtr<ID3D12Resource> textureResource = nullptr;

		/// @brief SRVハンドル
		std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> srvHandle;

		/// @brief 転送用リソース
		Microsoft::WRL::ComPtr<ID3D12Resource> uploadResource = nullptr;
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
		/// @param device 
		/// @param commandList 
		/// @return 
		FontHandle Load(const std::string& text, const std::string& fontName, int32_t pixel, ID3D12Device* device, ID3D12GraphicsCommandList* commandList);


	private:

		/// @brief 読み込み
		/// @param c 
		/// @param fontName 
		/// @param pixel 
		/// @param device 
		/// @param commandList 
		/// @return 
		CharHandle Load(char c, const std::string& fontName, int32_t pixel, ID3D12Device* device, ID3D12GraphicsCommandList* commandList);


	private:

		// FreeType
		FT_Library ft_;


		/// @brief フォントテーブル
		std::vector<std::unique_ptr<FontData>> fontTable_;

		/// @brief 文字テーブル
		std::vector<std::unique_ptr<CharData>> charTable_;
	};
}