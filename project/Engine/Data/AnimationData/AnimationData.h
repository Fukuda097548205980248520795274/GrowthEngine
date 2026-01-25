#pragma once
#include <string>
#include <map>
#include <vector>
#include "Math/Vector/Vector3/Vector3.h"
#include "Math/Quaternion/Quaternion.h"

namespace Engine
{
	// キーフレーム
	template<typename Tvalue>
	struct KeyFrame
	{
		// 値
		Tvalue value;

		// 時刻
		float time;
	};


	// 3次元用のキーフレーム
	using KeyFrameVector3 = KeyFrame<Vector3>;

	// クォータニオン用のキーフレーム
	using KeyFrameQuaternion = KeyFrame<Quaternion>;


	// ノードアニメーション
	struct NodeAnimation
	{
		// 位置
		std::vector<KeyFrameVector3> translate;

		// 回転
		std::vector<KeyFrameQuaternion> rotate;

		// 拡縮
		std::vector<KeyFrameVector3> scale;
	};

	// アニメーション
	struct Animation
	{
		// アニメーション全体の尺
		float duration = 0.0f;

		// ノードアニメーションの集合体
		std::vector<NodeAnimation> nodes;

		// ノードアニメーションインデックス
		std::map<std::string, int32_t> indices;
	};
}