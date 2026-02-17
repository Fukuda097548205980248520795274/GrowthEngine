#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cstdint>
#include <wrl.h>

namespace Engine
{
	class Log;

	class IndexBufferResource
	{
	public:

		/// @brief 初期化
		/// @param device デバイス
		/// @param numIndex インデックス数
		void Initialize(ID3D12Device* device, uint32_t numIndex, Log* log);

		/// @brief コマンドリストに登録する
		/// @param commandList 
		void Register(ID3D12GraphicsCommandList* commandList);

		// データ
		uint32_t* data_ = nullptr;

	private:

		/// @brief リソース
		Microsoft::WRL::ComPtr<ID3D12Resource> resource_ = nullptr;

		/// @brief ビュー
		D3D12_INDEX_BUFFER_VIEW view_{};
	};
}