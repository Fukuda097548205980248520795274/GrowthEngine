#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

#include "Log/Log.h"
#include "Func/ResourceFunc/ResourceFunc.h"
#include "RenderContext/DX12Heap/DX12Heap.h"
#include <cassert>
#include <format>

namespace Engine
{
	/// @brief 構造化バッファリソース
	/// @tparam T 
	template<typename T>
	class StructuredBufferResource
	{
	public:

		/// @brief 初期化
		/// @param device 
		/// @param heap 
		/// @param num 
		/// @param log 
		void Initialize(ID3D12Device* device, DX12Heap* heap, UINT num, Log* log);

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

		/// @brief SRVハンドル
		std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> srvHandle_;
	};
}

/// @brief 初期化
/// @param device 
/// @param heap 
/// @param num 
/// @param log 
template <typename T>
void Engine::StructuredBufferResource<T>::Initialize(ID3D12Device* device, DX12Heap* heap, UINT num, Log* log)
{
	// nullptrチェック
	assert(device);
	assert(heap);

	// リソース作成
	resource_ = CreateBufferResource(device, sizeof(T) * num, log);

	// データを割りあてる
	resource_->Map(0, nullptr, reinterpret_cast<void**>(&data_));

	// SRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = num;
	srvDesc.Buffer.StructureByteStride = sizeof(T);

	// ハンドルを取得する
	srvHandle_.first = heap->GetSrvCPUDescriptorHandle();
	srvHandle_.second = heap->GetSrvGPUDescriptorHandle();

	// ビューの生成
	device->CreateShaderResourceView(resource_.Get(), &srvDesc, srvHandle_.first);

	// ログ出力
	if (log)
	{
		log->Logging("SRV Format : UNKNOWN");
		log->Logging("SRV Shader4ComponentMapping : DEFAULT_SHADER_4_COMPONENT_MAPPING");
		log->Logging("SRV ViewDimension : DIMENSION_BUFFER");
		log->Logging("SRV Buffer Flags : NONE");
		log->Logging(std::format("SRV Buffer FirstElement : {}", srvDesc.Buffer.FirstElement));
		log->Logging(std::format("SRV Buffer NumElements : {}", srvDesc.Buffer.NumElements));
		log->Logging(std::format("SRV Buffer StructureByteStride : {} byte", srvDesc.Buffer.StructureByteStride));
		log->Logging("Creat ShaderResourceView \n");
	}
}

/// @brief コマンドリストに登録する
/// @param commandList 
/// @param rootParameterIndex 
template <typename T>
void Engine::StructuredBufferResource<T>::RegisterGraphics(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex)
{
	commandList->SetGraphicsRootDescriptorTable(rootParameterIndex, srvHandle_.second);
}

/// @brief コマンドリストに登録する
/// @param commandList 
/// @param rootParameterIndex 
template <typename T>
void Engine::StructuredBufferResource<T>::RegisterCompute(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex)
{
	commandList->SetComputeRootDescriptorTable(rootParameterIndex, srvHandle_.second);
}