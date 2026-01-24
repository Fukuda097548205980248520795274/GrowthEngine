#include "ModelResource.h"
#include "Func/ModelFunc/ModelFunc.h"
#include <cassert>
#include "Func/ResourceFunc/ResourceFunc.h"

/// @brief コンストラクタ
/// @param directory 
/// @param fileName 
/// @param name 
Engine::ModelResource::ModelResource(const std::string& directory, const std::string& fileName)
{
	// モデルデータを取得する
	modelData_ = LoadModel(directory, fileName);

	// ファイルパス
	filePath_ = directory + "/" + fileName;

	// 領域を確保する
	vertexTable_.resize(modelData_.meshes.size());
	indexTable_.resize(modelData_.meshes.size());
}

/// @brief 初期化
/// @param device 
void Engine::ModelResource::Initialize(ID3D12Device* device, ModelHandle handle, Log* log)
{
	// nullptrチェック
	assert(device);

	// ハンドルを取得する
	handle_ = handle;

	for (int32_t meshIndex = 0; meshIndex < modelData_.meshes.size(); meshIndex++)
	{
		std::unique_ptr<VertexResource> vertexResource = std::make_unique<VertexResource>();
		std::unique_ptr<IndexResource> indexResource = std::make_unique<IndexResource>();


		/*-----------------
		    頂点リソース
		-----------------*/

		// リソース生成
		vertexResource->resource_ = CreateBufferResource(device, sizeof(ModelVertexData) * modelData_.meshes[meshIndex].vertices.size(), log);

		// ビューの設定
		vertexResource->view_.BufferLocation = vertexResource->resource_->GetGPUVirtualAddress();
		vertexResource->view_.SizeInBytes = sizeof(ModelVertexData) * static_cast<UINT>(modelData_.meshes[meshIndex].vertices.size());
		vertexResource->view_.StrideInBytes = sizeof(ModelVertexData);

		// データをコピーする
		vertexResource->resource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexResource->data_));
		std::memcpy(vertexResource->data_, modelData_.meshes[meshIndex].vertices.data(),
			sizeof(ModelVertexData) * static_cast<UINT>(modelData_.meshes[meshIndex].vertices.size()));


		/*-----------------------
		    インデックスリソース
		-----------------------*/

		// リソース生成
		indexResource->resource_ = CreateBufferResource(device, sizeof(uint32_t) * modelData_.meshes[meshIndex].indices.size(), log);

		// ビューの設定
		indexResource->view_.BufferLocation = indexResource->resource_->GetGPUVirtualAddress();
		indexResource->view_.Format = DXGI_FORMAT_R32_UINT;
		indexResource->view_.SizeInBytes = sizeof(uint32_t) * static_cast<UINT>(modelData_.meshes[meshIndex].indices.size());

		// データをコピーする
		indexResource->resource_->Map(0, nullptr, reinterpret_cast<void**>(&indexResource->data_));
		std::memcpy(indexResource->data_, modelData_.meshes[meshIndex].indices.data(),
			sizeof(uint32_t) * static_cast<UINT>(modelData_.meshes[meshIndex].indices.size()));
		

		// テーブルに追加
		vertexTable_[meshIndex] = std::move(vertexResource);
		indexTable_[meshIndex] = std::move(indexResource);
	}
}

/// @brief コマンドリストに登録する
/// @param commandList 
/// @param meshIndex 
void Engine::ModelResource::Register(ID3D12GraphicsCommandList* commandList, UINT meshIndex)
{
	// インデックス
	commandList->IASetIndexBuffer(&indexTable_[meshIndex]->view_);

	// 頂点
	commandList->IASetVertexBuffers(0, 1, &vertexTable_[meshIndex]->view_);
}