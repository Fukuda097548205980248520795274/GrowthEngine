#pragma once
#pragma once
#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <cstdint>
#include <memory>
#include "Math/Matrix/Matrix4x4/Matrix4x4.h"
#include "Resource/OffscreenResource/OffscreenResource.h"
#include "Resource/DepthResource/DepthResource.h"

#include "PSO/PSOModel/PSOFullscreen/PSOFullscreen.h"

#include "RenderTargetPool/RenderTargetPool.h"
#include "Store/PostEffectStore/PostEffectStore.h"
#include "Store/RenderPassStore/RenderPassStore.h"

namespace Engine
{
	class DX12Heap;
	class DX12Buffering;
	class ShaderCompiler;
	class TextureStore;
	class Camera3DStore;
	class Log;

	class DX12Offscreen
	{
	public:

		/// @brief 初期化
		/// @param device 
		/// @param heap 
		/// @param buffering 
		/// @param compiler 
		/// @param log 
		void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, 
			DX12Heap* heap, DX12Buffering* buffering, ShaderCompiler* compiler, TextureStore* textureStore, Log* log);

		/// @brief シーン前のリセット
		void PerSceneReset();

		/// @brief フレームの最後の処理
		void EndFrame(ID3D12GraphicsCommandList* commandList);

		/// @brief サイズを作り直す
		/// @param device 
		/// @param buffering 
		void Resize(ID3D12Device* device,ID3D12GraphicsCommandList* commandList, DX12Buffering* buffering);

		/// @brief クリア
		/// @param commandList 
		void Clear(ID3D12GraphicsCommandList* commandList);

		/// @brief スワップチェインのRTVリソースにオフクリーンリソースを書き込む
		/// @param commandList 
		void RenderSwapChain(ID3D12GraphicsCommandList* commandList);

		/// @brief 書き込み先のレンダーターゲットのリソースを取得する
		/// @return 
		OffscreenResource* GetDestinationResource() { return destinationResource_; }

		/// @brief 現在のレンダーターゲットのSRVハンドルを取得する
		/// @return 
		D3D12_GPU_DESCRIPTOR_HANDLE GetCurrentResourceSrvHandle() { return currentResource_->GetSrvGpuHandle(); }

		/// @brief 現在のソースリソースを設定する
		/// @param resource 
		void SetSourceResource(OffscreenResource* resource) { sourceResource_ = resource; }

		/// @brief 現在のソースリソースを取得する
		/// @return 
		OffscreenResource* GetSourceResource() { return sourceResource_; }

		/// @brief 現在のレンダーターゲットのリソースを設定する
		/// @param resource 
		void SetDestinationResource(OffscreenResource* resource) { destinationResource_ = resource; }

		/// @brief 現在のレンダーターゲットのリソースを設定する
		/// @param resource 
		void SetCurrentResource(OffscreenResource* resource) { currentResource_ = resource; }

		/// @brief 現在のレンダーターゲットのリソースを取得する
		/// @return 
		OffscreenResource* GetCurrentResource() const { return currentResource_; }

		/// @brief 深度リソースを取得する
		/// @return 
		DepthResource* GetDepthResource() { return depthResource_.get(); }

		/// @brief ポストエフェクトを読み込む
		/// @param name 
		/// @param type 
		/// @param device 
		/// @param log 
		/// @return 
		PostEffectHandle LoadPostEffect(const std::string& name, PostEffect::Type type, ID3D12Device* device,ID3D12GraphicsCommandList* commandList, Log* log) 
		{
			return postEffectStore_->Load(name, type, device, commandList, buffering_, log);
		}

		/// @brief レンダーパスを読み込む
		/// @param name 
		/// @param drawFunc 
		/// @return 
		RenderPassHandle LoadRenderPass(const std::string& name, std::function<void()> drawFunc) { return renderPassStore_->Load(name, drawFunc); }


		/// @brief 中間リソースを解放する
		/// @param resource 
		void ReleaseIntermediateResource(OffscreenResource* resource) { renderPassStore_->ReleaseIntermediateResource(resource); }


		/// @brief ポストエフェクトを描画する
		/// @param hPostEffect 
		/// @param commandList 
		void DrawPostEffect(PostEffectHandle hPostEffect, ID3D12GraphicsCommandList* commandList, const PostEffectRenderContext& context);

		/// @brief ポストエフェクトを描画する
		/// @param name 
		/// @param commandList 
		void DrawPostEffect(const std::string& name, ID3D12GraphicsCommandList* commandList, const PostEffectRenderContext& context);

		/// @brief レンダーパスを実行する
		/// @param handle 
		/// @param commandList 
		/// @param dsvHandle 
		/// @param inputResource 
		void ExecuteRenderPass(RenderPassHandle handle, ID3D12GraphicsCommandList* commandList);

		/// @brief レンダーパスを実行する
		/// @param name 
		/// @param commandList 
		/// @param dsvHandle 
		/// @param inputResource 
		void ExecuteRenderPass(const std::string& name, ID3D12GraphicsCommandList* commandList);

		/// @brief レンダーパスに描画する
		/// @param renderTargetHandle 
		/// @param sourceHandle 
		/// @param commandList 
		void DrawToRenderPass(RenderPassHandle renderTargetHandle, RenderPassHandle sourceHandle, ID3D12GraphicsCommandList* commandList);

		/// @brief レンダーパスに描画する
		/// @param renderTargetName 
		/// @param sourceName 
		/// @param commandList 
		void DrawToRenderPass(const std::string& renderTargetName, const std::string& sourceName, ID3D12GraphicsCommandList* commandList);

		/// @brief モーションベクトルを描画する
		/// @param commandList 
		/// @param render 
		/// @param prefab 
		void DrawMotionVector(ID3D12GraphicsCommandList* commandList, DX12Render* render, DX12Prefab* prefab);

		/// @brief TAAを描画する
		/// @param context 
		void DrawTAA(ID3D12GraphicsCommandList* commandList);

		/// @brief モーションブラーを描画する
		/// @param commandList 
		void DrawMotionBlur(ID3D12GraphicsCommandList* commandList);

		/// @brief 残像を描画する
		/// @param commandList 
		/// @param cameraStore
		void DrawAfterImage(ID3D12GraphicsCommandList* commandList , Camera3DStore* cameraStore);


		/// @brief ポストエフェクトのパラメータを取得する
		/// @tparam T 
		/// @param hPostEffect 
		/// @return 
		template<typename T>
		T* GetPostEffectParam(PostEffectHandle hPostEffect) { return postEffectStore_->GetParam<T>(hPostEffect); }

		/// @brief ポストエフェクトのパラメータを取得する
		/// @tparam T 
		/// @param name 
		/// @return 
		template<typename T>
		T* GetPostEffectParam(const std::string& name) { return postEffectStore_->GetParam<T>(name); }


		/// @brief レンダーパスのパラメータを取得する
		/// @param handle 
		/// @return 
		RenderPassData::Param* GetRenderPassParam(RenderPassHandle handle) { return renderPassStore_->GetPassParam(handle); }

		/// @brief レンダーパスのパラメータを取得する
		/// @param name 
		/// @return 
		RenderPassData::Param* GetRenderPassParam(const std::string& name) { return renderPassStore_->GetPassParam(name); }


		/// @brief デバッグ用パラメータ
		void DebugParameter();


		template<typename T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;


	private:

		/// @brief デプスステンシルのクリア
		/// @param commandList 
		void ClearDepthStencil(ID3D12GraphicsCommandList* commandList);


	private:

		// 読み込みのリソース
		OffscreenResource* sourceResource_ = nullptr;

		// 書き込み対象のレンダーターゲット
		OffscreenResource* destinationResource_ = nullptr;

		// 最新のパスの結果
		OffscreenResource* currentResource_ = nullptr;


		/// @brief レンダーターゲットプール
		std::unique_ptr<RenderTargetPool> renderTargetPool_ = nullptr;

		// ポストエフェクトストア
		std::unique_ptr<PostEffectStore> postEffectStore_ = nullptr;

		/// @brief レンダーパスストア
		std::unique_ptr<RenderPassStore> renderPassStore_ = nullptr;


	private:

		// 深度リソース
		std::unique_ptr<DepthResource> depthResource_ = nullptr;


	private:


		/// @brief フルスクリーンPSO
		std::unique_ptr<PSOFullscreen> psoFullscreen_ = nullptr;

		/// @brief 頂点シェーダのBlob
		ComPtr<IDxcBlob> vertexShaderBlob_ = nullptr;
		
		/// @brief ピクセルシェーダーのBlob
		ComPtr<IDxcBlob> pixelShaderBlob_ = nullptr;


	private:

		/// @brief ヒープ
		DX12Heap* heap_;

		/// @brief DX12Buffering
		DX12Buffering* buffering_;
	};
}