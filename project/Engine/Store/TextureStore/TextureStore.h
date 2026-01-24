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

	class TextureStore
	{
	public:

		/// @brief テクスチャデータ
		struct TextureData
		{
			// テクスチャリソース
			Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;

			// 中間リソース
			Microsoft::WRL::ComPtr<ID3D12Resource> subResource = nullptr;

			// ミップイメージ
			DirectX::ScratchImage mipImages{};

			// SRV用ハンドル
			std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> srvHandle;

			// ハンドル
			TextureHandle handle{};

			// 名前
			std::string name{};
		};


	public:


		/// @brief 読み込み
		/// @param filePath 
		/// @param heap 
		/// @param device 
		/// @param commandList 
		/// @param log 
		TextureHandle Load(const std::string& filePath, DX12Heap* heap, ID3D12Device* device, ID3D12GraphicsCommandList* commandList, Log* log);

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