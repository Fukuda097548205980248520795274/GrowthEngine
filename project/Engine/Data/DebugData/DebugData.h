#pragma once

namespace Engine
{
	namespace DebugData
	{
		// Guizmoのモード
		enum class GuizmoMode
		{
			Translate,
			Rotate,
			Scale
		};

		// Guizmoの次元
		enum class GuizmoDimension
		{
			Orthogothic,
			Perspective
		};

		/// @brief デバッグデータ : Guizmo
		struct DebugGuizmoData
		{
			/// @brief 選択しているかどうか
			bool isSelect = false;

			// モード
			GuizmoMode mode = GuizmoMode::Translate;

			// 次元
			GuizmoDimension dimension;
		};
	}
}