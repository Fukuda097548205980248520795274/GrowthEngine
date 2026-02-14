#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

#include "Log/Log.h"
#include "Func/ResourceFunc/ResourceFunc.h"
#include <cassert>

namespace Engine
{
	/// @brief 定数バッファ
	/// @tparam T 
	template<typename T>
	class ConstantBufferResource
	{
	public:

		/// @brief 初期化
		/// @param device 
		/// @param log 
		void Initialize(ID3D12Device* device, Log* log);

		/// @brief コマンドリストに登録する
		/// @param commandList 
		/// @param rootParameterIndex 
		void RegisterGraphics(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex);

		/// @brief コマンドリストに登録する
		/// @param commandList 
		/// @param rootParameterIndex 
		void RegisterCompute(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex);

		/// @brief データ
		T* data_ = nullptr;


	private:

		/// @brief リソース
		Microsoft::WRL::ComPtr<ID3D12Resource> resource_ = nullptr;
	};
}


/// @brief 初期化
/// @param device 
/// @param log 
template<typename T>
void Engine::ConstantBufferResource<T>::Initialize(ID3D12Device* device, Log* log)
{
	// nullptrチェック
	assert(device);

	// リソース生成
	resource_ = CreateBufferResource(device, sizeof(T), log);

	// データ割り当て
	resource_->Map(0, nullptr, reinterpret_cast<void**>(&data_));
}

/// @brief コマンドリストに登録する
/// @param commandList 
/// @param rootParameterIndex 
template<typename T>
void Engine::ConstantBufferResource<T>::RegisterGraphics(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex)
{
	commandList->SetGraphicsRootConstantBufferView(rootParameterIndex, resource_->GetGPUVirtualAddress());
}

/// @brief コマンドリストに登録する
/// @param commandList 
/// @param rootParameterIndex 
template<typename T>
void Engine::ConstantBufferResource<T>::RegisterCompute(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex)
{
	commandList->SetComputeRootConstantBufferView(rootParameterIndex, resource_->GetGPUVirtualAddress());
}