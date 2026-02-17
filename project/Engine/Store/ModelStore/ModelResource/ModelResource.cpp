#include "ModelResource.h"
#include "Func/ModelFunc/ModelFunc.h"
#include <cassert>
#include "Func/ResourceFunc/ResourceFunc.h"

/// @brief 初期化
/// @param device 
void Engine::ModelResource::Initialize(const std::string& directory, const std::string& fileName,ModelHandle handle,
	TextureStore* textureStore, DX12Heap* heap, ID3D12Device* device, ID3D12GraphicsCommandList* commandList, Log* log)
{
	// nullptrチェック
	assert(device);
	assert(textureStore);
	assert(heap);
	assert(device);
	assert(commandList);


	// モデルデータを取得する
	modelData_ = LoadModel(directory, fileName, textureStore, heap, device, commandList, log);

	// モデルノードを読み取る
	LoadNode(modelData_, directory, fileName);

	// ノード更新
	if (!modelData_.nodes.empty())UpdateWorldMatrix(modelData_, 0);

	// ファイルパス
	filePath_ = directory + "/" + fileName;

	// 領域を確保する
	vertexTable_.resize(modelData_.meshes.size());
	indexTable_.resize(modelData_.meshes.size());



	// ハンドルを取得する
	handle_ = handle;

	for (int32_t meshIndex = 0; meshIndex < modelData_.meshes.size(); meshIndex++)
	{
		/*-----------------
		    頂点リソース
		-----------------*/

		// 頂点リソースの生成と初期化
		std::unique_ptr<VertexBufferResource<ModelVertexData>> vertexResource = std::make_unique<VertexBufferResource<ModelVertexData>>();
		vertexResource->Initialize(device, static_cast<UINT>(modelData_.meshes[meshIndex].vertices.size()), log);

		// データをコピー
		std::memcpy(vertexResource->data_, modelData_.meshes[meshIndex].vertices.data(),
			sizeof(ModelVertexData) * static_cast<UINT>(modelData_.meshes[meshIndex].vertices.size()));

		// テーブルに追加
		vertexTable_[meshIndex] = std::move(vertexResource);


		/*-----------------------
		    インデックスリソース
		-----------------------*/

		// リソース生成
		std::unique_ptr<IndexBufferResource> indexResource = std::make_unique<IndexBufferResource>();
		indexResource->Initialize(device, static_cast<UINT>(modelData_.meshes[meshIndex].indices.size()), log);

		// データをコピー
		std::memcpy(indexResource->data_, modelData_.meshes[meshIndex].indices.data(),
			sizeof(uint32_t) * static_cast<UINT>(modelData_.meshes[meshIndex].indices.size()));
		
		// テーブルに追加
		indexTable_[meshIndex] = std::move(indexResource);
	}
}

/// @brief コマンドリストに登録する
/// @param commandList 
/// @param meshIndex 
void Engine::ModelResource::Register(ID3D12GraphicsCommandList* commandList, UINT meshIndex)
{
	// インデックス
	indexTable_[meshIndex]->Register(commandList);

	// 頂点
	vertexTable_[meshIndex]->Register(commandList);
}