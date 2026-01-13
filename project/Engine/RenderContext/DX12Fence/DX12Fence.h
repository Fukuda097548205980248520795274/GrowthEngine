#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <cstdint>

namespace Engine
{
	class Log;

	class DX12Fence
	{
	public:

		/// <summary>
		/// デストラクタ
		/// </summary>
		~DX12Fence();

		/// <summary>
		/// 初期化
		/// </summary>
		/// <param name="device"></param>
		void Initialize(Log* log, ID3D12Device* device);

		/// <summary>
		/// GPUにシグナルを送る
		/// </summary>
		void SendSignal(ID3D12CommandQueue* commandQueue);

		/// <summary>
		/// GPUを待つ
		/// </summary>
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