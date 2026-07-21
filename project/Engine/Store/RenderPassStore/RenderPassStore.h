#pragma once
#include <vector>
#include <memory>
#include "RenderPassData/RenderPassData.h"

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
		OffscreenResource* RenderPassDraw(RenderPassHandle handle, DX12Offscreen* offscreen, ID3D12GraphicsCommandList* commandList, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle);

		/// @brief レンダーパスに対応する描画関数を実行する
		/// @param name 
		/// @param offscreen 
		/// @param commandList 
		/// @param dsvHandle 
		OffscreenResource* RenderPassDraw(const std::string& name, DX12Offscreen* offscreen, ID3D12GraphicsCommandList* commandList, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle);

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


		/// @brief レンダーパスを取得する
		/// @param handle 
		/// @return 
		RenderPassData* GetPass(RenderPassHandle handle) { return dataTable_[handle].get(); }

		/// @brief レンダーパスを取得する
		/// @param name 
		/// @return 
		RenderPassData* GetPass(const std::string& name) { return dataTable_[nameTable_.at(name)].get(); }


		/// @brief レンダーパスのパラメータを取得する
		/// @param handle 
		/// @return 
		RenderPassData::Param* GetPassParam(RenderPassHandle handle) { return dataTable_[handle]->GetParam(); }

		/// @brief レンダーパスのパラメータを取得する
		/// @param name 
		/// @return 
		RenderPassData::Param* GetPassParam(const std::string& name) { return dataTable_[nameTable_.at(name)]->GetParam(); }


		/// @brief 中間リソースを解放する
		/// @param resource 
		void ReleaseIntermediateResource(OffscreenResource* resource);

		/// @brief ソースリソースを返却する
		/// @param resource 
		void ReturnSourceResource(OffscreenResource* resource);

		/// @brief アクティブなリソースを設定する
		/// @param resource 
		void SetActiveResources(OffscreenResource* resource) { activeResources_.push_back(resource); }


	private:

		/// @brief データテーブル
		std::vector<std::unique_ptr<RenderPassData>> dataTable_;

		/// @brief 名前テーブル
		std::unordered_map<std::string, RenderPassHandle> nameTable_;

		/// @brief 実行リスト
		std::vector<OffscreenResource*> activeResources_;

		/// @brief レンダーターゲットプール
		RenderTargetPool* renderTargetPool_ = nullptr;
	};
}