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

		/// @brief ジッタリングして更新処理
		virtual void JitterUpdate() = 0;


		/// @brief 現在のビュー正射影行列を取得する
		/// @return 
		Matrix4x4 GetCurrentVPMatrix()const { return currentVPMatrix_; }

		/// @brief 前フレームのビュー正射影行列を取得する
		/// @return 
		Matrix4x4 GetPrevVPMatrix()const { return prevVPMatrix_; }

		/// @brief ジッタリングなしの現在のビュー正射影行列を取得する
		/// @return 
		Matrix4x4 GetCurrentVPUnJitterMatrix()const { return currentVPUnJitterMatrix_; }

		/// @brief ジッタリングなしの前フレームのビュー正射影行列を取得する
		/// @return 
		Matrix4x4 GetPrevVPUnJitterMatrix()const { return prevVPUnJitterMatrix_; }

		/// @brief 正射影行列を取得する
		/// @return 
		Matrix4x4 GetProjectionMatrix()const { return projectionMatrix_; }

		/// @brief ワールド行列を取得する
		/// @return 
		Matrix4x4 GetWorldMatrix()const { return worldMatrix_; }

		/// @brief ビュー行列を取得する
		/// @return 
		Matrix4x4 GetViewMatrix()const { return worldMatrix_.Inverse(); }

		/// @brief ワールド座標を取得する
		/// @return 
		Vector3 GetWorldPosition()const { return Vector3(worldMatrix_.m[3][0], worldMatrix_.m[3][1], worldMatrix_.m[3][2]); }


	protected:

		// ワールド行列
		Matrix4x4 worldMatrix_ = MakeIdentityMatrix4x4();

		// 正射影行列
		Matrix4x4 projectionMatrix_ = MakeIdentityMatrix4x4();


	protected:

		// ジッタリングのインデックス
		int32_t jitterIndex_ = 0;

		// 現在のビュー正射影行列
		Matrix4x4 currentVPMatrix_ = MakeIdentityMatrix4x4();

		// 前フレームのビュー正射影行列
		Matrix4x4 prevVPMatrix_ = MakeIdentityMatrix4x4();

		// ジッタリングなしの現在のビュー正射影行列
		Matrix4x4 currentVPUnJitterMatrix_ = MakeIdentityMatrix4x4();

		// ジッタリングなしの前フレームのビュー正射影行列
		Matrix4x4 prevVPUnJitterMatrix_ = MakeIdentityMatrix4x4();
	};
}