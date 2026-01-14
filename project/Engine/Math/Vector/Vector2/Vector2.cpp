#include "Vector2.h"
#include <cmath>

/// @brief 長さ
/// @return 
float Vector2::Length() const
{
	return std::sqrtf(std::powf(x, 2.0f) + std::powf(y, 2.0f));
}

/// @brief 正規化
/// @return 
Vector2 Vector2::Normalize() const
{
	// 正規化ベクトル
	Vector2 normalize = Vector2(0.0f, 0.0f);

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
float Dot(const Vector2& v1, const Vector2& v2)
{
	return v1.x * v2.x + v1.y * v2.y;
}

/// @brief クロス積
/// @param v1 
/// @param v2 
/// @return 
float Cross(const Vector2& v1, const Vector2& v2)
{
	return v1.x * v2.y - v1.y * v2.x;
}

/// @brief 射影ベクトル
/// @param line 線
/// @param point 点
/// @return 
Vector2 Project(const Vector2& line, const Vector2& point)
{
	Vector2 normalize = line.Normalize();
	return Dot(normalize, point) * normalize;
}

/// @brief 反射ベクトル
/// @param input 入射ベクトル
/// @param normal 法線
/// @return 
Vector2 Reflect(const Vector2& input, const Vector2& normal)
{
	Vector2 normal2 = normal.Normalize();
	return input - (2.0f * (Dot(input, normal2) * normal2));
}