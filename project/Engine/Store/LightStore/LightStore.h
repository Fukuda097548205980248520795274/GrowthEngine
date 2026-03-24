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
#include "Resource/StructuredBufferResource/StructuredBufferResource.h"
#include "Resource/ConstantBufferResource/ConstantBufferResource.h"

#include "DataForGPU/LightDataForGPU/LightDataForGPU.h"

#include "Parameter/LightParameter/LightParameter.h"

namespace Engine
{
	class DX12Heap;
	class Log;
	class ShaderCompiler;
	class DX12Render;
	class DX12Prefab;

	class LightStore
	{
	public:

		/// @brief コンストラクタ
		LightStore();

		/// @brief 初期化
		/// @param device 
		/// @param compiler 
		/// @param log 
		void Initialize(ID3D12Device* device,ID3D12GraphicsCommandList* commandList, DX12Heap* heap, ShaderCompiler* compiler, Log* log);

		/// @brief 更新処理
		void Update();

		/// @brief フレーム毎リセット
		void FrameReset();

		/// @brief シーン前リセット
		void PerSceneReset();

		/// @brief サイズを作り直す
		/// @param device 
		/// @param width 
		/// @param height 
		void Resize(ID3D12Device* device, int32_t width, int32_t height) { shadowMapTextureResource_->Resize(device, width, height); }

		/// @brief 読み込み
		/// @param name 
		/// @param type 
		/// @return 
		LightHandle Load(const std::string& name, Light::Type type);

		/// @brief シャドウマップ
		/// @param commandList 
		/// @param model 
		/// @param prefab 
		/// @param projectionMatrix 
		void ShadowMap(ID3D12GraphicsCommandList* commandList, DX12Render* render, DX12Prefab* prefab, const Matrix4x4& projectionMatrix);

		/// @brief シャドウマップ用座標変換リソースを取得する
		/// @return 
		ConstantBufferResource<Matrix4x4>* GetShadowMapTransformationResource() { return shadowMapTransformationResource_.get(); }

		/// @brief シャドウマップテクスチャリソースを取得する
		/// @return 
		ShadowMapTextureResource* GetShadowMapTextureResource() { return shadowMapTextureResource_.get(); }

		/// @brief ライトのコマンドリスト登録
		/// @param commandList 
		/// @param numLightRootParameterIndex 
		/// @param directionalLightRootParameterIndex 
		/// @param pointLightRootParameterIndex 
		/// @param spotLightRootParameterIndex 
		void LightRegister(ID3D12GraphicsCommandList* commandList, UINT numLightRootParameterIndex,
			UINT directionalLightRootParameterIndex, UINT pointLightRootParameterIndex, UINT spotLightRootParameterIndex);

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

		/// @brief パラメータを取得する
		/// @tparam T 
		/// @param name 
		/// @return 
		template<typename T>
		T* GetParam(const std::string& name)
		{
			BaseLightData* data = dataTable_[nameTable_[name]].get();
			return static_cast<T*>(data->GetParam());
		}


	public:

		/// @brief デバッグ用パラメータ
		void DebugParameter();

		/// @brief デバッグ用の線を描画
		void DebugDrawLine();

		/// @brief デバッグ用レイピッキング
		/// @param ray 
		/// @param pickList 
		void DebugRayPicking(const Collision3D::Ray& ray, std::vector<std::pair<float, bool*>>& pickList);

		/// @brief デバッグ用Guizmo操作
		/// @param viewMatrix 
		/// @param projMatrix 
		void DebugGuizmo(const Matrix4x4& viewMatrix, const Matrix4x4& projMatrix);


	private:

		/// @brief 平行光源を設置する
		/// @param lightData 
		void SetDirection(BaseLightData* lightData);

		/// @brief ポイントライトを設置する
		/// @param lightData 
		void SetPoint(BaseLightData* lightData);

		/// @brief スポットライトを設置する
		/// @param lightData 
		void SetSpot(BaseLightData* lightData);


	private:

		/// @brief データテーブル
		std::vector<std::unique_ptr<BaseLightData>> dataTable_;

		/// @brief 名前テーブル
		std::unordered_map<std::string, LightHandle> nameTable_;

		/// @brief パラメータ
		std::unique_ptr<LightParameter> parameter_ = nullptr;


	private:

		// シャドウマップPSO
		std::unique_ptr<PSOShadowMap> psoShadowMap_ = nullptr;

		/// @brief プレハブ用シャドウマップPSO
		std::unique_ptr<PSOShadowMapPrefab> psoShadowMapPrefab_ = nullptr;


	private:

		// シャドウマップ用座標変換リソース
		std::unique_ptr<ConstantBufferResource<Matrix4x4>> shadowMapTransformationResource_ = nullptr;

		// シャドウマップテクスチャリソース
		std::unique_ptr<ShadowMapTextureResource> shadowMapTextureResource_ = nullptr;



	private:

		// 最大ライト数
		static constexpr uint32_t kNumMaxLight = 512;

		/// @brief ライト数リソース
		std::unique_ptr<ConstantBufferResource<LightNumDataForGPU>> numLightResource_ = nullptr;

		/// @brief 平行光源リソース
		std::unique_ptr<StructuredBufferResource<DirectionalLightDataForGPU>> directionalLightResource_ = nullptr;

		/// @brief ポイントライトリソース
		std::unique_ptr<StructuredBufferResource<PointLightDataForGPU>> pointLightResource_ = nullptr;

		/// @brief スポットリソース
		std::unique_ptr<StructuredBufferResource<SpotLightDataForGPU>> spotLightResource_ = nullptr;
	};
}