#pragma once
#include "Handle/Handle.h"
#include "Math/Vector/Vector2/Vector2.h"
#include "Math/Vector/Vector3/Vector3.h"
#include "Math/Vector/Vector4/Vector4.h"
#include "PSO/EnumBlendMode/EnumBlendMode.h"
#include <vector>

namespace Engine
{
	namespace Render3D
	{
		/// @brief 種類
		enum class Type
		{
			StaticModel,
			AnimationModel,
			SkinningModel,
			UVSphere,
			Ring,
			Cylinder,
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

				/// @brief シャドウマップ描画
				bool drawShadowMap;

				/// @brief シャドウ有効化
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

				/// @brief シャドウマップ描画
				bool drawShadowMap;

				/// @brief シャドウ有効化
				bool enableShadow;
			};

			/// @brief アニメーション
			struct Animation
			{
				/// @brief タイマー
				float timer;

				/// @brief アニメーションハンドル
				AnimationHandle hAnimation;
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

				/// @brief アニメーション
				Animation animation;


				/// @brief メッシュトランスフォーム
				std::vector<Transform> meshTransforms;

				/// @brief メッシュマテリアル
				std::vector<Material> meshMaterial;

				/// @brief ブラー
				std::vector<Blur> meshBlur;
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

				/// @brief シャドウマップ描画
				bool drawShadowMap;

				/// @brief シャドウ有効化
				bool enableShadow;
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

				/// @brief アニメーション
				Animation animation;


				/// @brief メッシュトランスフォーム
				std::vector<Transform> meshTransforms;

				/// @brief メッシュマテリアル
				std::vector<Material> meshMaterial;

				/// @brief ブラー
				std::vector<Blur> meshBlur;
			};
		}

		/// @brief UV球
		namespace UVSphere
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

			// @brief UVトランスフォーム
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

				/// @brief シャドウマップ描画
				bool drawShadowMap;

				/// @brief シャドウ有効化
				bool enableShadow;
			};

			// 分割
			struct Division
			{
				// スライス数
				int32_t slices;

				// リング数
				int32_t rings;
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

				/// @brief 分割
				Division division;

				/// @brief ブラー
				Blur blur;
			};
		}

		/// @brief リング
		namespace Ring
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

			// @brief UVトランスフォーム
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

				/// @brief シャドウマップ描画
				bool drawShadowMap;

				/// @brief シャドウ有効化
				bool enableShadow;
			};

			// 分割
			struct Division
			{
				// スライス数
				int32_t slices;
			};

			// サイズ
			struct Size
			{
				// 内半径
				float inRadius;

				// 外半径
				float outRadius;
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

				/// @brief 分割
				Division division;

				/// @brief 大きさ
				Size size;

				/// @brief ブラー
				Blur blur;
			};
		}

		/// @brief 円柱
		namespace Cylinder
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

			// @brief UVトランスフォーム
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

				/// @brief シャドウマップ描画
				bool drawShadowMap;

				/// @brief シャドウ有効化
				bool enableShadow;
			};

			// 分割
			struct Division
			{
				// スライス数
				int32_t slices;
			};

			// 大きさ
			struct Size
			{
				// 上半径
				float topRadius;

				// 下半径
				float bottomRadius;

				// 高さ
				float height;
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

				/// @brief 分割
				Division division;

				/// @brief 大きさ
				Size size;

				/// @brief ブラー
				Blur blur;
			};
		}
	}
}