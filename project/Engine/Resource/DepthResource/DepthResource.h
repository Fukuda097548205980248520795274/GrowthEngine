#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

namespace Engine
{
	class DX12Heap;
	class DX12Buffering;
	class Log;

	class DepthResource
	{
	public:

		/// @brief 初期化
		/// @param device 
		/// @param buffering 
		/// @param heap 
		/// @param log 
		void Initialize(ID3D12Device* device, DX12Buffering* buffering, DX12Heap* heap, Log* log);

		/// @brief デプスステンシルのクリア
		/// @param commandList 
		void ClearDepthStencil(ID3D12GraphicsCommandList* commandList);

		/// @brief DSV用CPUハンドルを取得する
		/// @return 
		D3D12_CPU_DESCRIPTOR_HANDLE GetDsvCpuHandle() { return dsvCPUHandle_; }


	private:

		// リソース
		Microsoft::WRL::ComPtr<ID3D12Resource> resource_ = nullptr;

		// DSV用CPUハンドル
		D3D12_CPU_DESCRIPTOR_HANDLE dsvCPUHandle_{};
	};
}