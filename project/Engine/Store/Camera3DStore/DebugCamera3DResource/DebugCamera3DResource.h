#pragma once
#include <memory>
#include <string>
#include "Camera/Camera3D/Camera3D.h"
#include "Parameter/DebugCameraParameter/DebugCameraParameter.h"

class GrowthEngine;

namespace Engine
{
	class DebugCamera3DResource
	{
	public:

		/// @brief コンストラクタ
		DebugCamera3DResource();

		/// @brief デストラクタ
		~DebugCamera3DResource();

		/// @brief 更新処理
		void Update();

		/// @brief ジッタリングして更新処理
		void JitterUpdate();

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

		/// @brief パラメータ
		std::unique_ptr<DebugCameraParameter> parameter_ = nullptr;

		// 有効
		bool enable_ = false;


	private:

		/// @brief ピボットポイントの更新
		void PivotPointUpdate();

		// ピボットポイント
		Vector3 pivotPoint_ = Vector3(0.0f, 0.0f, 0.0f);

		// ピボットポイントとの距離
		float pointLength_ = 50.0f;
	};
}