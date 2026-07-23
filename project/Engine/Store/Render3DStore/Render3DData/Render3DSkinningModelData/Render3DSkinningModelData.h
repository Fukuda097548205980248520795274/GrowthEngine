#pragma once
#include "../Render3DBaseData.h"
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

	class Render3DSkinningModelData : public Render3DBaseData
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		/// @param hModel 
		Render3DSkinningModelData(const std::string& name, ModelHandle hModel,AnimationHandle hAnimation, SkeletonHandle hSkeleton, Render3DHandle hRender3D, Render3DParameter* parameter)
			: hModel_(hModel),hAnimation_(hAnimation), hSkeleton_(hSkeleton), Render3DBaseData(name, hRender3D, parameter) {
			type_ = Render3D::Type::SkinningModel;
		}

		/// @brief 初期化
		/// @param modelStore 
		/// @param device 
		void Initialize(ModelStore* modelStore, TextureStore* textureStore, AnimationStore* animationStore, SkeletonStore* skeletonStore, LightStore* lightStore,
			DX12Heap* heap, ID3D12Device* device, ID3D12GraphicsCommandList* commandList, Log* log);

		/// @brief 更新処理
		void Update() override;

		/// @brief リセット
		void Reset() override;

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
		void Register(Camera3DStore* cameraStore, SkyboxStore* skyboxStore, ID3D12GraphicsCommandList* commandList, BasePSOModel* pso) override;

		/// @brief コマンドリスト
		/// @param commandList 
		/// @param pso 
		void Register(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOShadowMap* pso) override;

		/// @brief コマンドリストに登録
		/// @param commandList 
		/// @param pso 
		void RegisterMotionVector(ID3D12GraphicsCommandList* commandList, BasePSOMotionVector* pso) override;

		/// @brief アウトライン用のコマンドリストに登録
		/// @param commandList 
		/// @param cameraStore 
		/// @param pso 
		void RegisterOutline(ID3D12GraphicsCommandList* commandList, BasePSOOutline* pso) override;

		/// @brief ボーンのワールド行列を取得する
		/// @param name 
		/// @return 
		Matrix4x4 GetBoneWorldMatrix(const std::string& name) override;


	public:

		/// @brief デバッグ用パラメータ
		void DebugParameter() override;


	private:

		// モデルハンドル
		ModelHandle hModel_ = 0;

		/// @brief アニメーションハンドル
		AnimationHandle hAnimation_ = 0;

		/// @brief スケルトンハンドル
		SkeletonHandle hSkeleton_ = 0;


	private:

		/// @brief パラメータ
		std::unique_ptr<Render3D::SkinningModel::Param> param_ = nullptr;

		// テクスチャファイルパステーブル
		std::vector<std::string> textureFilePathTable_;


		// 座標変換リソース
		std::vector<std::unique_ptr<ConstantBufferResource<PrimitiveModelTransformationDataForGPU>>> meshTransformationResources_;

		// マテリアルリソース
		std::vector<std::unique_ptr<ConstantBufferResource<PrimitiveModelMaterialDataForGPU>>> meshMaterialResources_;

		// シャドウマップ用座標変換リソース
		std::vector<std::unique_ptr<ConstantBufferResource<Matrix4x4>>> shadowMapTransformationResource_;

		/// @brief モーションベクター用リソース
		std::vector<std::unique_ptr<ConstantBufferResource<MotionVectorDataForGPU>>> motionVectorResources_;

		// アウトライン用座標変換リソース
		std::vector<std::unique_ptr<ConstantBufferResource<Matrix4x4>>> outlineTransformationResources_;

		// アウトライン用色リソース
		std::vector<std::unique_ptr<ConstantBufferResource<Vector4>>> outlineColorResources_;


	private:

		/// @brief 入力頂点リソース
		std::vector<std::unique_ptr<StructuredBufferResource<VertexDataForGPU>>> inputVertexResource_;

		/// @brief 出力頂点リソース
		std::vector<std::unique_ptr<RWStructuredVertexBufferResource<VertexDataForGPU>>> outputVertexResource_;

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