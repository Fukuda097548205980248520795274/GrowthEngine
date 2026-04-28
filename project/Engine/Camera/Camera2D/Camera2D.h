#pragma once
#include "../BaseCamera.h"
#include "Data/CameraData/CameraData.h"
#include <memory>

class GrowthEngine;

namespace Engine
{
	class Camera2D : public BaseCamera
	{
	public:

		/// @brief コンストラクタ
		Camera2D();

		/// @brief 更新処理
		void Update() override;

		/// @brief ジッタリングして更新処理
		void JitterUpdate() override;

		/// @brief パラメータを取得する
		/// @return 
		Camera2DData::Param* GetParam() const { return param_.get(); }


	private:

		/// @brief パラメータ
		std::unique_ptr<Camera2DData::Param> param_ = nullptr;

		/// @brief エンジン
		const GrowthEngine* engine_ = nullptr;
	};
}