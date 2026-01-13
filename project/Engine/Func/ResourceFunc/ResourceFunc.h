#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

namespace Engine
{
	class Log;

	/// @brief トランジションバリアを張る
	/// @param resource 
	/// @param before 
	/// @param after 
	/// @param commandList 
	void TransitionBarrier(ID3D12Resource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after, ID3D12GraphicsCommandList* commandList);

	/// @brief バッファリソースを生成する
	/// @param device 
	/// @param sizeInBytes 
	/// @param log 
	/// @return 
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(ID3D12Device* device, size_t sizeInBytes, Log* log);

	/// @brief UAVリソースを生成する
	/// @param device 
	/// @param sizeInBytes 
	/// @param log 
	/// @return 
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateUAVResource(ID3D12Device* device, size_t sizeInBytes, Log* log);
}