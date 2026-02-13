#pragma once
#include <string>
#include <vector>
#include <optional>
#include <unordered_map>
#include <array>
#include <span>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

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


	// 頂点ウェイトデータ
	struct VertexWeightData
	{
		float weight;
		uint32_t vertexIndex;
	};

	// ジョイントウェイトデータ
	struct JointWeightData
	{
		int32_t jointIndex;
		std::vector<VertexWeightData> vertexWeights;
	};



	/// @brief 頂点インフルエンス
	struct VertexInfluence
	{
		std::array<float, 4> weights;
		std::array<int32_t, 4> jointIndices;
	};

	/// @brief スキニングデータ
	struct SkinningData
	{
		uint32_t jointCount;
		std::vector<VertexInfluence> influence;
	};


	// GPUに送るウェル
	struct WellForGPU
	{
		// 位置用
		Matrix4x4 skeletonSpaceMatrix;

		// 法線用
		Matrix4x4 skeletonSpaceInverseTransposeMatrix;
	};


	// スキンクラスター
	struct SkinCluster
	{
		std::vector<Matrix4x4> inverseBindPoseMatrices;

		Microsoft::WRL::ComPtr<ID3D12Resource> influenceResource;
		D3D12_VERTEX_BUFFER_VIEW influenceBufferView;
		std::span<VertexInfluence> mappedInfluence;

		Microsoft::WRL::ComPtr<ID3D12Resource> paletteResource;
		std::span<WellForGPU> mappedPalette;
		std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> paletteSrvHandle;
	};


	/// @brief メッシュボーンデータ
	struct MeshBoneData
	{
		// ジョイントウェイト
		std::vector<JointWeightData> jointWeights;

		// スキニングデータ
		std::optional<SkinningData> skinning;
	};

	/// @brief モデルボーンデータ
	struct ModelBoneData
	{
		// メッシュボーンデータ
		std::vector<MeshBoneData> meshes;

		// メッシュボーンインデックス
		std::unordered_map<std::string, int32_t> meshIndices;
	};
}