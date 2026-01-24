#include "ModelStore.h"
#include <cassert>

/// @brief 読み込み
/// @param directory 
/// @param fileName 
/// @return 
ModelHandle Engine::ModelStore::Load(const std::string& directory, const std::string& fileName, ID3D12Device* device, Log* log)
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
	std::unique_ptr<ModelResource> modelResource = std::make_unique<ModelResource>(directory, fileName);
	modelResource->Initialize(device, handle, log);

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