#pragma once
#include "ModelResource/ModelResource.h"

namespace Engine
{
	class Log;

	class ModelStore
	{
	public:

		/// @brief 読み込み
		/// @param directory 
		/// @param fileName 
		/// @param device 
		/// @param log 
		/// @return 
		ModelHandle Load(const std::string& directory, const std::string& fileName, ID3D12Device* device, Log* log);

		/// @brief コマンドリストに登録する
		/// @param commandList 
		/// @param modelHandle 
		/// @param meshIndex 
		void Register(ID3D12GraphicsCommandList* commandList, ModelHandle modelHandle, int32_t meshIndex);


	private:

		// データテーブル
		std::vector<std::unique_ptr<ModelResource>> dataTable_;
	};
}