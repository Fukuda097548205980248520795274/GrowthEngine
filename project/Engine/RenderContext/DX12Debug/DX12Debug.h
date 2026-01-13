#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

namespace Engine
{
	class Log;

	class DX12Debug
	{
	public:

		/// @brief 初期化
		/// @param log 
		void Initialize(Log* log);

		/// @brief 警告・エラーで停止させる
		/// @param device 
		void Stop(ID3D12Device* device);

		// Microsoft::WRL 省略
		template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;


	private:

		// デバッグコントローラ
		ComPtr<ID3D12Debug1> debugController_ = nullptr;
	};
}