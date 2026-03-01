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
#include "Resource/StructuredBufferResource/StructuredBufferResource.h"
#include "Resource/RWStructuredBufferResource/RWStructuredBufferResource.h"

#include "DataForGPU/TransformationDataForGPU/TransformationDataForGPU.h"
#include "DataForGPU/MaterialDataForGPU/MaterialDataForGPU.h"
#include "DataForGPU/VertexDataForGPU/VertexDataForGPU.h"

#include "Data/SkinCluster/SkinCluster.h"

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
	class DX12Heap;
	class ComputePSOSkinning;
	class Camera3DStore;
	class SkyboxStore;

	class PrimitiveSkinningModelData : public PrimitiveBaseData
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		/// @param hModel 
		PrimitiveSkinningModelData(const std::string& name, ModelHandle hModel,AnimationHandle hAnimation, SkeletonHandle hSkeleton, PrimitiveHandle hPrimitive, PrimitiveParameter* parameter)
			: hModel_(hModel),hAnimation_(hAnimation), hSkeleton_(hSkeleton),  PrimitiveBaseData(name, hPrimitive, parameter) {
			type_ = Primitive::Type::SkinningModel;
		}

		/// @brief 初期化
		/// @param modelStore 
		/// @param device 
		void Initialize(ModelStore* modelStore, TextureStore* textureStore, AnimationStore* animationStore, SkeletonStore* skeletonStore, LightStore* lightStore,
			DX12Heap* heap, ID3D12Device* device, ID3D12GraphicsCommandList* commandList, Log* log);

		/// @brief 更新処理
		void Update() override;

		/// @brief パラメータを取得する
		/// @return 
		void* GetParam()override { return param_.get(); }

		/// @brief スキニングを行う
		/// @param commandList 
		/// @param pso 
		void Skinning(ID3D12GraphicsCommandList* commandList, ComputePSOSkinning* pso);

		/// @brief コマンドリスト
		/// @param commandList 
		/// @param pso 
		void Register(Camera3DStore* cameraStore, SkyboxStore* skyboxStore, ID3D12GraphicsCommandList* commandList, BasePSOModel* pso);

		/// @brief コマンドリスト
		/// @param commandList 
		/// @param pso 
		void Register(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOShadowMap* pso);

		/// @brief デバッグ用パラメータ
		void DebugParameter();


	private:

		// モデルハンドル
		ModelHandle hModel_ = 0;

		/// @brief アニメーションハンドル
		AnimationHandle hAnimation_ = 0;

		/// @brief スケルトンハンドル
		SkeletonHandle hSkeleton_ = 0;


	private:

		/// @brief パラメータ
		std::unique_ptr<Primitive::SkinningModel::Param> param_ = nullptr;

		// テクスチャファイルパステーブル
		std::vector<std::string> textureFilePathTable_;


		// 座標変換リソース
		std::vector<std::unique_ptr<ConstantBufferResource<PrimitiveModelTransformationDataForGPU>>> meshTransformationResources_;

		// マテリアルリソース
		std::vector<std::unique_ptr<ConstantBufferResource<PrimitiveModelMaterialDataForGPU>>> meshMaterialResources_;


		// シャドウマップ用座標変換リソース
		std::vector<std::unique_ptr<ConstantBufferResource<Matrix4x4>>> shadowMapTransformationResource_;


	private:

		/// @brief 入力頂点リソース
		std::vector<std::unique_ptr<StructuredBufferResource<VertexDataForGPU>>> inputVertexResource_;

		/// @brief 出力頂点リソース
		std::vector<std::unique_ptr<RWStructuredBufferResource<VertexDataForGPU>>> outputVertexResource_;

		/// @brief 頂点数リソース
		std::vector<std::unique_ptr<ConstantBufferResource<uint32_t>>> vertexNumResource_;

		/// @brief スキンクラスター
		std::vector<std::unique_ptr<SkinCluster>> skinClusters_;

		/// @brief スケルトン
		Skeleton skeleton_{};


	private:

		/// @brief モデルストア
		ModelStore* modelStore_ = nullptr;

		/// @brief テクスチャストア
		TextureStore* textureStore_ = nullptr;

		/// @brief アニメーションストア
		AnimationStore* animationStore_ = nullptr;

		/// @brief スケルトンストア
		SkeletonStore* skeletonStore_ = nullptr;

		/// @brief ライトストア
		LightStore* lightStore_ = nullptr;
	};
}