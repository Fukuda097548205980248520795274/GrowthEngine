#include "ModelFunc.h"

#include <cassert>
#include <list>


/// @brief モデルを読み込む
/// @param directory 
/// @param fileName 
/// @return 
Engine::ModelData Engine::LoadModel(const std::string& directory, const std::string& fileName)
{
	// AssImpでファイルを開く
	Assimp::Importer importer;
	std::string filePath = directory + "/" + fileName;

	// シーンのデータ
	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices |
		aiProcess_RemoveRedundantMaterials | aiProcess_CalcTangentSpace);

	// メッシュがないのは対応しない
	assert(scene->HasMeshes());

	// 頂点データ
	Vector4 position = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
	Vector3 normal = Vector3(0.0f, 0.0f, 0.0f);
	Vector2 texcoord = Vector2(0.0f, 0.0f);

	// モデルデータ
	ModelData modelData;

	// メッシュの数を確保する
	modelData.meshes.resize(scene->mNumMeshes);

	// メッシュ
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex)
	{
		// メッシュのデータ
		aiMesh* mesh = scene->mMeshes[meshIndex];

		// メッシュデータ
		MeshData meshData;

		// 頂点の数を確保する
		meshData.vertices.resize(static_cast<size_t>(mesh->mNumVertices));


		// 法線がないメッシュは非対応
		assert(mesh->HasNormals());

		// UV座標がないのは非対応
		assert(mesh->HasTextureCoords(0));


		// メッシュの名前を取得する
		std::string meshName = mesh->mName.C_Str();

		// 頂点データを登録する
		for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex)
		{
			// 頂点データを取得する
			aiVector3D& position = mesh->mVertices[vertexIndex];
			aiVector3D& normal = mesh->mNormals[vertexIndex];
			aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];

			// 値を取得する
			ModelVertexData vertex;
			vertex.position = Vector4(position.x, position.y, position.z, 1.0f);
			vertex.normal = Vector3(normal.x, normal.y, normal.z);
			vertex.texcoord = Vector2(texcoord.x, texcoord.y);

			// 右手から左手に変換
			vertex.position.x *= -1.0f;
			vertex.normal.x *= -1.0f;

			// データに登録
			meshData.vertices[vertexIndex] = vertex;
		}

		// インデックスデータを登録する
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex)
		{
			// フェイスのデータ
			aiFace& face = mesh->mFaces[faceIndex];

			// 三角形のみサポート
			assert(face.mNumIndices == 3);

			// インデックスを登録
			for (uint32_t i = 0; i < face.mNumIndices; ++i)
			{
				meshData.indices.push_back(face.mIndices[i]);
			}
		}


		// マテリアルインデックス
		uint32_t materialIndex = mesh->mMaterialIndex;

		// マテリアルデータ
		aiMaterial* material = scene->mMaterials[materialIndex];

		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0)
		{
			aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
			meshData.material.filePath = directory + "/" + textureFilePath.C_Str();
		}


		// 登録する
		modelData.meshes[meshIndex] = meshData;
		modelData.meshIndices[meshName] = meshIndex;
	}

	return modelData;
}

/// @brief ノードの読み込み
/// @param modelData 
/// @param directory 
/// @param fileName 
void Engine::LoadNode(ModelData& modelData, const std::string& directory, const std::string& fileName)
{
	// AssImpでファイルを開く
	Assimp::Importer importer;
	std::string filePath = directory + "/" + fileName;

	// シーンのデータ
	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

	ReadNodeRecursive(scene->mRootNode, -1, modelData);
}


/// @brief ノードの再帰読み込み
/// @param aiNode 
/// @param parentIndex 
/// @param modelData 
void Engine::ReadNodeRecursive(const aiNode* aiNode, int32_t parentIndex, ModelData& modelData)
{
	// 新しいノードを追加
	int32_t currentIndex = static_cast<int32_t>(modelData.nodes.size());
	modelData.nodes.emplace_back();
	ModelNode& node = modelData.nodes.back();



	// トランスフォームに格納する
	aiVector3D scale, translate;
	aiQuaternion rotate;
	aiNode->mTransformation.Decompose(scale, rotate, translate);

	node.transform.scale = Vector3(scale.x, scale.y, scale.z);

	// 右手 -> 左手
	node.transform.translate = Vector3(-translate.x, translate.y, translate.z);

	// X軸を反転　回転方向が逆なので軸を反転させる
	node.transform.rotate = Quaternion(rotate.x, -rotate.y, -rotate.z, rotate.w);



	// ノードの名前
	node.name = aiNode->mName.C_Str();

	// ローカル行列
	node.localMatrix = MakeAffineMatrix4x4(node.transform.scale, node.transform.rotate, node.transform.translate);


	// 親インデックス
	if (parentIndex >= 0) {
		node.parent = parentIndex;
		modelData.nodes[parentIndex].children.push_back(currentIndex);
	}

	// メッシュインデックス（複数ある場合は最初だけ対応）
	if (aiNode->mNumMeshes > 0) {
		node.meshIndex = aiNode->mMeshes[0];
	}

	// 子ノードを再帰的に処理
	for (uint32_t i = 0; i < aiNode->mNumChildren; ++i) {
		ReadNodeRecursive(aiNode->mChildren[i], currentIndex, modelData);
	}
}

/// @brief ノードの更新処理
/// @param modelData 
/// @param nodeIndex 
void Engine::UpdateWorldMatrix(ModelData& modelData, int32_t nodeIndex)
{
	ModelNode& node = modelData.nodes[nodeIndex];

	if (node.parent.has_value()) {
		node.worldMatrix = modelData.nodes[*node.parent].worldMatrix * node.localMatrix;
	}
	else 
	{
		node.worldMatrix = node.localMatrix;
	}

	// 自身の子ノードを更新
	for (int32_t child : node.children) 
	{
		UpdateWorldMatrix(modelData, child);
	}
}


/// @brief アニメーションの読み込み
/// @param directoryPath 
/// @param filename 
/// @return 
Engine::Animation Engine::LoadAnimationFile(const std::string& directoryPath, const std::string& filename)
{
	Animation animation;

	Assimp::Importer importer;
	std::string filePath = directoryPath + "/" + filename;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), 0);

	// アニメーションがないといけいない
	if (scene->mNumAnimations == 0)
	{
		return animation;
	}

	// 最初のアニメーションだけ採用
	aiAnimation* animationAssImp = scene->mAnimations[0];

	// 時間の単位を秒に変換
	animation.duration = float(animationAssImp->mDuration / animationAssImp->mTicksPerSecond);

	// 領域確保
	animation.nodes.resize(animationAssImp->mNumChannels);

	for (uint32_t channelIndex = 0; channelIndex < animationAssImp->mNumChannels; ++channelIndex)
	{
		aiNodeAnim* nodeAnimationAssImp = animationAssImp->mChannels[channelIndex];

		// インデックスを記録する
		std::string animationName = nodeAnimationAssImp->mNodeName.C_Str();
		animation.indices[animationName] = channelIndex;

		// ノードを取得する
		NodeAnimation& node = animation.nodes[channelIndex];


		// 位置
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssImp->mNumPositionKeys; ++keyIndex)
		{
			aiVectorKey& keyAssImp = nodeAnimationAssImp->mPositionKeys[keyIndex];

			KeyFrameVector3 keyframe;

			// 秒に変換
			keyframe.time = float(keyAssImp.mTime / animationAssImp->mTicksPerSecond);

			// 右手 -> 左手
			keyframe.value = { -keyAssImp.mValue.x, keyAssImp.mValue.y,keyAssImp.mValue.z };
			node.translate.push_back(keyframe);
		}

		// 回転
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssImp->mNumRotationKeys; ++keyIndex)
		{
			aiQuatKey& keyAssImp = nodeAnimationAssImp->mRotationKeys[keyIndex];

			KeyFrameQuaternion keyframe;

			// 秒に変換
			keyframe.time = float(keyAssImp.mTime / animationAssImp->mTicksPerSecond);

			// 右手 -> 左手
			keyframe.value = { keyAssImp.mValue.x, -keyAssImp.mValue.y, -keyAssImp.mValue.z , keyAssImp.mValue.w };
			node.rotate.push_back(keyframe);
		}

		// 拡縮
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssImp->mNumScalingKeys; ++keyIndex)
		{
			aiVectorKey& keyAssImp = nodeAnimationAssImp->mScalingKeys[keyIndex];

			KeyFrameVector3 keyframe;

			// 秒に変換
			keyframe.time = float(keyAssImp.mTime / animationAssImp->mTicksPerSecond);

			// そのまま
			keyframe.value = { keyAssImp.mValue.x, keyAssImp.mValue.y,keyAssImp.mValue.z };
			node.scale.push_back(keyframe);
		}
	}

	return animation;
}


/// @brief ボーンを読み込む
/// @param modelData 
/// @param directory 
/// @param fileName 
/// @param skeleton 
void Engine::LoadBone(ModelData& modelData, const std::string& directory, const std::string& fileName, const Skeleton& skeleton)
{
	// AssImpでファイルを開く
	Assimp::Importer importer;
	std::string filePath = directory + "/" + fileName;

	// シーンのデータ
	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices |
		aiProcess_RemoveRedundantMaterials | aiProcess_CalcTangentSpace);

	// メッシュがないのは対応しない
	assert(scene->HasMeshes());

	// メッシュ数が決まっていなかったら確保する
	if (modelData.meshes.empty())
	{
		modelData.meshes.resize(scene->mNumMeshes);
	}

	// メッシュ
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex)
	{
		// メッシュのデータ
		aiMesh* mesh = scene->mMeshes[meshIndex];

		// 法線がないメッシュは非対応
		assert(mesh->HasNormals());

		// UV座標がないのは非対応
		assert(mesh->HasTextureCoords(0));


		modelData.meshes[meshIndex].jointWeights.clear();
		modelData.meshes[meshIndex].jointWeights.resize(mesh->mNumBones);

		for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
		{
			aiBone* aiBone = mesh->mBones[boneIndex];

			// ボーン名を取得
			std::string boneName = aiBone->mName.C_Str();

			// Skeleton の jointMap から jointIndex を取得
			auto it = skeleton.jointMap.find(boneName);
			if (it == skeleton.jointMap.end()) {
				// Skeleton に存在しないボーンは無視（AssImp では稀にある）
				continue;
			}

			int32_t jointIndex = it->second;

			// JointWeightData を取得
			JointWeightData& jointWeight = modelData.meshes[meshIndex].jointWeights[boneIndex];
			jointWeight.jointIndex = jointIndex;

			// 頂点ウェイトを追加
			for (uint32_t w = 0; w < aiBone->mNumWeights; ++w)
			{
				const aiVertexWeight& aiWeight = aiBone->mWeights[w];

				VertexWeightData vw;
				vw.vertexIndex = aiWeight.mVertexId;
				vw.weight = aiWeight.mWeight;

				jointWeight.vertexWeights.push_back(vw);
			}
		}

	}
}