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

#include "PSO/PSOPostEffect/PSOCopyImage/PSOCopyImage.h"

#include "Store/PostEffectStore/PostEffectStore.h"

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
		void Initialize(ID3D12Device* device, DX12Heap* heap, DX12Buffering* buffering, ShaderCompiler* compiler, TextureStore* textureStore, Log* log);

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

		/// @brief 現在使用中のオフスクリーンを取得する
		/// @return 
		ID3D12Resource* GetCurrentResource() { return offscreenResource_[currentOffscreen_]->GetResource(); }

		/// @brief 現在使用中のオフスクリーンのSRV用GPUハンドルを取得する
		/// @return 
		D3D12_GPU_DESCRIPTOR_HANDLE GetCurrentResourceSrvHandle() { return offscreenResource_[currentOffscreen_]->GetSrvGpuHandle(); }

		/// @brief ポストエフェクトを読み込む
		/// @param name 
		/// @param type 
		/// @param device 
		/// @param log 
		/// @return 
		PostEffectHandle LoadPostEffect(const std::string& name, PostEffect::Type type, ID3D12Device* device,ID3D12GraphicsCommandList* commandList, Log* log) 
		{
			return postEffectStore_->Load(name, type, device, commandList, buffering_, heap_, log);
		}


		/// @brief ポストエフェクトを描画する
		/// @param hPostEffect 
		/// @param commandList 
		void DrawPostEffect(PostEffectHandle hPostEffect, ID3D12GraphicsCommandList* commandList, const PostEffectRenderContext& context);

		/// @brief ポストエフェクトを描画する
		/// @param name 
		/// @param commandList 
		void DrawPostEffect(const std::string& name, ID3D12GraphicsCommandList* commandList, const PostEffectRenderContext& context);


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

		/// @brief デバッグ用パラメータ
		void DebugParameter();


		template<typename T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;


	private:

		/// @brief レンダーターゲットのクリア
		/// @param commandList 
		void ClearRenderTarget(ID3D12GraphicsCommandList* commandList);

		/// @brief デプスステンシルのクリア
		/// @param commandList 
		void ClearDepthStencil(ID3D12GraphicsCommandList* commandList);


	private:

		/// @brief 最大オフスクリーン数
		static constexpr int32_t kMaxOffscreenCount = 2;

		// 現在のオフスクリーン
		int32_t currentOffscreen_ = 0;

		// オフスクリーンリソース
		std::unique_ptr<OffscreenResource> offscreenResource_[kMaxOffscreenCount] = { nullptr };

		// ポストエフェクトストア
		std::unique_ptr<PostEffectStore> postEffectStore_ = nullptr;


	private:

		// 深度リソース
		std::unique_ptr<DepthResource> depthResource_ = nullptr;


	private:

		// 頂点シェーダ
		ComPtr<IDxcBlob> vertexShaderBlob_ = nullptr;


		// CopyImage PSO
		std::unique_ptr<PSOCopyImage> psoCopyImage_ = nullptr;


	private:

		/// @brief ヒープ
		DX12Heap* heap_;

		/// @brief DX12Buffering
		DX12Buffering* buffering_;
	};
}