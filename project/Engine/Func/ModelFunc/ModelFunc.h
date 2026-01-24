#pragma once
#include "Data/ModelData/ModelData.h"
#include "Data/BoneData/BoneData.h"
#include <string>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

namespace Engine
{

	/// @brief ジオメトリを読み込む
	/// @param modelData 
	/// @param directory 
	/// @param fileName 
	void LoadGeometry(ModelData& modelData, const std::string& directory, const std::string& fileName);

	/// @brief マテリアルを読み込む
	/// @param modelData 
	/// @param directory 
	/// @param fileName 
	void LoadMaterial(ModelData& modelData, const std::string& directory, const std::string& fileName);

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