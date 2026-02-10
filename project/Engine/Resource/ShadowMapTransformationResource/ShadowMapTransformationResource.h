#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include "Math/Matrix/Matrix4x4/Matrix4x4.h"

namespace Engine
{
	class Log;

	class ShadowMapTransformationResource
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
		Matrix4x4* data_ = nullptr;

	private:

		/// @brief リソース
		Microsoft::WRL::ComPtr<ID3D12Resource> resource_ = nullptr;
	};
}