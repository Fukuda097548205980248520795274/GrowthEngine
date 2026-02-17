#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <cassert>

#include "Log/Log.h"
#include "Func/ResourceFunc/ResourceFunc.h"

namespace Engine
{
	/// @brief 頂点バッファリソース
	/// @tparam T 
	template<typename T>
	class VertexBufferResource
	{
	public:

		/// @brief 初期化
		/// @param device 
		/// @param numVertex 
		/// @param log 
		void Initialize(ID3D12Device* device, UINT numVertex, Log* log);

		/// @brief コマンドリストに登録する
		/// @param commandList 
		void Register(ID3D12GraphicsCommandList* commandList);


		/// @brief データ
		T* data_ = nullptr;

	private:

		/// @brief リソース
		Microsoft::WRL::ComPtr<ID3D12Resource> resource_ = nullptr;

		/// @brief ビュー
		D3D12_VERTEX_BUFFER_VIEW view_{};
	};
}

/// @brief 初期化
/// @param device 
/// @param numVertex 
/// @param log 
template<typename T>
void Engine::VertexBufferResource<T>::Initialize(ID3D12Device* device, UINT numVertex, Log* log)
{
	// nullptrチェック
	assert(device);

	// リソース生成
	resource_ = CreateBufferResource(device, sizeof(T) * numVertex, log);
	resource_->Map(0, nullptr, reinterpret_cast<void**>(&data_));

	// ビューの割り当て
	view_.BufferLocation = resource_->GetGPUVirtualAddress();
	view_.SizeInBytes = sizeof(T) * numVertex;
	view_.StrideInBytes = sizeof(T);
}

/// @brief コマンドリストに登録する
/// @param commandList 
template<typename T>
void Engine::VertexBufferResource<T>::Register(ID3D12GraphicsCommandList* commandList)
{
	commandList->IASetVertexBuffers(0, 1, &view_);
}