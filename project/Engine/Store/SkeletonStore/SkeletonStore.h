#pragma once
#include <memory>
#include "SkeletonResource/SkeletonResource.h"

namespace Engine
{
	class SkeletonStore
	{
	public:

		/// @brief 読み込む
		/// @param directory 
		/// @param fileName 
		/// @param nodes 
		/// @return 
		SkeletonHandle Load(const std::string& directory, const std::string& fileName, const std::vector<ModelNode>& nodes);

		/// @brief スケルトンを取得する
		/// @param handle 
		/// @return 
		Skeleton GetSkeleton(SkeletonHandle handle) { return dataTable_[handle]->GetSkeleton(); }


	private:

		// データテーブル
		std::vector<std::unique_ptr<SkeletonResource>> dataTable_;
	};
}