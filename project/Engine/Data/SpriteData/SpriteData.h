#pragma once
#include "Math/Vector/Vector2/Vector2.h"
#include "Math/Vector/Vector4/Vector4.h"
#include "Handle/Handle.h"

namespace Engine
{
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
}