#pragma once
#include "Handle/Handle.h"
#include "Math/Vector/Vector2/Vector2.h"
#include "Math/Vector/Vector3/Vector3.h"
#include "Math/Vector/Vector4/Vector4.h"
#include <vector>

namespace Engine
{
	namespace Primitive
	{
		/// @brief 種類
		enum class Type
		{
			StaticModel,
			AnimationModel,
			SkinningModel
		};

		/// @brief 静的モデル
		namespace StaticModel
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

				/// @brief 環境
				float environment;
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

		/// @brief アニメーションモデル
		namespace AnimationModel
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

				/// @brief 環境
				float environment;
			};

			/// @brief アニメーション
			struct Animation
			{
				/// @brief タイマー
				float timer;

				/// @brief アニメーションハンドル
				AnimationHandle hAnimation;
			};

			/// @brief パラメータ
			struct Param
			{
				/// @brief モデルトランスフォーム
				Transform modelTransform;

				/// @brief アニメーション
				Animation animation;


				/// @brief メッシュトランスフォーム
				std::vector<Transform> meshTransforms;

				/// @brief メッシュマテリアル
				std::vector<Material> meshMaterial;
			};
		}

		/// @brief スキニングモデル
		namespace SkinningModel
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

				/// @brief 環境
				float environment;
			};

			/// @brief アニメーション
			struct Animation
			{
				/// @brief タイマー
				float timer;

				/// @brief アニメーションハンドル
				AnimationHandle hAnimation;

				/// @brief スケルトンハンドル
				SkeletonHandle hSkeleton;
			};

			/// @brief パラメータ
			struct Param
			{
				/// @brief モデルトランスフォーム
				Transform modelTransform;

				/// @brief アニメーション
				Animation animation;


				/// @brief メッシュトランスフォーム
				std::vector<Transform> meshTransforms;

				/// @brief メッシュマテリアル
				std::vector<Material> meshMaterial;
			};
		}
	}
}