#include "SkeletonStore.h"
#include "Store/ModelStore/ModelStore.h"
#include "GrowthEngine.h"

Engine::SkeletonStore::SkeletonStore()
{
	// インスタンスを取得する
	engine_ = GrowthEngine::GetInstance();
}

/// @brief 読み込む
/// @param directory 
/// @param fileName 
/// @return 
SkeletonHandle Engine::SkeletonStore::Load(const std::string& directory, const std::string& fileName, const std::vector<ModelNode>& nodes)
{
	// 同じデータがないかどうか
	for (auto& data : dataTable_)
	{
		if (directory + "/" + fileName == data->GetFilePath())
			return data->GetHandle();
	}

	// ハンドル
	SkeletonHandle handle = static_cast<SkeletonHandle>(dataTable_.size());

	// データ生成と記録
	std::unique_ptr<SkeletonResource> data = std::make_unique<SkeletonResource>(directory, fileName, nodes, handle);
	dataTable_.push_back(std::move(data));

	return handle;
}

/// @brief スケルトンのデバッグ描画
/// @param skeleton 
/// @param position 
/// @param color 
void Engine::SkeletonStore::DrawDebugSkeleton(const Skeleton& skeleton, const Vector4& color , const WorldTransform3D* worldTransform)
{
#ifdef DEVELOPMENT

	// ジョイントをループして線を描画する
	for (const Joint& joint : skeleton.joints)
	{
		// 親がいないときは線を描画しない
		if (!joint.parent)
			continue;
		
		// 親のインデックスを取得する
		const int32_t kParentIndex = *joint.parent;

		// 親のインデックスが不正なときは線を描画しない
		if (kParentIndex < 0 || kParentIndex >= static_cast<int32_t>(skeleton.joints.size()))
			continue;

		// 親のジョイントを取得する
		const Joint& parentJoint = skeleton.joints[kParentIndex];

		Matrix4x4 parentSkeletonSpaceMatrix = joint.skeletonSpaceMatrix;
		Matrix4x4 jointSkeletonSpaceMatrix = parentJoint.skeletonSpaceMatrix;
		if (worldTransform)
		{
			parentSkeletonSpaceMatrix = parentSkeletonSpaceMatrix * worldTransform->GetWorldMatrix();
			jointSkeletonSpaceMatrix = jointSkeletonSpaceMatrix * worldTransform->GetWorldMatrix();
		}

		// 線の始点と終点を計算する
		Vector3 start = 
			Vector3(parentSkeletonSpaceMatrix.m[3][0], parentSkeletonSpaceMatrix.m[3][1], parentSkeletonSpaceMatrix.m[3][2]);

		// 線の終点を計算する
		Vector3 end = 
			Vector3(jointSkeletonSpaceMatrix.m[3][0], jointSkeletonSpaceMatrix.m[3][1], jointSkeletonSpaceMatrix.m[3][2]);

		engine_->DrawDebugLine3D(start, end, color);
	}

#endif
}