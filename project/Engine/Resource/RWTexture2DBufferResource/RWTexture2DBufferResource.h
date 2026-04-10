#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <utility>

namespace Engine
{
	class Log;
	class DX12Heap;

	class RWTexture2DBufferResource
	{
	public:

		/// @brief 初期化
		/// @param device 
		/// @param commandList 
		/// @param heap 
		/// @param width 
		/// @param height 
		/// @param log 
		void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, DX12Heap* heap, uint32_t width, uint32_t height, Log* log);

		/// @brief コマンドリストに登録する
		/// @param commandList 
		/// @param rootParameterIndex 
		void RegisterGraphics(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex);

		/// @brief コマンドリストに登録する
		/// @param commandList 
		/// @param rootParameterIndex 
		void RegisterCompute(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex);

		/// @brief サイズを作り直す
		/// @param device 
		/// @param width 
		/// @param height 
		void Resize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, uint32_t width, uint32_t height);

		/// @brief バリアを張る
		/// @param commandList 
		/// @param before 
		/// @param after 
		void Barrier(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);

		/// @brief リソースを取得する
		/// @return 
		ID3D12Resource* GetResource() { return resource_.Get(); }

		/// @brief 横幅を取得する
		/// @return 
		int32_t GetWidth()const { return width_; }

		/// @brief 縦幅を取得する
		/// @return 
		int32_t GetHeight()const { return height_; }

	private:

		/// @brief リソース
		Microsoft::WRL::ComPtr<ID3D12Resource> resource_ = nullptr;

		/// @brief UAVハンドル
		std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> uavHandle_;

		/// @brief SRVハンドル
		std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> srvHandle_;

		// @brief テクスチャの幅
		int32_t width_ = 0;
		int32_t height_ = 0;
	};
}