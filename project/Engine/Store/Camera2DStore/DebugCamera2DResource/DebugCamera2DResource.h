#pragma once
#include <memory>
#include <string>
#include "Camera/Camera2D/Camera2D.h"
#include "Parameter/DebugCameraParameter/DebugCameraParameter.h"

namespace Engine
{
	class DebugCamera2DResource
	{
	public:

		/// @brief コンストラクタ
		DebugCamera2DResource();

		/// @brief コンストラクタ
		~DebugCamera2DResource();

		/// @brief 更新処理
		void Update();

		/// @brief カメラデータを取得する
		/// @return 
		const Camera2D& GetCamera2D() { return *camera2d_; }

		/// @brief 有効かどうか
		/// @return 
		bool IsEnable() { return enable_; }


	private:

		// エンジン
		const GrowthEngine* engine_;


		// カメラ3D
		std::unique_ptr<Camera2D> camera2d_ = nullptr;

		/// @brief パラメータ
		std::unique_ptr<DebugCameraParameter> parameter_ = nullptr;

		// 有効
		bool enable_ = false;
	};
}