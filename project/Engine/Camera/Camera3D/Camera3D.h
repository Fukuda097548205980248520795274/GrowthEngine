#pragma once
#include "../BaseCamera.h"
#include "Math/Quaternion/Quaternion.h"
#include "Data/CameraData/CameraData.h"
#include <memory>

class GrowthEngine;

namespace Engine
{
	class Camera3D : public BaseCamera
	{
	public:

		/// @brief コンストラクタ
		Camera3D();

		/// @brief 更新処理
		void Update() override;

		/// @brief クォータニオンを取得する
		/// @return 
		Quaternion GetQuaternion()const { return quaternion_; }

		/// @brief パラメータを取得する
		/// @return 
		Camera3DData::Param* GetParam() const { return param_.get(); }


	private:

		/// @brief パラメータ
		std::unique_ptr<Camera3DData::Param> param_ = nullptr;

		// クォータニオン
		Quaternion quaternion_ = MakeIdentityQuaternion();

		/// @brief エンジン
		const GrowthEngine* engine_ = nullptr;
	};
}