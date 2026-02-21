#pragma once
#include "PrefabPrimitiveData/PrefabPrimitiveBaseData.h"
#include "Math/Matrix/Matrix4x4/Matrix4x4.h"
#include <memory>
#include <vector>

namespace Engine
{
	class ModelStore;
	class TextureStore;
	class AnimationStore;
	class SkeletonStore;
	class Camera3DStore;
	class LightStore;
	class ShaderCompiler;
	class DX12Heap;
	class Log;
	class BasePSOShadowMap;
	class BasePSOModel;

	class PrefabPrimitiveStore
	{
	public:

		/// @brief 初期化
		/// @param device 
		/// @param compiler 
		/// @param heap 
		/// @param modelStore 
		/// @param textureStore 
		/// @param animationStore 
		/// @param skeletonStore 
		/// @param lightStore 
		/// @param cameraStore 
		/// @param log 
		void Initialize(ID3D12Device* device, ShaderCompiler* compiler, DX12Heap* heap,
			ModelStore* modelStore, TextureStore* textureStore, AnimationStore* animationStore, SkeletonStore* skeletonStore,
			LightStore* lightStore, Camera3DStore* cameraStore, Log* log);

		/// @brief プレハブの読み込み
		/// @param device 
		/// @param commandList 
		/// @param hModel 
		/// @param hAnimation 
		/// @param hSkeleton 
		/// @param name 
		/// @param numInstance 
		/// @param type 
		/// @param log 
		PrefabPrimitiveHandle Load(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, ModelHandle hModel, AnimationHandle hAnimation, SkeletonHandle hSkeleton,
			const std::string& name, uint32_t numInstance, Prefab::Type type, Log* log);

		/// @brief 更新処理
		void Update();

		/// @brief コマンドリストに登録する
		/// @param hPrefabPrimitive 
		/// @param commandList 
		/// @param pso 
		void Register(PrefabPrimitiveHandle hPrefabPrimitive, ID3D12GraphicsCommandList* commandList, BasePSOModel* pso);

		/// @brief シャドウマップの描画処理
		/// @param viewProjection 
		/// @param commandList 
		/// @param pso 
		void ShadowMapDraw(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOShadowMap* pso);

		/// @brief リセット
		void Reset();

		/// @brief 全てのインスタンスを削除する
		void DestroyAllInstance();

		/// @brief インスタンスを生成する
		/// @tparam T 
		/// @param hPrefabPrimitive 
		/// @return 
		template<typename T>
		T* CreateInstance(PrefabPrimitiveHandle hPrefabPrimitive)
		{
			PrefabPrimitiveBaseData* data = dataTable_[hPrefabPrimitive].get();
			return static_cast<T*>(data->CreateInstance());
		}


	private:

		/// @brief データテーブル
		std::vector<std::unique_ptr<PrefabPrimitiveBaseData>> dataTable_;


	private:

		/// @brief ヒープ
		DX12Heap* heap_ = nullptr;

		/// @brief モデルストア
		ModelStore* modelStore_ = nullptr;

		/// @brief テクスチャストア
		TextureStore* textureStore_ = nullptr;

		/// @brief アニメーションストア
		AnimationStore* animationStore_ = nullptr;

		/// @brief スケルトンストア
		SkeletonStore* skeletonStore_ = nullptr;

		/// @brief カメラストア
		Camera3DStore* cameraStore_ = nullptr;

		/// @brief ライトストア
		LightStore* lightStore_ = nullptr;
	};
}