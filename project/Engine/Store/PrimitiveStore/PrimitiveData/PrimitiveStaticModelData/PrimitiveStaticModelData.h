#pragma once
#include "../PrimitiveBaseData.h"
#include "Handle/Handle.h"
#include <memory>
#include <vector>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include "Data/PrimitiveData/PrimitiveData.h"

#include "Resource/ConstantBufferResource/ConstantBufferResource.h"
#include "DataForGPU/TransformationDataForGPU/TransformationDataForGPU.h"
#include "DataForGPU/MaterialDataForGPU/MaterialDataForGPU.h"

namespace Engine
{
	class ModelStore;
	class BasePSOModel;
	class BasePSOShadowMap;
	class Log;
	class TextureStore;
	class LightStore;

	class PrimitiveStaticModelData : public PrimitiveBaseData
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		/// @param hModel 
		PrimitiveStaticModelData(const std::string& name , ModelHandle hModel, PrimitiveHandle hPrimitive) 
			: hModel_(hModel), hPrimitive_(hPrimitive), PrimitiveBaseData(name) { typeName_ = "StaticModel"; }

		/// @brief 初期化
		/// @param modelStore 
		/// @param device 
		void Initialize(ModelStore* modelStore, TextureStore* textureStore, ID3D12Device* device, Log* log);

		/// @brief 更新処理
		/// @param viewProjection 
		void Update(const Matrix4x4& viewProjection) override;

		/// @brief シャドウマップ用更新処理
		/// @param viewProjection 
		void ShadowMapUpdate(const Matrix4x4& viewProjection) override;

		/// @brief パラメータを取得する
		/// @return 
		void* GetParam()override { return param_.get(); }

		/// @brief コマンドリスト
		/// @param commandList 
		/// @param pso 
		void Register(ID3D12GraphicsCommandList* commandList, BasePSOModel* pso, LightStore* lightStore);

		/// @brief コマンドリスト
		/// @param commandList 
		/// @param pso 
		void Register(ID3D12GraphicsCommandList* commandList, BasePSOShadowMap* pso);


	private:

		// モデルハンドル
		ModelHandle hModel_ = 0;

		// 静的モデルハンドル
		PrimitiveHandle hPrimitive_ = 0;


	private:

		/// @brief パラメータ
		std::unique_ptr<StaticModel::Param> param_ = nullptr;

		
		// 座標変換リソース
		std::vector<std::unique_ptr<ConstantBufferResource<PrimitiveModelTransformationDataForGPU>>> meshTransformationResources_;

		// マテリアルリソース
		std::vector<std::unique_ptr<ConstantBufferResource<PrimitiveModelMaterialDataForGPU>>> meshMaterialResources_;


	private:

		// シャドウマップ用座標変換リソース
		std::vector<std::unique_ptr<ConstantBufferResource<Matrix4x4>>> shadowMapTransformationResource_;


	private:

		/// @brief モデルストア
		ModelStore* modelStore_ = nullptr;

		/// @brief テクスチャストア
		TextureStore* textureStore_ = nullptr;
	};
}