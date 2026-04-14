#pragma once
#include "../Render3DBaseData.h"

namespace Engine
{
	class ModelStore;
	class BasePSOModel;
	class BasePSOShadowMap;
	class Log;
	class TextureStore;
	class LightStore;
	class AnimationStore;
	class Camera3DStore;
	class SkyboxStore;

	class Render3DAnimationModelData : public Render3DBaseData
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		/// @param hModel 
		Render3DAnimationModelData(const std::string& name, ModelHandle hModel, AnimationHandle hAnimation, Render3DHandle hRender3D, Render3DParameter* parameter)
			: hModel_(hModel),hAnimation_(hAnimation), Render3DBaseData(name, hRender3D, parameter) {
			type_ = Render3D::Type::AnimationModel;
		}

		/// @brief 初期化
		/// @param modelStore 
		/// @param device 
		void Initialize(ModelStore* modelStore, TextureStore* textureStore, AnimationStore* animationStore, LightStore* lightStore, ID3D12Device* device, Log* log);

		/// @brief 更新処理
		void Update() override;

		/// @brief リセット
		void Reset() override;

		/// @brief パラメータを取得する
		/// @return 
		void* GetParam()override { return param_.get(); }

		/// @brief コマンドリストに登録する
		/// @param cameraStore 
		/// @param commandList 
		/// @param pso 
		void Register(Camera3DStore* cameraStore, SkyboxStore* skyboxStore, ID3D12GraphicsCommandList* commandList, BasePSOModel* pso);

		/// @brief コマンドリストに登録する
		/// @param viewProjection 
		/// @param commandList 
		/// @param pso 
		void Register(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOShadowMap* pso);


	public:

		/// @brief デバッグ用パラメータ
		void DebugParameter() override;

		/// @brief デバッグ用レイピッキング
		/// @param ray 
		/// @param pickList 
		void DebugRayPicker(const Collision3D::Ray& ray, std::vector<std::pair<float, DebugData::DebugGuizmoData*>>& pickList) override;

		/// @brief Guizmo操作
		/// @param cameraStore 
		void DebugGuizmo(Camera3DStore* cameraStore) override;


	private:

		// モデルハンドル
		ModelHandle hModel_ = 0;

		/// @brief アニメーションハンドル
		AnimationHandle hAnimation_ = 0;


	private:

		/// @brief パラメータ
		std::unique_ptr<Render3D::AnimationModel::Param> param_ = nullptr;

		// テクスチャファイルパステーブル
		std::vector<std::string> textureFilePathTable_;


		// 座標変換リソース
		std::vector<std::unique_ptr<ConstantBufferResource<PrimitiveModelTransformationDataForGPU>>> meshTransformationResources_;

		// マテリアルリソース
		std::vector<std::unique_ptr<ConstantBufferResource<PrimitiveModelMaterialDataForGPU>>> meshMaterialResources_;

		// シャドウマップ用座標変換リソース
		std::vector<std::unique_ptr<ConstantBufferResource<Matrix4x4>>> shadowMapTransformationResource_;


	private:

		/// @brief モデルストア
		ModelStore* modelStore_ = nullptr;

		/// @brief テクスチャストア
		TextureStore* textureStore_ = nullptr;

		/// @brief アニメーションストア
		AnimationStore* animationStore_ = nullptr;

		/// @brief ライトストア
		LightStore* lightStore_ = nullptr;
	};
}