#pragma once
#include "Resource/VertexBufferResource/VertexBufferResource.h"
#include "Resource/IndexBufferResource/IndexBufferResource.h"
#include "DataForGPU/VertexDataForGPU/VertexDataForGPU.h"

namespace Engine
{
	class PlaneVertexResource
	{
	public:

		/// @brief 初期化
		/// @param device 
		/// @param log 
		void Initialize(ID3D12Device* device, Log* log);

		/// @brief コマンドリストに登録する
		/// @param commandList 
		void Register(ID3D12GraphicsCommandList* commandList);


	private:

		/// @brief 頂点リソース
		std::unique_ptr<VertexBufferResource<VertexDataForGPU>> vertexResource_ = nullptr;

		/// @brief インデックスリソース
		std::unique_ptr<IndexBufferResource> indexResource_ = nullptr;
	};
}