#pragma once
#include <string>
#include <vector>
#include <optional>
#include <unordered_map>

#include "Math/Matrix/Matrix4x4/Matrix4x4.h"
#include "Math/Vector/Vector4/Vector4.h"
#include "Data/TransformData/TransformData.h"

namespace Engine
{
	// ジョイント
	struct Joint
	{
		// 名前
		std::string name;

		// Transform情報
		QuaternionTransform3D transform;

		// ローカル行列
		Matrix4x4 localMatrix;

		// SkeletonSpaceでの変換行列
		Matrix4x4 skeletonSpaceMatrix;

		/// @brief バインドポーズの逆行列
		Matrix4x4 offsetMatrix;


		// 自身のインデックス
		int32_t index;

		// 子jointのインデックスのリスト
		std::vector<int32_t> children;

		// 親joint
		std::optional<int32_t> parent;
	};


	/// @brief スケルトン
	struct Skeleton
	{
		// ルート
		int32_t root;

		// ジョイント
		std::vector<Joint> joints;

		// ジョイントマップ
		std::unordered_map<std::string, int32_t> jointMap;
	};
}