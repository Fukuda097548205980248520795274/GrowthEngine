#pragma once
#include <vector>
#include "Math/Vector/Vector2/Vector2.h"
#include "Math/Vector/Vector4/Vector4.h"
#include "Handle/Handle.h"
#include "PSO/EnumBlendMode/EnumBlendMode.h"

namespace Engine
{
	namespace Render2D
	{
		/// @brief 種類
		enum class Type
		{
			Sprite,
			Text
		};

		// @brief スクリーンアンカー
		enum class ScreenAnchor
		{
			LeftTop,
			Top,
			RightTop,
			Left,
			Center,
			Right,
			LeftBottom,
			Bottom,
			RightBottom
		};

		/// @brief 矩形
		namespace Sprite
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

				/// @brief テクスチャハンドル
				TextureHandle hTexture;
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
				/// @brief ブレンドモード
				BlendMode blendMode;

				/// @brief 画面のアンカー
				ScreenAnchor screenAnchor = ScreenAnchor::LeftBottom;

				/// @brief トランスフォーム
				Transform transform;

				/// @brief マテリアル
				Material material;

				/// @brief テクスチャ
				Texture texture;
			};
		}

		/// @brief テキスト
		namespace Text
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
			};

			/// @brief テクスチャ
			struct Texture
			{
				Vector2 anchor;
			};

			/// @brief パラメータ
			struct Param
			{
				/// @brief ブレンドモード
				BlendMode blendMode;

				/// @brief 画面のアンカー
				ScreenAnchor screenAnchor = ScreenAnchor::LeftBottom;

				/// @brief トランスフォーム
				Transform transform;

				/// @brief テクスチャ
				Texture texture;

				/// @brief 文字トランスフォーム
				std::vector<Transform> charTransform;

				/// @brief 文字テクスチャ
				std::vector<Texture> charTexture;

				/// @brief 文字マテリアル
				std::vector<Material> charMaterial;
			};
		}
	}
}