#pragma once
#include "Input/Input.h"

/// @brief マウスボタンの種類
enum class MouseButton
{
	Left,
	Right,
	Middle
};

namespace Engine
{
	namespace InputData
	{
		/// @brief キー
		struct KeyParam
		{
			/// @brief キー
			BYTE key;
		};

		/// @brief ゲームパッドボタン
		struct GamepadButtonParam
		{
			/// @brief コントローラ
			DWORD controller;

			/// @brief ボタン
			DWORD button;
		};

		/// @brief ゲームパッドスティック
		struct GamepadStickParam
		{
			/// @brief コントローラ
			DWORD controller;

			/// @brief 方向
			Vector2 direction;

			/// @brief 内積
			float dot;
		};

		/// @brief ゲームパッドトリガー
		struct GamepadTriggerParam
		{
			/// @brief コントローラ
			DWORD controller;

			/// @brief 閾値
			float threshold;
		};

		/// @brief マウスボタン
		struct MouseButtonParam
		{
			/// @brief ボタン
			MouseButton button;
		};
	}
}