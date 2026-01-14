#pragma once

/// @brief 3次元ベクトル
struct Vector3
{
	float x;
	float y;
	float z;

	/// @brief 長さ
	/// @return 
	float Length() const;

	/// @brief 正規化
	/// @return 
	Vector3 Normalize() const;
};

namespace
{
	/// @brief スカラー倍
	/// @param scalar 
	/// @param vector 
	/// @return 
	Vector3 operator*(float scalar, const Vector3& vector)
	{
		Vector3 multiply = Vector3(0.0f, 0.0f, 0.0f);
		multiply.x = scalar * vector.x;
		multiply.y = scalar * vector.y;
		multiply.z = scalar * vector.z;
		return multiply;
	}

	/// @brief スカラー倍
	/// @param vector 
	/// @param scalar 
	/// @return 
	Vector3 operator*(const Vector3& vector, float scalar)
	{
		Vector3 multiply = Vector3(0.0f, 0.0f, 0.0f);
		multiply.x = vector.x * scalar;
		multiply.y = vector.y * scalar;
		multiply.z = vector.z * scalar;
		return multiply;
	}

	/// @brief スカラー除算
	/// @param scalar 
	/// @param vector 
	/// @return 
	Vector3 operator/(float scalar, const Vector3& vector)
	{
		Vector3 division = Vector3(0.0f, 0.0f, 0.0f);
		division.x = scalar / vector.x;
		division.y = scalar / vector.y;
		division.z = scalar / vector.z;
		return division;
	}

	/// @brief スカラー除算
	/// @param vector 
	/// @param scalar 
	/// @return 
	Vector3 operator/(const Vector3& vector, float scalar)
	{
		Vector3 division = Vector3(0.0f, 0.0f, 0.0f);
		division.x = vector.x / scalar;
		division.y = vector.y / scalar;
		division.z = vector.z / scalar;
		return division;
	}
}

/// @brief 内積
/// @param v1 
/// @param v2 
/// @return 
float Dot(const Vector3& v1, const Vector3& v2);

/// @brief クロス積
/// @param v1 
/// @param v2 
/// @return 
Vector3 Cross(const Vector3& v1, const Vector3& v2);

/// @brief 射影ベクトル
/// @param line 線
/// @param point 点
/// @return 
Vector3 Project(const Vector3& line, const Vector3& point);