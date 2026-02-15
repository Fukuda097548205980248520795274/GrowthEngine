#pragma once
#include "Data/ModelData/ModelData.h"
#include "Data/SkeletonData/SkeletonData.h"
#include "Data/AnimationData/AnimationData.h"
#include <string>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

namespace Engine
{
	class TextureStore;
	class DX12Heap;
	class Log;

	/// @brief モデルを読み込む
	/// @param directory 
	/// @param fileName 
	/// @param textureStore 
	/// @param heap 
	/// @param device 
	/// @param commandList 
	/// @param log 
	/// @return 
	ModelData LoadModel(const std::string& directory, const std::string& fileName,
		TextureStore* textureStore, DX12Heap* heap, ID3D12Device* device, ID3D12GraphicsCommandList* commandList, Log* log);

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

	/// @brief ノードの更新処理
	/// @param modelData 
	/// @param nodeIndex 
	void UpdateWorldMatrix(ModelData& modelData, int32_t nodeIndex);

	/// @brief アニメーションの読み込み
	/// @param directoryPath 
	/// @param filename 
	/// @return 
	Animation LoadAnimationFile(const std::string& directoryPath, const std::string& filename);

	/// @brief ボーンを読み込む
	/// @param directory 
	/// @param fileName 
	/// @param skeleton 
	/// @return 
	ModelBoneData LoadBone(const std::string& directory, const std::string& fileName, const Skeleton& skeleton);

	/// @brief スケルトンを作成する
	/// @param rootNode 
	/// @return 
	Skeleton CreateSkeleton(const std::vector<ModelNode>& nodes);

	/// @brief ジョイントを作成する
	/// @param node 
	/// @param parent 
	/// @param joints 
	/// @return 
	int32_t CreateJoint(const std::vector<ModelNode>& nodes, const ModelNode& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);

	/// @brief スケルトンを更新する
	/// @param skeleton 
	void UpdateSkeleton(Skeleton& skeleton);

	/// @brief 任意の時刻の値を取得する
	/// @param keyframe 
	/// @param time 
	/// @return 
	Vector3 CalculateValue(const std::vector<KeyFrameVector3>& keyframe, float time);

	/// @brief 任意の時刻の値を取得する
	/// @param keyframe 
	/// @param time 
	/// @return 
	Quaternion CalculateValue(const std::vector<KeyFrameQuaternion>& keyframe, float time);

	/// @brief ボーンアニメーションを再生する
	/// @param skeleton 
	/// @param animation 
	/// @param animationTime 
	void ApplyBoneAnimation(Skeleton& skeleton, Animation& animation, float animationTime);
}