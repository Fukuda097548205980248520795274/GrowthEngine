#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <cstdint>

namespace Engine
{
	class DX12Heap;
	class Log;

	class DX12Offscreen
	{
	public:

		/// @brief 初期化
		/// @param device 
		/// @param heap 
		/// @param log 
		void Initialize(ID3D12Device* device, DX12Heap* heap, Log* log);


		template<typename T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;

	private:

		/// @brief 最大オフスクリーン数
		static constexpr int32_t kMaxOffscreenCount = 2;

		
	};
}