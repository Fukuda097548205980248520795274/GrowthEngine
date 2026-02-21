#pragma once
#include <vector>
#include "Handle/Handle.h"
#include "Math/Vector/Vector2/Vector2.h"
#include "Math/Vector/Vector3/Vector3.h"
#include "Math/Vector/Vector4/Vector4.h"

namespace Engine
{
	/// @brief プレハブ
	namespace Prefab
	{
		enum class Type
		{
			StaticModel,
			AnimationModel,
			SkinningModel,
		};

		/// @brief 静的モデル
		namespace StaticModel
		{
			/// @brief 基底
			namespace Base
			{
				/// @brief トランスフォーム
				struct Transform
				{
					// 拡縮
					Vector3 scale;

					// 回転
					Vector3 rotate;

					// 移動
					Vector3 translate;
				};

				/// @brief UVトランスフォーム
				struct UVTransform
				{
					// 拡縮
					Vector2 scale;

					// 回転
					float radius;

					// 移動
					Vector2 translate;
				};

				/// @brief マテリアル
				struct Material
				{
					/// @brief テクスチャハンドル
					TextureHandle hTexture;

					/// @brief 色
					Vector4 color;

					/// @brief UV
					UVTransform uv;
				};

				/// @brief パラメータ
				struct Param
				{
					/// @brief モデルトランスフォーム
					Transform modelTransform;


					/// @brief メッシュトランスフォーム
					std::vector<Transform> meshTransforms;

					/// @brief メッシュマテリアル
					std::vector<Material> meshMaterial;
				};
			}

			/// @brief インスタンス
			namespace Instance
			{
				/// @brief トランスフォーム
				struct Transform
				{
					// 拡縮
					Vector3 scale;

					// 回転
					Vector3 rotate;

					// 移動
					Vector3 translate;
				};

				/// @brief UVトランスフォーム
				struct UVTransform
				{
					// 拡縮
					Vector2 scale;

					// 回転
					float radius;

					// 移動
					Vector2 translate;
				};

				/// @brief マテリアル
				struct Material
				{
					/// @brief 色
					Vector4 color;

					/// @brief UV
					UVTransform uv;
				};

				/// @brief パラメータ
				struct Param
				{
					/// @brief モデルトランスフォーム
					Transform modelTransform;


					/// @brief メッシュトランスフォーム
					std::vector<Transform> meshTransforms;

					/// @brief メッシュマテリアル
					std::vector<Material> meshMaterial;
				};
			}
		}

		/// @brief スプライト
		namespace Sprite
		{
			/// @brief 基底
			namespace Base
			{
				/// @brief トランスフォーム
				struct Transform
				{
					/// @brief 拡縮
					Vector2 scale;

					/// @brief 回転
					float rotate;

					/// @brief 移動
					Vector2 translate;
				};

				/// @brief マテリアル
				struct Material
				{
					/// @brief 色
					Vector4 color;

					/// @brief UV
					Transform uv;
				};

				/// @brief テクスチャ
				struct Texture
				{
					/// @brief テクスチャハンドル
					TextureHandle hTexture;

					/// @brief サイズ
					Vector2 size;

					/// @brief アンカー
					Vector2 anchor;
				};

				/// @brief パラメータ
				struct Param
				{
					/// @brief トランスフォーム
					Transform transform;

					/// @brief マテリアル
					Material material;

					/// @brief テクスチャ
					Texture texture;
				};
			}

			/// @brief インスタンス
			namespace Instance
			{
				/// @brief トランスフォーム
				struct Transform
				{
					/// @brief 拡縮
					Vector2 scale;

					/// @brief 回転
					float rotate;

					/// @brief 移動
					Vector2 translate;
				};

				/// @brief マテリアル
				struct Material
				{
					/// @brief 色
					Vector4 color;

					/// @brief UV
					Transform uv;
				};

				/// @brief テクスチャ
				struct Texture
				{
					/// @brief サイズ
					Vector2 size;

					/// @brief アンカー
					Vector2 anchor;
				};

				/// @brief パラメータ
				struct Param
				{
					/// @brief トランスフォーム
					Transform transform;

					/// @brief マテリアル
					Material material;

					/// @brief テクスチャ
					Texture texture;
				};
			}
		}
	}
}