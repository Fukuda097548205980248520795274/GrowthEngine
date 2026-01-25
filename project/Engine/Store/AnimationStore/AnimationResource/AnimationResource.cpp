#include "AnimationResource.h"
#include "Func/ModelFunc/ModelFunc.h"
#include <cassert>

/// @brief コンストラクタ
/// @param directory 
/// @param fileName 
Engine::AnimationResource::AnimationResource(const std::string& directory, const std::string& fileName, AnimationHandle handle)
	: handle_(handle)
{
	// アニメーションを取得する
	animation_ = LoadAnimationFile(directory, fileName);

	// ファイルパス
	filePath_ = directory + '/' + fileName;
}
