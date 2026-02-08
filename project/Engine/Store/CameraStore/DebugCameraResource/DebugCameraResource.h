#pragma once
#include <memory>
#include <string>
#include "Application/Camera/Camera3D/Camera3D.h"

class GrowthEngine;

namespace Engine
{
	class DebugCameraResource
	{
	public:

		/// @brief コンストラクタ
		DebugCameraResource();

		/// @brief 更新処理
		void Update();

		/// @brief カメラデータを取得する
		/// @return 
		const Camera3D& GetCamera3D() { return *camera3d_; }

		/// @brief 有効かどうか
		/// @return 
		bool IsEnable() { return enable_; }


	private:

		// エンジン
		const GrowthEngine* engine_;


		// カメラ3D
		std::unique_ptr<Camera3D> camera3d_ = nullptr;

		// 有効
		bool enable_ = false;


		// ピボットポイント
		Vector3 pivotPoint_ = Vector3(0.0f, 0.0f, 0.0f);

		// ピボットポイントとの距離
		float pointLength_ = 50.0f;
	};
}