#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <optional>
#include <unordered_map>
#include <wrl.h>

#include "Handle/Handle.h"
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
		// テクスチャハンドル
		TextureHandle handle;
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



	/// @brief メッシュデータ
	struct MeshData
	{
		// インデックスデータ
		std::vector<uint32_t> indices;

		// 頂点データ
		std::vector<ModelVertexData> vertices;

		// マテリアルデータ
		MaterialData material;
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