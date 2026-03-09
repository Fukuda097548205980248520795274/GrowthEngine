#pragma once
#include "Math/Vector/Vector3/Vector3.h"
#include "Math/Quaternion/Quaternion.h"
#include "Math/Matrix/Matrix4x4/Matrix4x4.h"

class WorldTransform2D
{
public:

	WorldTransform2D() = default;
	~WorldTransform2D() = default;



	/// @brief 更新処理
	void Update();

	/// @brief 親を設定する
	/// @param parent 
	void SetParent(WorldTransform2D* parent) { parent_ = parent; }

	/// @brief ワールド行列を取得する
	/// @return 
	Matrix4x4 GetWorldMatrix()const { return worldMatrix_; }

	/// @brief クォータニオンを取得する
	/// @return 
	Quaternion GetQuaternion()const { return quaternion_; }

	/// @brief ワールド座標を取得する
	/// @return 
	Vector2 GetWorldPosition()const;


public:

	/// @brief 拡縮
	Vector2 scale_ = Vector2(1.0f, 1.0f);

	/// @brief 回転
	float rotate_ = 0.0f;

	/// @brief 位置
	Vector2 translate_ = Vector2(0.0f, 0.0f);


private:

	// ワールド行列
	Matrix4x4 worldMatrix_ = MakeIdentityMatrix4x4();

	// クォータニオン
	Quaternion quaternion_ = MakeIdentityQuaternion();

	// 親
	WorldTransform2D* parent_ = nullptr;
};

