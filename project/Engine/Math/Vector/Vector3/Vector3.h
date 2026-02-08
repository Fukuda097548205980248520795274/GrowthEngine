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


	/// @brief 加算する
	/// @param vector 
	/// @return 
	Vector3 operator+=(const Vector3& vector)
	{
		this->x += vector.x;
		this->y += vector.y;
		this->z += vector.z;
		return *this;
	}

	/// @brief 減算する
	/// @param vector 
	/// @return 
	Vector3 operator-=(const Vector3& vector)
	{
		this->x -= vector.x;
		this->y -= vector.y;
		this->z -= vector.z;
		return *this;
	}

	/// @brief スカラー倍
	/// @param scalar 
	/// @return 
	Vector3 operator*=(float scalar)
	{
		this->x *= scalar;
		this->y *= scalar;
		this->z *= scalar;
		return *this;
	}

	/// @brief スカラー除算
	/// @param scalar 
	/// @return 
	Vector3 operator/=(float scalar)
	{
		this->x /= scalar;
		this->y /= scalar;
		this->z /= scalar;
		return *this;
	}
};

namespace
{
	/// @brief 加算
	/// @param v1 
	/// @param v2 
	/// @return 
	Vector3 operator+(const Vector3& v1, const Vector3& v2)
	{
		Vector3 add = Vector3(0.0f, 0.0f, 0.0f);
		add.x = v1.x + v2.x;
		add.y = v1.y + v2.y;
		add.z = v1.z + v2.z;
		return add;
	}

	/// @brief 減算
	/// @param v1 
	/// @param v2 
	/// @return 
	Vector3 operator-(const Vector3& v1, const Vector3& v2)
	{
		Vector3 subtract = Vector3(0.0f, 0.0f, 0.0f);
		subtract.x = v1.x - v2.x;
		subtract.y = v1.y - v2.y;
		subtract.z = v1.z - v2.z;
		return subtract;
	}

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

	/// @brief +
	/// @param vector 
	/// @return 
	Vector3 operator+(const Vector3& vector)
	{
		return vector;
	}

	/// @brief -
	/// @param vector 
	/// @return 
	Vector3 operator-(const Vector3& vector)
	{
		return Vector3(-vector.x, -vector.y, -vector.z);
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

/// @brief 反射ベクトル
/// @param input 入射ベクトル
/// @param normal 法線
/// @return 
Vector3 Reflect(const Vector3& input, const Vector3& normal);

/// @brief 球面座標系
/// @param radius 半径
/// @param theta 
/// @param phi 
/// @return 
Vector3 SphericalCoordinate(float radius, float theta, float phi);