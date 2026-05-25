#include "ModelStore.h"
#include <cassert>

/// @brief 初期化
/// @param device 
/// @param log 
void Engine::ModelStore::Initilaize(ID3D12Device* device, Log* log)
{
	// nullptrチェック
	assert(device);
	
	// 立方体頂点リソースの生成と初期化
	cubeVertexResource_ = std::make_unique<CubeVertexResource>();
	cubeVertexResource_->Initialize(device, log);

	// 平面頂点リソースの生成と初期化
	planeVertexResource_ = std::make_unique<PlaneVertexResource>();
	planeVertexResource_->Initialize(device, log);
}

/// @brief 読み込み
/// @param directory 
/// @param fileName 
/// @return 
ModelHandle Engine::ModelStore::Load(const std::string& directory, const std::string& fileName,
	TextureStore* textureStore, DX12Heap* heap, ID3D12Device* device, ID3D12GraphicsCommandList* commandList, Log* log)
{
	// nullptrチェック
	assert(device);

	// 同じファイルは読まず、ハンドルを返す
	for (auto& data : dataTable_)
	{
		if (directory + "/" + fileName == data->GetFilePath())
			return data->GetHandle();
	}

	// ハンドル
	ModelHandle handle = static_cast<ModelHandle>(dataTable_.size());

	// リソース生成と初期化
	std::unique_ptr<ModelResource> modelResource = std::make_unique<ModelResource>();
	modelResource->Initialize(directory, fileName, handle, textureStore, heap, device, commandList, log);

	// テーブルに追加
	dataTable_.push_back(std::move(modelResource));

	return handle;
}

/// @brief コマンドリストに登録する
/// @param commandList 
/// @param modelHandle 
/// @param meshIndex 
void Engine::ModelStore::Register(ID3D12GraphicsCommandList* commandList, ModelHandle modelHandle, int32_t meshIndex)
{
	dataTable_[modelHandle]->Register(commandList, meshIndex);
}

/// @brief 立方体の頂点リソースをコマンドリストに登録する
/// @param commandList 
void Engine::ModelStore::CubeVertexRegiseter(ID3D12GraphicsCommandList* commandList)
{
	cubeVertexResource_->Register(commandList);
}

/// @brief 平面の頂点リソースをコマンドリストに登録する
/// @param commandList 
void Engine::ModelStore::PlaneVertexRegiseter(ID3D12GraphicsCommandList* commandList)
{
	planeVertexResource_->Register(commandList);
}