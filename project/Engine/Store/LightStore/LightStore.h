#pragma once
#include <vector>
#include <d3d12.h>
#include <dxgi1_6.h>

#include "Math/Matrix/Matrix4x4/Matrix4x4.h"

#include "Handle/Handle.h"
#include "LightData/BaseLightData.h"

#include "Application/Light/LightDirectional/LightDirectional.h"

#include "PSO/PSOShadowMap/PSOShadowMap/PSOShadowMap.h"
#include "PSO/PSOShadowMap/PSOShadowMapPrefab/PSOShadowMapPrefab.h"
#include "PSO/ComputePSO/ComputePSOLightCulling/ComputePSOLightCulling.h"

#include "Resource/ShadowMapTextureResource/ShadowMapTextureResource.h"
#include "Resource/ConstantBufferResource/ConstantBufferResource.h"

namespace Engine
{
	class DX12Heap;
	class Log;
	class ShaderCompiler;
	class DX12Model;

	class LightStore
	{
	public:

		/// @brief 初期化
		/// @param device 
		/// @param compiler 
		/// @param log 
		void Initialize(ID3D12Device* device,ID3D12GraphicsCommandList* commandList, DX12Heap* heap, ShaderCompiler* compiler, Log* log);

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
		void Update(ID3D12GraphicsCommandList* commandList, DX12Model* model, const Matrix4x4& projectionMatrix);

		/// @brief シャドウマップ用座標変換リソースを取得する
		/// @return 
		ConstantBufferResource<Matrix4x4>* GetShadowMapTransformationResource() { return shadowMapTransformationResource_.get(); }

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

		// シャドウマップPSO
		std::unique_ptr<PSOShadowMap> psoShadowMap_ = nullptr;

		/// @brief プレハブ用シャドウマップPSO
		std::unique_ptr<PSOShadowMapPrefab> psoShadowMapPrefab_ = nullptr;
		
		// ライトカリングPSO
		std::unique_ptr<ComputePSOLightCulling> psoLightCulling_ = nullptr;


	private:

		// シャドウマップ用座標変換リソース
		std::unique_ptr<ConstantBufferResource<Matrix4x4>> shadowMapTransformationResource_ = nullptr;
	};
}