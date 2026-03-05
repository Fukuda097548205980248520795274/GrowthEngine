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

		/// @brief サイズを作り直す
		/// @param device 
		/// @param buffering 
		void Resize(ID3D12Device* device, DX12Buffering* buffering);

		/// @brief レンダーターゲットの設定とクリア
		/// @param commandList 
		/// @param dsvHandle 
		void ClearRenderTarget(ID3D12GraphicsCommandList* commandList, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle);

		/// @brief コマンドリストに登録する
		/// @param commandList 
		/// @param rootParameterIndex 
		void Register(ID3D12GraphicsCommandList* commandList , UINT rootParameterIndex);

		/// @brief RTV用CPUハンドルを取得する
		/// @return 
		D3D12_CPU_DESCRIPTOR_HANDLE GetRtvCpuHandle() { return rtvCpuHandle_; }

		/// @brief SRV用GPUハンドルを取得する
		/// @return 
		D3D12_GPU_DESCRIPTOR_HANDLE GetSrvGpuHandle() { return srvHandle_.second; }

		/// @brief リソースを取得する
		/// @return 
		ID3D12Resource* GetResource() { return resource_.Get(); }


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