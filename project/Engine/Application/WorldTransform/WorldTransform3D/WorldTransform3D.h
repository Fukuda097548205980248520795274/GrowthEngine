#pragma once
#include "Math/Vector/Vector3/Vector3.h"
#include "Math/Quaternion/Quaternion.h"
#include "Math/Matrix/Matrix4x4/Matrix4x4.h"

class WorldTransform3D
{
public:

	WorldTransform3D() = default;
	~WorldTransform3D() = default;



	/// @brief 更新処理
	void Update();

	/// @brief 親を設定する
	/// @param parent 
	void SetParent(WorldTransform3D* parent) { parent_ = parent; }

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

	/// @brief 拡縮
	Vector3 scale_ = Vector3(1.0f, 0.0f, 0.0f);

	/// @brief 回転
	Vector3 rotate_ = Vector3(0.0f, 0.0f, 0.0f);

	/// @brief 位置
	Vector3 translate_ = Vector3(0.0f, 0.0f, 0.0f);


private:

	// ワールド行列
	Matrix4x4 worldMatrix_ = MakeIdentityMatrix4x4();

	// クォータニオン
	Quaternion quaternion_ = MakeIdentityQuaternion();

	// 親
	WorldTransform3D* parent_ = nullptr;
};

