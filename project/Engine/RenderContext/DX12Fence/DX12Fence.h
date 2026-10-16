#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <cstdint>

namespace Engine
{
	/// @brief ログ
	class Log;

	/// @brief DXフェンス
	class DX12Fence
	{
	public:

		/// @brief デストラクタ
		~DX12Fence();

		/// @brief 初期化
		/// @param log 
		/// @param device 
		void Initialize(Log* log, ID3D12Device* device);

		/// @brief GPUにシグナルを送る
		/// @param commandQueue 
		void SendSignal(ID3D12CommandQueue* commandQueue);

		/// @brief GPUを待つ
		void WaitGPU();

		// Microsoft::WRL 省略
		template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;


	private:

		// フェンス
		ComPtr<ID3D12Fence> fence_ = nullptr;

		// フェンスの値
		uint64_t fenceValue_ = 0;

		// フェンスのイベント
		HANDLE fenceEvent_;
	};
}