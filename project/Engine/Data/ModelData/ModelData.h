#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <optional>
#include <array>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <unordered_map>
#include <wrl.h>
#include <span>

#include "Math/Matrix/Matrix4x4/Matrix4x4.h"
#include "Math/Vector/Vector4/Vector4.h"

#include "Data/TransformData/TransformData.h"

namespace Engine
{
	/// @brief モデル用頂点データ
	struct ModelVertexData
	{
		// ローカル座標
		Vector4 position;

		// UV座標
		Vector2 texcoord;

		// 法線
		Vector3 normal;
	};

	/// @brief スプライト用頂点データ
	struct SpriteVertexData
	{
		// ローカル座標
		Vector4 position;

		// UV座標
		Vector2 texcoord;
	};



	/// @brief マテリアルデータ
	struct MaterialData
	{
		// ファイルパパス
		std::string filePath;
	};


	/// @brief モデルノード
	struct ModelNode
	{
		// 名前
		std::string name;

		// ローカル行列
		Matrix4x4 localMatrix;

		// ワールド行列
		Matrix4x4 worldMatrix;

		// クォータニオン用のトランスフォーム
		QuaternionTransform3D transform;

		// 子ノードインデックス
		std::vector<int32_t> children;

		// 親ノードインデックス
		std::optional<int32_t> parent;

		// メッシュインデックス
		std::optional<int32_t> meshIndex;
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



	/// @brief メッシュデータ
	struct MeshData
	{
		// インデックスデータ
		std::vector<uint32_t> indices;

		// 頂点データ
		std::vector<ModelVertexData> vertices;

		// マテリアルデータ
		MaterialData material;


		// ジョイントウェイト
		std::vector<JointWeightData> jointWeights;

		// スキニングデータ
		std::optional<SkinningData> skinning;
	};



	
	/// @brief モデルデータ
	struct ModelData
	{
		// メッシュデータ
		std::vector<MeshData> meshes;

		// メッシュインデックス
		std::unordered_map<std::string, int32_t> meshIndices;

		// ノード
		std::vector<ModelNode> nodes;
	};
}