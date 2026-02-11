#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include "DataForGPU/LightDataForGPU/LightDataForGPU.h"

namespace Engine
{
	class Log;

	class LightCullingParamResource
	{
	public:

		/// @brief 初期化
		/// @param device 
		/// @param log 
		void Initialize(ID3D12Device* device, Log* log);

		/// @brief コマンドリストに登録する
		/// @param commandList 
		/// @param rootParameterIndex 
		void Register(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex);

		/// @brief データ
		LightCullingParamForGPU* data_ = nullptr;


	private:

		// リソース
		Microsoft::WRL::ComPtr<ID3D12Resource> resource_ = nullptr;
	};
}