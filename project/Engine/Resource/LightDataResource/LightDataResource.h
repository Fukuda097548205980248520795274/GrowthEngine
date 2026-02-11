#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <utility>
#include "DataForGPU/LightDataForGPU/LightDataForGPU.h"

namespace Engine
{
	class DX12Heap;
	class Log;

	class LightDataResource
	{
	public:

		/// @brief 初期化
		/// @param device 
		/// @param heap 
		/// @param num 
		/// @param log 
		void Initialize(ID3D12Device* device, DX12Heap* heap,uint32_t num, Log* log);

		/// @brief コマンドリストに登録する
		/// @param commandList 
		/// @param rootParameterIndex 
		void Register(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex);

		/// @brief データ
		LightDataForGPU* data_ = nullptr;


	private:

		/// @brief リソース
		Microsoft::WRL::ComPtr<ID3D12Resource> resource_ = nullptr;

		/// @brief SRVハンドル
		std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> srvHandle_;
	};
}