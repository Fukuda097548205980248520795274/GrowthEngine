#pragma once
#include "Handle/Handle.h"
#include "Data/SkeletonData/SkeletonData.h"
#include "Data/ModelData/ModelData.h"

namespace Engine
{
	class SkeletonResource
	{
	public:

		/// @brief コンストラクタ
		/// @param directory 
		/// @param fileName 
		/// @param modelNode 
		/// @param handle 
		SkeletonResource(const std::string& directory, const std::string& fileName,const std::vector<ModelNode>& nodes, SkeletonHandle handle);

		/// @brief ファイルパスを取得する
		/// @return 
		std::string GetFilePath()const { return filePath_; }

		/// @brief ハンドルを取得する
		/// @return 
		SkeletonHandle GetHandle()const { return handle_; }

		/// @brief スケルトンを取得する
		/// @return 
		Skeleton GetSkeleton()const { return skeleton_; }

		/// @brief ボーンデータを取得する
		/// @return 
		ModelBoneData GetBoneData()const { return boneData_; }


	private:

		// スケルトン
		Skeleton skeleton_{};

		// ボーンデータ
		ModelBoneData boneData_{};

		// ハンドル
		SkeletonHandle handle_ = 0;

		// ファイルパス
		std::string filePath_{};
	};
}