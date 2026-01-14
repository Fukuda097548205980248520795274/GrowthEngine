#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <utility>

namespace Engine
{
	class DX12Heap;
	class DX12Buffering;
	class Log;

	class OffscreenResource
	{
	public:

		/// @brief 初期化
		/// @param device 
		/// @param buffering 
		/// @param heap 
		/// @param log 
		void Initialize(ID3D12Device* device, DX12Buffering* buffering, DX12Heap* heap, Log* log);

		/// @brief レンダーターゲットの設定とクリア
		/// @param commandList 
		/// @param dsvHandle 
		void ClearRenderTarget(ID3D12GraphicsCommandList* commandList, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle);

		/// @brief RTV用CPUハンドルを取得する
		/// @return 
		D3D12_CPU_DESCRIPTOR_HANDLE GetRtvCpuHandle() { return rtvCpuHandle_; }

		/// @brief SRV用GPUハンドルを取得する
		/// @return 
		D3D12_GPU_DESCRIPTOR_HANDLE GetSrvGpuHandle() { return srvHandle_.second; }


		template<typename T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;

	private:

		// リソース
		ComPtr<ID3D12Resource> resource_ = nullptr;

		// RTV用CPUハンドル
		D3D12_CPU_DESCRIPTOR_HANDLE rtvCpuHandle_{};

		// SRV用ハンドル
		std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> srvHandle_{};
	};
}