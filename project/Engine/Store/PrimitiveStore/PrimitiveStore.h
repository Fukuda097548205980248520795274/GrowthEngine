#pragma once
#include "PrimitiveData/PrimitiveBaseData.h"
#include <memory>
#include <vector>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

class PrimitiveStaticModel;

namespace Engine
{
	class BasePSOModel;
	class BasePSOShadowMap;

	class ModelStore;
	class TextureStore;
	class AnimationStore;
	class LightStore;
	class Log;

	class PrimitiveStore
	{
	public:

		/// @brief 更新処理
		/// @param viewProjection 
		void Update(const Matrix4x4& viewProjection);

		/// @brief シャドウマップ用の更新処理
		/// @param viewProjection 
		void ShadowMapUpdate(const Matrix4x4& viewProjection);

		/// @brief シャドウマップ用の描画処理
		/// @param commandList 
		/// @param pso 
		void ShadowMapDraw(ID3D12GraphicsCommandList* commandList, BasePSOShadowMap* pso);

		/// @brief プリミティブを読み込む
		/// @param modelStore 
		/// @param textureStore 
		/// @param device 
		/// @param hModel 
		/// @param name 
		/// @param type 
		/// @param log 
		/// @return 
		PrimitiveHandle Load(ModelStore* modelStore,TextureStore* textureStore,AnimationStore* animationStore, ID3D12Device* device, 
			ModelHandle hModel,AnimationHandle hAnimation, const std::string& name , const std::string& type, Log* log);

		/// @brief コマンドリストに登録する
		/// @param commandList 
		/// @param handle 
		/// @param meshIndex 
		void Register(ID3D12GraphicsCommandList* commandList, PrimitiveHandle handle, BasePSOModel* pso, LightStore* lightStore);

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
	};
}