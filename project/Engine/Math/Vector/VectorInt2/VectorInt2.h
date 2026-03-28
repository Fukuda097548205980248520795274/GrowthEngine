#pragma once

struct VectorInt2
{
	int x;
	int y;

	/// @brief 加算する
	/// @param vector 
	/// @return 
	VectorInt2 operator+=(const VectorInt2& vector)
	{
		this->x += vector.x;
		this->y += vector.y;
		return *this;
	}

	/// @brief 減算する
	/// @param vector 
	/// @return 
	VectorInt2 operator-=(const VectorInt2& vector)
	{
		this->x -= vector.x;
		this->y -= vector.y;
		return *this;
	}

	/// @brief スカラー倍
	/// @param scalar 
	/// @return 
	VectorInt2 operator*=(int scalar)
	{
		this->x *= scalar;
		this->y *= scalar;
		return *this;
	}

	/// @brief スカラー除算
	/// @param scalar 
	/// @return 
	VectorInt2 operator/=(int scalar)
	{
		this->x /= scalar;
		this->y /= scalar;
		return *this;
	}
};

namespace
{
	/// @brief 加算
	/// @param v1 
	/// @param v2 
	/// @return 
	VectorInt2 operator+(const VectorInt2& v1, const VectorInt2& v2)
	{
		VectorInt2 add = VectorInt2(0, 0);
		add.x = v1.x + v2.x;
		add.y = v1.y + v2.y;
		return add;
	}

	/// @brief 減算
	/// @param v1 
	/// @param v2 
	/// @return 
	VectorInt2 operator-(const VectorInt2& v1, const VectorInt2& v2)
	{
		VectorInt2 subtract = VectorInt2(0, 0);
		subtract.x = v1.x - v2.x;
		subtract.y = v1.y - v2.y;
		return subtract;
	}

	/// @brief スカラー倍
	/// @param scalar 
	/// @param vector 
	/// @return 
	VectorInt2 operator*(int scalar, const VectorInt2& vector)
	{
		VectorInt2 multiply = VectorInt2(0, 0);
		multiply.x = scalar * vector.x;
		multiply.y = scalar * vector.y;
		return multiply;
	}

	/// @brief スカラー倍
	/// @param vector 
	/// @param scalar 
	/// @return 
	VectorInt2 operator*(const VectorInt2& vector, int scalar)
	{
		VectorInt2 multiply = VectorInt2(0, 0);
		multiply.x = vector.x * scalar;
		multiply.y = vector.y * scalar;
		return multiply;
	}

	/// @brief スカラー除算
	/// @param scalar 
	/// @param vector 
	/// @return 
	VectorInt2 operator/(int scalar, const VectorInt2& vector)
	{
		VectorInt2 division = VectorInt2(0, 0);
		division.x = scalar / vector.x;
		division.y = scalar / vector.y;
		return division;
	}

	/// @brief スカラー除算
	/// @param vector 
	/// @param scalar 
	/// @return 
	VectorInt2 operator/(const VectorInt2& vector, int scalar)
	{
		VectorInt2 division = VectorInt2(0, 0);
		division.x = vector.x / scalar;
		division.y = vector.y / scalar;
		return division;
	}

	/// @brief +
	/// @param vector 
	/// @return 
	VectorInt2 operator+(const VectorInt2& vector)
	{
		return vector;
	}

	/// @brief -
	/// @param vector 
	/// @return 
	VectorInt2 operator-(const VectorInt2& vector)
	{
		return VectorInt2(-vector.x, -vector.y);
	}
}