#pragma once
#include <vector>
#include <memory>
#include "RenderContext/DX12Offscreen/RenderPass/RenderPass.h"

namespace Engine
{
	class RenderTargetPool;
	class DX12Offscreen;

	class RenderPassStore
	{
	public:

		/// @brief 初期化
		/// @param renderTargetPool 
		void Initialize(RenderTargetPool* renderTargetPool);

		/// @brief 読み込み
		/// @param name 
		/// @return 
		RenderPassHandle Load(const std::string& name, std::function<void()> drawFunc);

		/// @brief レンダーパスに対応する描画関数を実行する
		/// @param handle 
		/// @param commandList 
		/// @param dsvHandle 
		OffscreenResource* RenderPassDraw(RenderPassHandle handle, DX12Offscreen* offscreen, ID3D12GraphicsCommandList* commandList, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle, OffscreenResource* inputResource);

		/// @brief レンダーパスに対応する描画関数を実行する
		/// @param name 
		/// @param offscreen 
		/// @param commandList 
		/// @param dsvHandle 
		OffscreenResource* RenderPassDraw(const std::string& name, DX12Offscreen* offscreen, ID3D12GraphicsCommandList* commandList, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle, OffscreenResource* inputResource);

		/// @brief レンダーパスに描画する
		/// @param handle 
		/// @param offscreen 
		/// @param commandList 
		/// @param psoFullscreen 
		void DrawToRenderPass(RenderPassHandle renderTargetHandle, RenderPassHandle sourceHandle, ID3D12GraphicsCommandList* commandList, PSOFullscreen* psoFullscreen);

		/// @brief レンダーパスに描画する
		/// @param name 
		/// @param offscreen 
		/// @param commandList 
		/// @param psoFullscreen 
		void DrawToRenderPass(const std::string& renderTargetName, const std::string& sourceName, ID3D12GraphicsCommandList* commandList, PSOFullscreen* psoFullscreen);


		/// @brief レンダーパスを返却する
		void Return();


		/// @brief レンダーパスに対応する描画関数をループで実行する
		/// @param name 
		/// @param commandList 
		/// @param dsvHandle 
		/// @param firstInput 
		/// @param loopCount 
		/// @return 
		OffscreenResource* PingPongDraw(const std::string& name,DX12Offscreen* offscreen, ID3D12GraphicsCommandList* commandList, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle, OffscreenResource* firstInput, int32_t loopCount);

		/// @brief レンダーパスを取得する
		/// @param handle 
		/// @return 
		RenderPass* GetPass(RenderPassHandle handle) { return dataTable_[handle].get(); }

		/// @brief レンダーパスを取得する
		/// @param name 
		/// @return 
		RenderPass* GetPass(const std::string& name) { return dataTable_[nameTable_.at(name)].get(); }


		/// @brief 中間リソースを解放する
		/// @param resource 
		void ReleaseIntermediateResource(OffscreenResource* resource);


	private:

		/// @brief データテーブル
		std::vector<std::unique_ptr<RenderPass>> dataTable_;

		/// @brief 名前テーブル
		std::unordered_map<std::string, RenderPassHandle> nameTable_;

		/// @brief 実行リスト
		std::vector<OffscreenResource*> activeResources_;

		/// @brief レンダーターゲットプール
		RenderTargetPool* renderTargetPool_ = nullptr;
	};
}