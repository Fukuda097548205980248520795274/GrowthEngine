#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

namespace Engine
{
	class DX12Heap;
	class DX12Buffering;
	class Log;

	class OffscreenResource
	{
	public:

		/// @brief 初期化
		/// @param device 
		/// @param buffering 
		/// @param heap 
		/// @param log 
		void Initialize(ID3D12Device* device, DX12Buffering* buffering, DX12Heap* heap, Log* log);

		template<typename T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;

	private:

		// リソース
		ComPtr<ID3D12Resource> resource_ = nullptr;
	};
}