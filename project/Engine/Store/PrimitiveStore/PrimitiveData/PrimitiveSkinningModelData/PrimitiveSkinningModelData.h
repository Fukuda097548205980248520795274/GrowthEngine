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
	class AnimationStore;
	class SkeletonStore;

	class PrimitiveSkinningModelData : public PrimitiveBaseData
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		/// @param hModel 
		PrimitiveSkinningModelData(const std::string& name, ModelHandle hModel, AnimationHandle hAnimation, PrimitiveHandle hPrimitive, SkeletonHandle hSkeleton)
			: hModel_(hModel), hAnimation_(hAnimation), hSkeleton_(hSkeleton), hPrimitive_(hPrimitive), PrimitiveBaseData(name) {
			typeName_ = "SkinningModel";
		}

		/// @brief 初期化
		/// @param modelStore 
		/// @param device 
		void Initialize(ModelStore* modelStore, TextureStore* textureStore, AnimationStore* animationStore,SkeletonStore* skeletonStore, ID3D12Device* device, Log* log);

		/// @brief 更新処理
		void Update() override;

		/// @brief パラメータを取得する
		/// @return 
		void* GetParam()override { return param_.get(); }

		/// @brief コマンドリスト
		/// @param commandList 
		/// @param pso 
		void Register(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOModel* pso, LightStore* lightStore);

		/// @brief コマンドリスト
		/// @param commandList 
		/// @param pso 
		void Register(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOShadowMap* pso);


	private:

		// モデルハンドル
		ModelHandle hModel_ = 0;

		// プリミティブハンドル
		PrimitiveHandle hPrimitive_ = 0;

		/// @brief アニメーションハンドル
		AnimationHandle hAnimation_ = 0;

		/// @brief スケルトンハンドル
		SkeletonHandle hSkeleton_ = 0;


	private:

		/// @brief パラメータ
		std::unique_ptr<SkinningModel::Param> param_ = nullptr;


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

		/// @brief アニメーションストア
		AnimationStore* animationStore_ = nullptr;

		/// @brief スケルトンストア
		SkeletonStore* skeletonStore_ = nullptr;
	};
}