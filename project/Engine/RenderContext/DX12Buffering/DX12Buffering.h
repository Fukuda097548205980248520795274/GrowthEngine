#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <cstdint>

namespace Engine
{
	class Log;
	class WinApp;
	class DX12Heap;

	class DX12Buffering
	{
	public:

		/// @brief 初期化
		/// @param log 
		/// @param heap 
		/// @param winApp 
		/// @param dxgiFactory 
		/// @param device 
		/// @param commandQueue 
		void Initialize(Log* log, DX12Heap* heap,WinApp* winApp,
			IDXGIFactory7* dxgiFactory, ID3D12Device* device, ID3D12CommandQueue* commandQueue);

		/// @brief スワップチェーンを取得する
		/// @return 
		IDXGISwapChain4* GetSwapChain()const { return swapChain_.Get(); }

		/// @brief スワップチェーンの設定の取得
		/// @return 
		DXGI_SWAP_CHAIN_DESC1 GetSwapChainDesc()const { return swapChainDesc_; }

		/// @brief スワップチェーンRTVのCPUハンドルの取得
		/// @param index 
		/// @return 
		D3D12_CPU_DESCRIPTOR_HANDLE GetSwapChainRtvCPUHandle(UINT index)const { return rtvCPUHandle_[index]; }

		/// @brief スワップチェーンのリソースを取得する
		/// @param index 
		/// @return 
		ID3D12Resource* GetSwapChainResource(UINT index)const { return swapChainResource_[index].Get(); }

		/// @brief RTVの設定を取得する
		/// @return 
		D3D12_RENDER_TARGET_VIEW_DESC GetRtvDesc()const { return rtvDesc_; }


		// Microsoft::WRL 省略
		template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	private:


		// スワップチェーンの設定
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc_{};

		// スワップチェーン
		ComPtr<IDXGISwapChain4> swapChain_ = nullptr;

		// スワップチェーンのリソース
		ComPtr<ID3D12Resource> swapChainResource_[2] = { nullptr };

		// CPUハンドル
		D3D12_CPU_DESCRIPTOR_HANDLE rtvCPUHandle_[2]{};


		// RTV
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_{};
	};
}