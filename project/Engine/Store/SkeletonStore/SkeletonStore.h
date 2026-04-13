#pragma once
#include <memory>
#include "SkeletonResource/SkeletonResource.h"

class GrowthEngine;

namespace Engine
{
	class SkeletonStore
	{
	public:

		/// @brief コンストラクタ
		SkeletonStore();

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

		/// @brief ボーンデータを取得する
		/// @param handle 
		/// @return 
		ModelBoneData GetBoneData(SkeletonHandle handle) { return dataTable_[handle]->GetBoneData(); }

		
		/// @brief スケルトンのデバッグ描画
		/// @param skeleton 
		/// @param position 
		/// @param color 
		void DrawDebugSkeleton(const Skeleton& skeleton, const Vector3& position, const Vector4& color);


	private:

		// データテーブル
		std::vector<std::unique_ptr<SkeletonResource>> dataTable_;


	private:

		// エンジン
		const GrowthEngine* engine_ = nullptr;
	};
}