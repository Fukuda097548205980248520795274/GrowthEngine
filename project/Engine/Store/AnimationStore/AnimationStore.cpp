#include "AnimationStore.h"

/// @brief 読み込む
/// @param directory 
/// @param fileName 
AnimationHandle Engine::AnimationStore::Load(const std::string& directory, const std::string& fileName)
{
	// 同じファイルは読まず、ハンドルを返す
	for (auto& data : dataTable_)
	{
		if (directory + "/" + fileName == data->GetFilePath())
			return data->GetHandle();
	}

	// ハンドルを取得する
	AnimationHandle handle = static_cast<AnimationHandle>(dataTable_.size());

	// アニメーションデータの生成と初期化
	std::unique_ptr<AnimationResource> data = std::make_unique<AnimationResource>(directory, fileName, handle);

	// テーブルに追加
	dataTable_.push_back(std::move(data));
	
	return handle;
}