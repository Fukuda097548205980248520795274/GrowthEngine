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

		/// @brief デバッグデータ : Guizmo
		struct DebugGuizmoData
		{
			/// @brief 選択しているかどうか
			bool isSelect = false;

			// モード
			GuizmoMode mode = GuizmoMode::Translate;
		};
	}
}