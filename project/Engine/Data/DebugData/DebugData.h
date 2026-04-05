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


			// ローカル移動
			bool isScaleLocal_ = false;

			// ローカル回転
			bool isRotateLocal_ = false;

			// ローカル移動
			bool isTranslateLocal_ = false;
		};
	}
}