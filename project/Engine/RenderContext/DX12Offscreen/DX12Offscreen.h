#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <cstdint>
#include <memory>
#include "Resource/OffscreenResource/OffscreenResource.h"
#include "Resource/DepthResource/DepthResource.h"

namespace Engine
{
	class DX12Heap;
	class DX12Buffering;
	class Log;

	class DX12Offscreen
	{
	public:

		/// @brief 初期化
		/// @param device 
		/// @param heap 
		/// @param buffering 
		/// @param log 
		void Initialize(ID3D12Device* device, DX12Heap* heap, DX12Buffering* buffering, Log* log);

		/// @brief クリア
		/// @param commandList 
		void Clear(ID3D12GraphicsCommandList* commandList);


		template<typename T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;


	private:

		/// @brief レンダーターゲットのクリア
		/// @param commandList 
		void ClearRenderTarget(ID3D12GraphicsCommandList* commandList);

		/// @brief デプスステンシルのクリア
		/// @param commandList 
		void ClearDepthStencil(ID3D12GraphicsCommandList* commandList);


	private:

		/// @brief 最大オフスクリーン数
		static constexpr int32_t kMaxOffscreenCount = 2;

		// 現在のオフスクリーン
		int32_t currentOffscreen_ = 0;

		// オフスクリーンリソース
		std::unique_ptr<OffscreenResource> offscreenResource_[kMaxOffscreenCount] = { nullptr };


	private:

		// 深度リソース
		std::unique_ptr<DepthResource> depthResource_ = nullptr;
	};
}