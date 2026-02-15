#include "SkeletonResource.h"
#include "Func/ModelFunc/ModelFunc.h"

/// @brief コンストラクタ
/// @param directory 
/// @param fileName 
/// @param handle 
Engine::SkeletonResource::SkeletonResource(const std::string& directory, const std::string& fileName, const std::vector<ModelNode>& nodes, SkeletonHandle handle)
	: handle_(handle)
{
	// スケルトンを作成する
	skeleton_ = CreateSkeleton(nodes);

	// ボーンデータを取得する
	boneData_ = LoadBone(directory, fileName, skeleton_);

	// ファイルパス
	filePath_ = directory + "/" + fileName;
}