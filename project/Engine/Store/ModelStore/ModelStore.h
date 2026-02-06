#pragma once
#include "ModelResource/ModelResource.h"

namespace Engine
{
	class TextureStore;
	class Log;
	class DX12Heap;

	class ModelStore
	{
	public:

		/// @brief 読み込み
		/// @param directory 
		/// @param fileName 
		/// @param device 
		/// @param log 
		/// @return 
		ModelHandle Load(const std::string& directory, const std::string& fileName,
			TextureStore* textureStore, DX12Heap* heap, ID3D12Device* device, ID3D12GraphicsCommandList* commandList, Log* log);

		/// @brief コマンドリストに登録する
		/// @param commandList 
		/// @param modelHandle 
		/// @param meshIndex 
		void Register(ID3D12GraphicsCommandList* commandList, ModelHandle modelHandle, int32_t meshIndex);

		/// @brief モデルデータを取得する
		/// @param handle 
		/// @return 
		ModelData GetModelData(ModelHandle handle) { return dataTable_[handle]->GetModelData(); }


	private:

		// データテーブル
		std::vector<std::unique_ptr<ModelResource>> dataTable_;
	};
}