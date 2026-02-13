#include "SkeletonResource.h"
#include "Func/ModelFunc/ModelFunc.h"

/// @brief コンストラクタ
/// @param directory 
/// @param fileName 
/// @param handle 
Engine::SkeletonResource::SkeletonResource(const std::string& directory, const std::string& fileName, const ModelNode& modelNode, SkeletonHandle handle)
	: handle_(handle)
{
	// ファイルパス
	filePath_ = directory + "/" + fileName;

	
}