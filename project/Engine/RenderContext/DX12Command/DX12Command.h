#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

namespace Engine
{
	class Log;

	class DX12Command
	{
	public:

		/// @brief 初期化
		/// @param device 
		/// @param log 
		void Initialize(ID3D12Device* device, Log* log);

		/// @brief コマンドキューを取得する
		/// @return 
		ID3D12CommandQueue* GetCommandQueue()const { return commandQueue_.Get(); }

		/// @brief コマンドアロケータを取得する
		/// @return 
		ID3D12CommandAllocator* GetCommandAllocator()const { return commandAllocator_.Get(); }

		/// @brief コマンドリストを取得する
		/// @return 
		ID3D12GraphicsCommandList* GetCommandList()const { return commandList_.Get(); }

		// Microsoft::WRL 省略
		template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;


	private:


		// コマンドキュー
		ComPtr<ID3D12CommandQueue> commandQueue_ = nullptr;

		// コマンドアロケータ
		ComPtr<ID3D12CommandAllocator> commandAllocator_;

		// コマンドリスト
		ComPtr<ID3D12GraphicsCommandList> commandList_;
	};
}