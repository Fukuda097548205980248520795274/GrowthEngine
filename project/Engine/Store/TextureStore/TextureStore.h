#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXTex.h>
#include <d3dx12.h>
#include <wrl.h>
#include "Handle/Handle.h"

namespace Engine
{
	class DX12Heap;
	class Log;

	/// @brief テクスチャの種類
	enum class TextureType
	{
		Texture2D,
		Cubemap
	};

	class TextureStore
	{
	public:

		/// @brief テクスチャデータ
		struct TextureData
		{
			/// @brief リソース
			Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;

			/// @brief 中間リソース
			Microsoft::WRL::ComPtr<ID3D12Resource> subResource = nullptr;

			/// @brief みっぷイメージ
			DirectX::ScratchImage mipImages{};

			/// @brief SRVハンドル
			std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> srvHandle;

			// ハンドル
			TextureHandle handle{};

			// 名前
			std::string name{};

			/// @brief 種類
			TextureType type_;
		};


	public:


		/// @brief 読み込み
		/// @param filePath 
		/// @param heap 
		/// @param device 
		/// @param commandList 
		/// @param log 
		TextureHandle Load(const std::string& filePath, DX12Heap* heap, ID3D12Device* device, ID3D12GraphicsCommandList* commandList, Log* log);

		/// @brief ファイルパスを取得する
		/// @param hTexture 
		/// @return 
		std::string GetFilePath(TextureHandle hTexture)const { return dataTable_[hTexture]->name; }

		/// @brief SRV用GPUハンドルを取得する
		/// @param handle 
		/// @return 
		D3D12_GPU_DESCRIPTOR_HANDLE GetSrvGpuHandle(TextureHandle handle) { return dataTable_[handle]->srvHandle.second; }

		/// @brief テクスチャの横幅を取得する
		/// @param handle 
		/// @return 
		size_t GetTextureWidth(TextureHandle handle)const { return dataTable_[handle]->mipImages.GetMetadata().width; }

		/// @brief テクスチャの縦幅を取得する
		/// @param handle 
		/// @return 
		size_t GetTextureHeight(TextureHandle handle)const { return dataTable_[handle]->mipImages.GetMetadata().height; }

		/// @brief ハンドルを取得する
		/// @param filePath 
		/// @return 
		TextureHandle GetHandle(const std::string& filePath);

		/// @brief 種類の取得
		/// @param handle 
		/// @return 
		TextureType GetType(TextureHandle handle)const { return dataTable_[handle]->type_; }

		/// @brief UIを描画する
		void DrawUI();


	private:

		/// @brief テクスチャのハッシュ値を計算する
		/// @param image 
		/// @return 
		size_t CalculateTextureHash(const DirectX::Image& image);


	private:

		
		// データテーブル
		std::vector<std::unique_ptr<TextureData>> dataTable_;

		// テクスチャ数
		int numTexture_ = 0;
	};
}