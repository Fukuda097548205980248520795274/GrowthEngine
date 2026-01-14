#pragma once

/// @brief 2次元ベクトル
struct Vector2
{
	float x;
	float y;

	/// @brief 長さ
	/// @return 
	float Length() const;

	/// @brief 正規化
	/// @return 
	Vector2 Normalize() const;


	// 加算
	Vector2 operator+=(const Vector2& vector)
	{
		this->x += vector.x;
		this->y += vector.y;
		return *this;
	}

	// 減算
	Vector2 operator-=(const Vector2& vector)
	{
		this->x -= vector.x;
		this->y -= vector.y;
		return *this;
	}

	// スカラー倍
	Vector2 operator*=(float scalar)
	{
		this->x *= scalar;
		this->y *= scalar;
		return *this;
	}

	// スカラー除算
	Vector2 operator/=(float scalar)
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
	Vector2 operator+(const Vector2& v1, const Vector2& v2)
	{
		Vector2 add = Vector2(0.0f, 0.0f);
		add.x = v1.x + v2.x;
		add.y = v1.y + v2.y;
		return add;
	}

	/// @brief 減算
	/// @param v1 
	/// @param v2 
	/// @return 
	Vector2 operator-(const Vector2& v1, const Vector2& v2)
	{
		Vector2 subtract = Vector2(0.0f, 0.0f);
		subtract.x = v1.x - v2.x;
		subtract.y = v1.y - v2.y;
		return subtract;
	}

	/// @brief スカラー倍
	/// @param scalar 
	/// @param vector 
	/// @return 
	Vector2 operator*(float scalar, const Vector2& vector)
	{
		Vector2 multiply = Vector2(0.0f, 0.0f);
		multiply.x = scalar * vector.x;
		multiply.y = scalar * vector.y;
		return multiply;
	}

	/// @brief スカラー倍
	/// @param vector 
	/// @param scalar 
	/// @return 
	Vector2 operator*(const Vector2& vector, float scalar)
	{
		Vector2 multiply = Vector2(0.0f, 0.0f);
		multiply.x = vector.x * scalar;
		multiply.y = vector.y * scalar;
		return multiply;
	}

	/// @brief スカラー除算
	/// @param scalar 
	/// @param vector 
	/// @return 
	Vector2 operator/(float scalar, const Vector2& vector)
	{
		Vector2 division = Vector2(0.0f, 0.0f);
		division.x = scalar / vector.x;
		division.y = scalar / vector.y;
		return division;
	}

	/// @brief スカラー除算
	/// @param vector 
	/// @param scalar 
	/// @return 
	Vector2 operator/(const Vector2& vector, float scalar)
	{
		Vector2 division = Vector2(0.0f, 0.0f);
		division.x = vector.x / scalar;
		division.y = vector.y / scalar;
		return division;
	}

	/// @brief +
	/// @param vector 
	/// @return 
	Vector2 operator+(const Vector2& vector)
	{
		return vector;
	}

	/// @brief -
	/// @param vector 
	/// @return 
	Vector2 operator-(const Vector2& vector)
	{
		return Vector2(-vector.x, -vector.y);
	}
}

/// @brief 内積
/// @param v1 
/// @param v2 
/// @return 
float Dot(const Vector2& v1, const Vector2& v2);

/// @brief クロス積
/// @param v1 
/// @param v2 
/// @return 
float Cross(const Vector2& v1, const Vector2& v2);

/// @brief 射影ベクトル
/// @param line 線
/// @param point 点
/// @return 
Vector2 Project(const Vector2& line, const Vector2& point);

/// @brief 反射ベクトル
/// @param input 入射ベクトル
/// @param normal 法線
/// @return 
Vector2 Reflect(const Vector2& input, const Vector2& normal);