#include "SkeletonStore.h"
#include "Store/ModelStore/ModelStore.h"

/// @brief 読み込む
/// @param directory 
/// @param fileName 
/// @return 
SkeletonHandle Engine::SkeletonStore::Load(const std::string& directory, const std::string& fileName, const std::vector<ModelNode>& nodes)
{
	// 同じデータがないかどうか
	for (auto& data : dataTable_)
	{
		if (directory + "/" + fileName == data->GetFilePath())
			return data->GetHandle();
	}

	// ハンドル
	SkeletonHandle handle = static_cast<SkeletonHandle>(dataTable_.size());

	// データ生成と記録
	std::unique_ptr<SkeletonResource> data = std::make_unique<SkeletonResource>(directory, fileName, nodes, handle);
	dataTable_.push_back(std::move(data));

	return handle;
}