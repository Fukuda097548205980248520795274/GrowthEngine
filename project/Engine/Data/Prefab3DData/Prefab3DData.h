#pragma once
#include <vector>
#include "Handle/Handle.h"
#include "Math/Vector/Vector2/Vector2.h"
#include "Math/Vector/Vector3/Vector3.h"
#include "Math/Vector/Vector4/Vector4.h"
#include "PSO/EnumBlendMode/EnumBlendMode.h"

class WorldTransform3D;

namespace Engine
{
	/// @brief プレハブ
	namespace Prefab3D
	{
		enum class Type
		{
			StaticModel,
			Cube
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

					/// @brief 環境
					float environment;

					// 光沢度
					float shininess;

					// ライティング有効化
					bool enableLighting;

					// ディフューズ
					bool enableDiffuse;

					// ハーフランバート有効化
					bool enableHalfLambert;

					// スペキュラー有効化
					bool enableSpecular;

					// ブリンフォン有効化
					bool enableBlinnPhong;

					// シャドウ有効化
					bool enableShadow;
				};

				/// @brief ブラー
				struct Blur
				{
					// 残像用マスク
					float afterImageMask;

					// モーションブラー用マスク
					float motionBlurMask;
				};

				/// @brief パラメータ
				struct Param
				{
					/// @brief ブレンドモード
					BlendMode blendMode;

					/// @brief モデルトランスフォーム
					Transform modelTransform;


					/// @brief メッシュトランスフォーム
					std::vector<Transform> meshTransforms;

					/// @brief メッシュマテリアル
					std::vector<Material> meshMaterial;

					/// @brief ブラー
					std::vector<Blur> meshBlur;
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

					/// @brief 環境
					float environment;

					// 光沢度
					float shininess;

					// ライティング有効化
					bool enableLighting;

					// ディフューズ
					bool enableDiffuse;

					// ハーフランバート有効化
					bool enableHalfLambert;

					// スペキュラー有効化
					bool enableSpecular;

					// ブリンフォン有効化
					bool enableBlinnPhong;

					// シャドウ有効化
					bool enableShadow;
				};

				/// @brief ブラー
				struct Blur
				{
					// 残像用マスク
					float afterImageMask;

					// モーションブラー用マスク
					float motionBlurMask;
				};

				/// @brief パラメータ
				struct Param
				{
					/// @brief モデルトランスフォーム
					Transform modelTransform;

					// 親トランスフォーム
					WorldTransform3D* parent = nullptr;


					/// @brief メッシュトランスフォーム
					std::vector<Transform> meshTransforms;

					/// @brief メッシュマテリアル
					std::vector<Material> meshMaterial;

					/// @brief ブラー
					std::vector<Blur> meshBlur;
				};
			}
		}

		/// @brief 立方体
		namespace Cube
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

					/// @brief 環境
					float environment;

					// 光沢度
					float shininess;

					// ライティング有効化
					bool enableLighting;

					// ディフューズ
					bool enableDiffuse;

					// ハーフランバート有効化
					bool enableHalfLambert;

					// スペキュラー有効化
					bool enableSpecular;

					// ブリンフォン有効化
					bool enableBlinnPhong;

					// シャドウ有効化
					bool enableShadow;
				};

				/// @brief ブラー
				struct Blur
				{
					// 残像用マスク
					float afterImageMask;

					// モーションブラー用マスク
					float motionBlurMask;
				};

				/// @brief パラメータ
				struct Param
				{
					/// @brief ブレンドモード
					BlendMode blendMode;

					/// @brief トランスフォーム
					Transform transform;

					/// @brief マテリアル
					Material material;

					/// @brief ブラー
					Blur blur;
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

					/// @brief 環境
					float environment;

					// 光沢度
					float shininess;

					// ライティング有効化
					bool enableLighting;

					// ディフューズ
					bool enableDiffuse;

					// ハーフランバート有効化
					bool enableHalfLambert;

					// スペキュラー有効化
					bool enableSpecular;

					// ブリンフォン有効化
					bool enableBlinnPhong;

					// シャドウ有効化
					bool enableShadow;
				};

				/// @brief ブラー
				struct Blur
				{
					// 残像用マスク
					float afterImageMask;

					// モーションブラー用マスク
					float motionBlurMask;
				};

				/// @brief パラメータ
				struct Param
				{
					/// @brief トランスフォーム
					Transform transform;

					// 親トランスフォーム
					WorldTransform3D* parent = nullptr;

					/// @brief マテリアル
					Material material;

					/// @brief ブラー
					Blur blur;
				};
			}
		}
	}
}