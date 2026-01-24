#pragma once
#include "Data/ModelData/ModelData.h"
#include "Data/SkeletonData/SkeletonData.h"
#include <string>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

namespace Engine
{

	/// @brief モデルを読み込む
	/// @param directory 
	/// @param fileName 
	/// @return 
	ModelData LoadModel(const std::string& directory, const std::string& fileName);

	/// @brief ノードを読み込む
	/// @param modelData 
	/// @param directory 
	/// @param fileName 
	void LoadNode(ModelData& modelData, const std::string& directory, const std::string& fileName);

	/// @brief ノードの再帰読み込み
	/// @param aiNode 
	/// @param parentIndex 
	/// @param modelData 
	void ReadNodeRecursive(const aiNode* aiNode, int32_t parentIndex, ModelData& modelData);

	/// @brief ボーンを読み込む
	/// @param modelData 
	/// @param directory 
	/// @param fileName 
	/// @param skeleton 
	void LoadBone(ModelData& modelData, const std::string& directory, const std::string& fileName, const Skeleton& skeleton);
}