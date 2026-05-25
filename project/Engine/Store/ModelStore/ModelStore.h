#pragma once
#include "ModelResource/ModelResource.h"
#include "Resource/VertexBufferResource/CubeVertexResource/CubeVertexResource.h"
#include "Resource/VertexBufferResource/PlaneVertexResource/PlaneVertexResource.h"

namespace Engine
{
	class TextureStore;
	class Log;
	class DX12Heap;

	class ModelStore
	{
	public:

		/// @brief 初期化
		/// @param device 
		/// @param log 
		void Initilaize(ID3D12Device* device, Log* log);

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

		/// @brief 立方体の頂点リソースをコマンドリストに登録する
		/// @param commandList 
		void CubeVertexRegiseter(ID3D12GraphicsCommandList* commandList);

		/// @brief 平面の頂点リソースをコマンドリストに登録する
		/// @param commandList 
		void PlaneVertexRegiseter(ID3D12GraphicsCommandList* commandList);

		/// @brief モデルデータを取得する
		/// @param handle 
		/// @return 
		ModelData GetModelData(ModelHandle handle) { return dataTable_[handle]->GetModelData(); }


	private:

		// データテーブル
		std::vector<std::unique_ptr<ModelResource>> dataTable_;


	private:

		/// @brief 立方体頂点リソース
		std::unique_ptr<CubeVertexResource> cubeVertexResource_ = nullptr;

		/// @brief 平面頂点リソース
		std::unique_ptr<PlaneVertexResource> planeVertexResource_ = nullptr;
	};
}