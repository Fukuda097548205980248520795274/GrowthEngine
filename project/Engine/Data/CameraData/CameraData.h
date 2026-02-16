#pragma once
#include "Math/Vector/Vector2/Vector2.h"
#include "Math/Vector/Vector3/Vector3.h"

namespace Engine
{
	/// @brief 3Dカメラ
	namespace Camera3DData
	{
		/// @brief トランスフォーム
		struct Transform
		{
			/// @brief 回転
			Vector3 rotate;

			/// @brief 移動
			Vector3 translate;
		};

		/// @brief 設定
		struct Setting
		{
			/// @brief 画角
			float fov;

			/// @brief 近平面
			float nearClip;

			/// @brief 遠平面
			float farClip;
		};

		/// @brief アスペクト
		struct Aspect
		{
			/// @brief 横幅
			float width;

			/// @brief 縦幅
			float height;
		};

		/// @brief パラメータ
		struct Param
		{
			/// @brief トランスフォーム
			Transform transform;

			/// @brief アスペクト
			Aspect aspect;

			/// @brief セッティング
			Setting setting;
		};
	}

	/// @brief 2Dカメラ
	namespace Camera2DData
	{
		/// @brief トランスフォーム
		struct Transform
		{
			/// @brief 回転
			float rotate;

			/// @brief 移動
			Vector2 translate;
		};

		/// @brief 設定
		struct Setting
		{
			/// @brief 近平面
			float nearClip;

			/// @brief 遠平面
			float farClip;
		};

		/// @brief アスペクト
		struct Aspect
		{
			/// @brief 横幅
			float width;

			/// @brief 縦幅
			float height;
		};

		/// @brief パラメータ
		struct Param
		{
			/// @brief トランスフォーム
			Transform transform;

			/// @brief アスペクト
			Aspect aspect;

			/// @brief セッティング
			Setting setting;
		};
	}
}