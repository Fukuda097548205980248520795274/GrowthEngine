#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <cstdint>
#include <utility>

namespace Engine
{
	class DX12Heap;
	class Log;

	class ShadowMapTextureResource
	{
	public:

		/// @brief 初期化
		/// @param directXHeap 
		/// @param device 
		void Initialize(DX12Heap* heap, ID3D12Device* device, int32_t width, int32_t height, Log* log);

		/// @brief デプスステンシルのクリア
		/// @param commandList 
		void ClearDepthStencil(ID3D12GraphicsCommandList* commandList);

		/// @brief SRVのGPUハンドルを取得する
		/// @return 
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandleSRV() { return srvHandle_.second; }

	private:


		// リソース
		Microsoft::WRL::ComPtr<ID3D12Resource> resource_ = nullptr;

		// SRVハンドル
		std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> srvHandle_;

		// DSVハンドル
		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle_{};
	};
}