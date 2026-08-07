#pragma once

/// @brief 4次元ベクトル
struct Vector4
{
	float x;
	float y;
	float z;
	float w;
};

/// @brief 線形補間を行う
/// @param a 
/// @param b 
/// @param t 
/// @return 
Vector4 Lerp(const Vector4& a, const Vector4& b, float t);