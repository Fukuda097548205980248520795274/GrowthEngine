#pragma once
#include "PrimitiveData/PrimitiveBaseData.h"
#include <memory>
#include <vector>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

#include "PSO/ComputePSO/ComputePSOSkinning/ComputePSOSkinning.h"

#include "Parameter/PrimitiveParameter/PrimitiveParameter.h"

class PrimitiveStaticModel;

namespace Engine
{
	class BasePSOModel;
	class BasePSOShadowMap;

	class ModelStore;
	class TextureStore;
	class AnimationStore;
	class SkeletonStore;
	class LightStore;
	class Log;
	class DX12Heap;
	class ShaderCompiler;
	class Camera3DStore;
	class SkyboxStore;

	class PrimitiveStore
	{
	public:

		/// @brief コンストラクタ
		PrimitiveStore();

		/// @brief 初期化
		/// @param device 
		/// @param compiler 
		/// @param heap 
		/// @param modelStore 
		/// @param textureStore 
		/// @param animationStore 
		/// @param skeletonStore 
		/// @param lightStore 
		/// @param log 
		void Initialize(ID3D12Device* device, ShaderCompiler* compiler, DX12Heap* heap,
			ModelStore* modelStore, TextureStore* textureStore, AnimationStore* animationStore, SkeletonStore* skeletonStore, LightStore* lightStore, Log* log);

		/// @brief 更新処理
		void Update(ID3D12GraphicsCommandList* commandList);

		/// @brief シャドウマップ用の描画処理
		/// @param commandList 
		/// @param pso 
		void ShadowMapDraw(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOShadowMap* pso);

		/// @brief プリミティブを読み込む
		/// @param device 
		/// @param commandList 
		/// @param hModel 
		/// @param hAnimation 
		/// @param hSkeleton 
		/// @param name 
		/// @param type 
		/// @param log 
		/// @return 
		PrimitiveHandle Load(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, ModelHandle hModel, AnimationHandle hAnimation, SkeletonHandle hSkeleton,
			const std::string& name, Primitive::Type type, Log* log);

		/// @brief コマンドリストに登録する
		/// @param commandList 
		/// @param handle 
		/// @param meshIndex 
		void Register(Camera3DStore* cameraStore, SkyboxStore* skyboxStore, ID3D12GraphicsCommandList* commandList, PrimitiveHandle handle, BasePSOModel* pso);

		/// @brief パラメータを取得する
		/// @tparam T 
		/// @param handle 
		/// @return 
		template<typename T>
		T* GetParam(PrimitiveHandle handle)
		{
			PrimitiveBaseData* data = dataTable_[handle].get();
			return static_cast<T*>(data->GetParam());
		}

		/// @brief デバッグ用パラメータ
		void DebugParameter();


	private:

		// データテーブル
		std::vector<std::unique_ptr<PrimitiveBaseData>> dataTable_;

		
		// スキニングPSO
		std::unique_ptr<ComputePSOSkinning> psoSkinning_ = nullptr;


		// プリミティブ用パラメータ
		std::unique_ptr<PrimitiveParameter> parameter_ = nullptr;


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

		/// @brief ライトストア
		LightStore* lightStore_ = nullptr;
	};
}