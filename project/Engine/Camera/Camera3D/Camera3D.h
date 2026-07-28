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

		/// @brief ジッタリングして更新処理
		void JitterUpdate() override;

		/// @brief クォータニオンを取得する
		/// @return 
		Quaternion GetQuaternion()const { return quaternion_; }

		/// @brief パラメータを取得する
		/// @return 
		Camera3DData::Param* GetParam() const { return param_.get(); }


	public:

		/// @brief 正射影かどうかを設定する
		/// @param isOrthographic 
		void SetOrthographic(bool isOrthographic) { isOrthographic_ = isOrthographic; }

		/// @brief 正射影かどうかを取得する
		/// @return 
		bool GetOrthographic() const { return isOrthographic_; }

		/// @brief 正射影サイズを設定する
		/// @param size 
		void SetOrthographicSize(float size) { orthographicSize_ = size; }

		/// @brief 正射影サイズを取得する
		/// @return 
		float GetOrthographicSize() const { return orthographicSize_; }


	private:

		/// @brief パラメータ
		std::unique_ptr<Camera3DData::Param> param_ = nullptr;

		// クォータニオン
		Quaternion quaternion_ = MakeIdentityQuaternion();

		/// @brief エンジン
		const GrowthEngine* engine_ = nullptr;

		// 正射影かどうか
		bool isOrthographic_ = false;

		// 正射影サイズ
		float orthographicSize_ = 50.0f;
	};
}