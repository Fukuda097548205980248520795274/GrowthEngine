#pragma once
#include <vector>
#include <cstdint>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXTex.h>
#include <d3dx12.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "Func/ConvertString/ConvertString.h"

namespace Engine
{
	class Log;

	/// @brief テクスチャを読み込みデータを取得する
	/// @param filePath 
	/// @return 
	DirectX::ScratchImage LoadTextureGetMipImages(const std::string& filePath, Log* log);

	/// @brief テクスチャ用リソースを生成する
	/// @param device 
	/// @param metadata 
	/// @return 
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata, Log* log);

	/// @brief フォントテクスチャ用リソースを生成する
	/// @param device 
	/// @param g 
	/// @param log 
	/// @return 
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateFontTextureResource(ID3D12Device* device, FT_GlyphSlot g, Log* log);

	/// @brief テクスチャリソースをGPUに転送する命令をコマンドリストに登録する
	/// @param texture 
	/// @param mipImages 
	/// @param device 
	/// @param commandList 
	/// @return 
	[[nodiscard]]
	Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages,
		ID3D12Device* device, ID3D12GraphicsCommandList* commandList, Log* log);

	/// @brief フォントテクスチャ用リソースをGPUに転送する命令をコマンドリストに登録する
	/// @param fontTexture 
	/// @param g 
	/// @param device 
	/// @param commandList 
	/// @param log 
	/// @return 
	[[nodiscard]]
	Microsoft::WRL::ComPtr<ID3D12Resource> UploadFontTextureData(ID3D12Resource* fontTexture, FT_GlyphSlot g,
		ID3D12Device* device, ID3D12GraphicsCommandList* commandList, Log* log);
}