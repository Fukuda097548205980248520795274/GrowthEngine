#pragma once
#include <vector>
#include <d3d12.h>
#include <dxgi1_6.h>

#include "Handle/Handle.h"
#include "LightData/BaseLightData.h"

#include "Application/Light/LightDirectional/LightDirectional.h"

#include "PSO/PSOShadowMap/PSOShadowMapPrimitive/PSOShadowMapPrimitive.h"
#include "PSO/ComputePSO/ComputePSOLightCulling/ComputePSOLightCulling.h"

#include "Resource/ShadowMapTextureResource/ShadowMapTextureResource.h"
#include "Resource/ShadowMapTransformationResource/ShadowMapTransformationResource.h"

namespace Engine
{
	class DX12Heap;
	class Log;
	class ShaderCompiler;
	class DX12Primitive;

	class LightStore
	{
	public:

		/// @brief 初期化
		/// @param device 
		/// @param compiler 
		/// @param log 
		void Initialize(ID3D12Device* device, ShaderCompiler* compiler, Log* log);

		/// @brief 読み込み
		/// @param name 
		/// @param type 
		/// @param heap 
		/// @param device 
		/// @param log 
		/// @return 
		LightHandle Load(const std::string& name, const std::string& type, DX12Heap* heap, ID3D12Device* device, Log* log);

		/// @brief 更新処理
		/// @param commandList 
		void Update(ID3D12GraphicsCommandList* commandList , DX12Primitive* primitive);

		/// @brief シャドウマップ用座標変換リソースを取得する
		/// @return 
		ShadowMapTransformationResource* GetShadowMapTransformationResource() { return shadowMapTransformationResource_.get(); }

		/// @brief シャドウマップテクスチャリソースを取得する
		/// @return 
		ShadowMapTextureResource* GetShadowMapTextureResource();

		/// @brief パラメータを取得する
		/// @tparam T 
		/// @param handle 
		/// @return 
		template <typename T>
		T* GetParam(LightHandle handle)
		{
			BaseLightData* data = dataTable_[handle].get();
			return static_cast<T*>(data->GetParam());
		}


	private:

		/// @brief データテーブル
		std::vector<std::unique_ptr<BaseLightData>> dataTable_;


	private:

		// プリミティブ用シャドウマップPSO
		std::unique_ptr<PSOShadowMapPrimitive> psoShadowMapPrimitive_ = nullptr;
		
		// ライトカリングPSO
		std::unique_ptr<ComputePSOLightCulling> psoLightCulling_ = nullptr;


	private:

		// シャドウマップ用座標変換リソース
		std::unique_ptr<ShadowMapTransformationResource> shadowMapTransformationResource_ = nullptr;
	};
}