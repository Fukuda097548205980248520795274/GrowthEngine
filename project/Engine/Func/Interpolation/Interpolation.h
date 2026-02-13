#pragma once

/// @brief 線形補間
/// @tparam T 
/// @param start 
/// @param end 
/// @param t 
/// @return 
template<typename T>
T Lerp(T start, T end, float t)
{
	return start * (1.0f - t) + end * t;
}