#include "Vector3.h"
#include <cmath>

/// @brief 長さ
/// @return 
float Vector3::Length() const
{
	return std::sqrtf(std::powf(x, 2.0f) + std::powf(y, 2.0f) + std::powf(z, 2.0f));
}

/// @brief 正規化
/// @return 
Vector3 Vector3::Normalize() const
{
	// 正規化ベクトル
	Vector3 normalize = Vector3(0.0f, 0.0f, 0.0f);

	// 長さ
	float length = this->Length();

	if (length != 0.0f)
	{
		normalize = *this / length;
	}

	return normalize;
}

/// @brief 内積
/// @param v1 
/// @param v2 
/// @return 
float Dot(const Vector3& v1, const Vector3& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

/// @brief クロス積
/// @param v1 
/// @param v2 
/// @return 
Vector3 Cross(const Vector3& v1, const Vector3& v2)
{
	Vector3 cross = Vector3(0.0f, 0.0f, 0.0f);
	cross.x = v1.y * v2.z - v1.z * v2.y;
	cross.y = v1.z * v2.x - v1.x * v2.z;
	cross.z = v1.x * v2.y - v1.y * v2.x;
	return cross;
}

/// @brief 射影ベクトル
/// @param line 線
/// @param point 点
/// @return 
Vector3 Project(const Vector3& line, const Vector3& point)
{
	Vector3 normal = line.Normalize();
	Vector3 project = Dot(normal, point) * normal;
	return project;
}