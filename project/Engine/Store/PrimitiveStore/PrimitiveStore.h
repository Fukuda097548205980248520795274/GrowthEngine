#pragma once
#include "PrimitiveData/PrimitiveStaticModelData/PrimitiveStaticModelData.h"

class PrimitiveStaticModel;

namespace Engine
{
	class BasePSOModel;
	class BasePSOShadowMap;

	class ModelStore;
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


	public:

		/// @brief 静的モデル読み込み
		/// @param model 
		/// @param modelStore 
		/// @param textureStore 
		/// @param device 
		/// @param hModel 
		/// @param name 
		/// @param log 
		/// @return 
		PrimitiveStaticModelHandle Load(PrimitiveStaticModel* model, ModelStore* modelStore,TextureStore* textureStore, ID3D12Device* device, 
			ModelHandle hModel, const std::string& name , Log* log);

		/// @brief コマンドリストに登録する
		/// @param commandList 
		/// @param handle 
		/// @param meshIndex 
		void Register(ID3D12GraphicsCommandList* commandList, PrimitiveStaticModelHandle handle, BasePSOModel* pso) { staticModelTable_[handle]->Register(commandList, pso); }


	private:


		// 静的モデルテーブル
		std::vector<std::unique_ptr<PrimitiveStaticModelData>> staticModelTable_;
	};
}