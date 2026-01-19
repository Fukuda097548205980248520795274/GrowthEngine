#pragma once
#include "Math/Vector/Vector3/Vector3.h"
#include "Math/Quaternion/Quaternion.h"
#include "Math/Matrix/Matrix4x4/Matrix4x4.h"

class Camera3D
{
public:

	/// @brief コンストラクタ
	Camera3D();

	/// @brief デストラクタ
	~Camera3D() = default;


	/// @brief 更新処理
	void Update();

	/// @brief ビュー正射影行列を取得する
	/// @return 
	Matrix4x4 GetViewProjectionMatrix()const { return viewProjectionMatrix_; }

	/// @brief ワールド行列を取得する
	/// @return 
	Matrix4x4 GetWorldMatrix()const { return worldMatrix_; }

	/// @brief クォータニオンを取得する
	/// @return 
	Quaternion GetQuaternion()const { return quaternion_; }

	/// @brief ワールド座標を取得する
	/// @return 
	Vector3 GetWorldPosition()const;


public:

	// 回転
	Vector3 rotate_ = Vector3(0.0f, 0.0f, 0.0f);

	// 平行移動
	Vector3 translate_ = Vector3(0.0f, 0.0f, 0.0f);


private:

	// ワールド行列
	Matrix4x4 worldMatrix_ = MakeIdentityMatrix4x4();

	// 正射影行列
	Matrix4x4 projectionMatrix_ = MakeIdentityMatrix4x4();

	// ビュー正射影行列
	Matrix4x4 viewProjectionMatrix_ = MakeIdentityMatrix4x4();

	// クォータニオン
	Quaternion quaternion_ = MakeIdentityQuaternion();



	// 有効視野
	float fov_ = 0.45f;


	// 近平面
	float nearClip_ = 0.01f;

	// 遠平面
	float farClip_ = 800.0f;


	// 横幅
	float width_ = 0.0f;

	// 縦幅
	float height_ = 0.0f;
};

