#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include "Math/Vector/Vector4/Vector4.h"
#include <cstdint>

struct Vector4;

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

	/// @brief 書き込み可能なテクスチャリソースを生成する
	/// @param device 
	/// @param width 
	/// @param height 
	/// @param swapChainFormat 
	/// @param rtvFormat 
	/// @param clearColor 
	/// @return 
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateRenderTextureResource(ID3D12Device* device, uint32_t width, uint32_t height,
		DXGI_FORMAT swapChainFormat, DXGI_FORMAT rtvFormat, Vector4 clearColor, Log* log);

	/// @brief 深度テクスチャリソースを生成する
	/// @param device 
	/// @param width 
	/// @param height 
	/// @return 
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height, Log* log);
}