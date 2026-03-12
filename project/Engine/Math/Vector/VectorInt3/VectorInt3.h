#pragma once

/// @brief 3次元ベクトル
struct VectorInt3
{
	int x;
	int y;
	int z;

	/// @brief 加算する
	/// @param vector 
	/// @return 
	VectorInt3 operator+=(const VectorInt3& vector)
	{
		this->x += vector.x;
		this->y += vector.y;
		this->z += vector.z;
		return *this;
	}

	/// @brief 減算する
	/// @param vector 
	/// @return 
	VectorInt3 operator-=(const VectorInt3& vector)
	{
		this->x -= vector.x;
		this->y -= vector.y;
		this->z -= vector.z;
		return *this;
	}

	/// @brief スカラー倍
	/// @param scalar 
	/// @return 
	VectorInt3 operator*=(int scalar)
	{
		this->x *= scalar;
		this->y *= scalar;
		this->z *= scalar;
		return *this;
	}

	/// @brief スカラー除算
	/// @param scalar 
	/// @return 
	VectorInt3 operator/=(int scalar)
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
	VectorInt3 operator+(const VectorInt3& v1, const VectorInt3& v2)
	{
		VectorInt3 add = VectorInt3(0, 0, 0);
		add.x = v1.x + v2.x;
		add.y = v1.y + v2.y;
		add.z = v1.z + v2.z;
		return add;
	}

	/// @brief 減算
	/// @param v1 
	/// @param v2 
	/// @return 
	VectorInt3 operator-(const VectorInt3& v1, const VectorInt3& v2)
	{
		VectorInt3 subtract = VectorInt3(0, 0, 0);
		subtract.x = v1.x - v2.x;
		subtract.y = v1.y - v2.y;
		subtract.z = v1.z - v2.z;
		return subtract;
	}

	/// @brief スカラー倍
	/// @param scalar 
	/// @param vector 
	/// @return 
	VectorInt3 operator*(int scalar, const VectorInt3& vector)
	{
		VectorInt3 multiply = VectorInt3(0, 0, 0);
		multiply.x = scalar * vector.x;
		multiply.y = scalar * vector.y;
		multiply.z = scalar * vector.z;
		return multiply;
	}

	/// @brief スカラー倍
	/// @param vector 
	/// @param scalar 
	/// @return 
	VectorInt3 operator*(const VectorInt3& vector, int scalar)
	{
		VectorInt3 multiply = VectorInt3(0, 0, 0);
		multiply.x = vector.x * scalar;
		multiply.y = vector.y * scalar;
		multiply.z = vector.z * scalar;
		return multiply;
	}

	/// @brief スカラー除算
	/// @param scalar 
	/// @param vector 
	/// @return 
	VectorInt3 operator/(int scalar, const VectorInt3& vector)
	{
		VectorInt3 division = VectorInt3(0, 0, 0);
		division.x = scalar / vector.x;
		division.y = scalar / vector.y;
		division.z = scalar / vector.z;
		return division;
	}

	/// @brief スカラー除算
	/// @param vector 
	/// @param scalar 
	/// @return 
	VectorInt3 operator/(const VectorInt3& vector, int scalar)
	{
		VectorInt3 division = VectorInt3(0, 0, 0);
		division.x = vector.x / scalar;
		division.y = vector.y / scalar;
		division.z = vector.z / scalar;
		return division;
	}

	/// @brief +
	/// @param vector 
	/// @return 
	VectorInt3 operator+(const VectorInt3& vector)
	{
		return vector;
	}

	/// @brief -
	/// @param vector 
	/// @return 
	VectorInt3 operator-(const VectorInt3& vector)
	{
		return VectorInt3(-vector.x, -vector.y, -vector.z);
	}
}