#pragma once
#include "Math/Matrix/Matrix4x4/Matrix4x4.h"

namespace Engine
{
	class BaseCamera
	{
	public:

		/// @brief 仮想デストラクタ
		virtual ~BaseCamera() = default;

		/// @brief 更新処理
		virtual void Update() = 0;

		/// @brief ビュー正射影行列を取得する
		/// @return 
		Matrix4x4 GetViewProjectionMatrix()const { return viewProjectionMatrix_; }

		/// @brief 正射影行列を取得する
		/// @return 
		Matrix4x4 GetProjectionMatrix()const { return projectionMatrix_; }

		/// @brief ワールド行列を取得する
		/// @return 
		Matrix4x4 GetWorldMatrix()const { return worldMatrix_; }

		/// @brief ワールド座標を取得する
		/// @return 
		Vector3 GetWorldPosition()const { return Vector3(worldMatrix_.m[3][0], worldMatrix_.m[3][1], worldMatrix_.m[3][2]); }


	protected:

		// ワールド行列
		Matrix4x4 worldMatrix_ = MakeIdentityMatrix4x4();

		// 正射影行列
		Matrix4x4 projectionMatrix_ = MakeIdentityMatrix4x4();

		// ビュー正射影行列
		Matrix4x4 viewProjectionMatrix_ = MakeIdentityMatrix4x4();
	};
}