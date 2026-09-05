#pragma once
#include <memory>
#include <vector>
#include <queue>
#include <cassert>
#include "Resource/OffscreenResource/OffscreenResource.h"

namespace Engine
{
	class DX12Heap;
	class DX12Buffering;

	class RenderTargetPool
	{
	public:

		/// @brief 初期化
		/// @param device 
		/// @param heap 
		/// @param buffering 
		/// @param poolSize 
		/// @param commandList 
		void Initialize(ID3D12Device* device, DX12Heap* heap, DX12Buffering* buffering, ID3D12GraphicsCommandList* commandList, int poolSize = 3);

		/// @brief 空いているレンダーターゲットを借りる
		/// @return 
		OffscreenResource* Rent(ID3D12GraphicsCommandList* commandList);

		/// @brief 使い終わったレンダーターゲットを返却する
		/// @param resource 
		void Return(OffscreenResource* resource);

		/// @brief フレーム終了時にリソースがすべて返却されているか確認する
		void CheckMemoryLeaks();

		/// @brief サイズを作り直す
		/// @param width 
		/// @param height 
		void Resize(int width, int height, ID3D12GraphicsCommandList* commandList);


	private:

		/// @brief レンダーターゲットを作成する
		/// @param width 
		/// @param height 
		void CreateRenderTarget(int width, int height, ID3D12GraphicsCommandList* commandList);


	private:

		// レンダーターゲットのリスト
		std::vector<std::unique_ptr<OffscreenResource>> resources_;

		// レンダーターゲットのプール
		std::queue<OffscreenResource*> freeQueue_;


		/// @brief デバイス
		ID3D12Device* device_ = nullptr;

		/// @brief ヒープ
		DX12Heap* heap_ = nullptr;

		/// @brief バッファリング
		DX12Buffering* buffering_ = nullptr;
	};
}