#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cstdint>
#include <wrl.h>

namespace Engine
{
	class Log;

	class IndexResource
	{
	public:

		/// @brief 初期化
		/// @param device デバイス
		/// @param numSurface 面の数
		void Initialize(ID3D12Device* device, uint32_t numSurface, Log* log);

		/// @brief コマンドリストに登録する
		/// @param commandList 
		void Register(ID3D12GraphicsCommandList* commandList);

		/// @brief 頂点数を取得する
		/// @return 
		UINT GetNumVertex()const { return numVertex_; }

		// データ
		uint32_t* data_ = nullptr;

	private:

		/// @brief リソース
		Microsoft::WRL::ComPtr<ID3D12Resource> resource_ = nullptr;

		/// @brief ビュー
		D3D12_INDEX_BUFFER_VIEW view_{};

		// 頂点数
		UINT numVertex_ = 0;
	};
}