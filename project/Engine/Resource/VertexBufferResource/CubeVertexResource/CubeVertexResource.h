#pragma once
#include "Resource/VertexBufferResource/VertexBufferResource.h"
#include "Resource/IndexBufferResource/IndexBufferResource.h"
#include "DataForGPU/VertexDataForGPU/VertexDataForGPU.h"

namespace Engine
{
	class CubeVertexResource
	{
	public:

		/// @brief 初期化
		/// @param device 
		/// @param log 
		void Initialize(ID3D12Device* device, Log* log);

		/// @brief コマンドリストに登録する
		/// @param commandList 
		void Register(ID3D12GraphicsCommandList* commandList);

		/// @brief インデックス数を取得する
		/// @return 
		int32_t GetNumIndex() const { return numIndex_; }


	private:

		/// @brief 頂点リソース
		std::unique_ptr<VertexBufferResource<VertexDataForGPU>> vertexResource_ = nullptr;

		/// @brief インデックスリソース
		std::unique_ptr<IndexBufferResource> indexResource_ = nullptr;

		/// @brief インデックス数
		int32_t numIndex_ = 36;
	};
}