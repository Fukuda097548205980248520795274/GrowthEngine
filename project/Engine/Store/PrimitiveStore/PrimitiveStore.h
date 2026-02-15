#pragma once
#include "PrimitiveData/PrimitiveBaseData.h"
#include <memory>
#include <vector>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

#include "PSO/ComputePSO/ComputePSOSkinning/ComputePSOSkinning.h"

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

	class PrimitiveStore
	{
	public:

		/// @brief 初期化
		/// @param device 
		/// @param compiler 
		/// @param log 
		void Initialize(ID3D12Device* device, ShaderCompiler* compiler, Log* log);

		/// @brief 更新処理
		void Update(ID3D12GraphicsCommandList* commandList);

		/// @brief シャドウマップ用の描画処理
		/// @param commandList 
		/// @param pso 
		void ShadowMapDraw(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOShadowMap* pso);

		/// @brief プリミティブを読み込む
		/// @param modelStore 
		/// @param textureStore 
		/// @param device 
		/// @param hModel 
		/// @param name 
		/// @param type 
		/// @param log 
		/// @return 
		PrimitiveHandle Load(ModelStore* modelStore, TextureStore* textureStore, AnimationStore* animationStore, SkeletonStore* skeletonStore,
			ID3D12Device* device, ID3D12GraphicsCommandList* commandList, ModelHandle hModel, AnimationHandle hAnimation, SkeletonHandle hSkeleton,
			DX12Heap* heap, const std::string& name, const std::string& type, Log* log);

		/// @brief コマンドリストに登録する
		/// @param commandList 
		/// @param handle 
		/// @param meshIndex 
		void Register(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, PrimitiveHandle handle, BasePSOModel* pso, LightStore* lightStore);

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


	private:


		// データテーブル
		std::vector<std::unique_ptr<PrimitiveBaseData>> dataTable_;

		
		// スキニングPSO
		std::unique_ptr<ComputePSOSkinning> psoSkinning_ = nullptr;
	};
}