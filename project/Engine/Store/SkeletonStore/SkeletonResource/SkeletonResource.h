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
		SkeletonResource(const std::string& directory, const std::string& fileName,const ModelNode& modelNode, SkeletonHandle handle);


	private:

		// スケルトン
		Skeleton skeleton_{};

		// ハンドル
		SkeletonHandle handle_ = 0;

		// ファイルパス
		std::string filePath_{};
	};
}