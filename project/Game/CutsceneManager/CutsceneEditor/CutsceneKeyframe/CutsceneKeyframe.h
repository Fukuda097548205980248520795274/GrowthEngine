#pragma once
#include "Math/Vector/Vector3/Vector3.h"
#include <string>
#include <vector>
#include <numbers>
#include <algorithm>

/// @brief キーの構造体
template<typename T>
struct Key
{
	float time = 0.0f;

	T value;
};

/// @brief キーフレームカットシーンデータ
struct KeyframeCutsceneData
{
	// 名前
	std::string name;

	// 時間
	float duration = 0.0f;

	// カメラのキーフレーム
	std::vector<Key<Vector3>> positionKeys;
	std::vector<Key<Vector3>> rotationKeys;
	std::vector<Key<float>> fovKeys;
};

/// @brief サンプリング結果を格納する構造体
struct CameraSample
{
	Vector3 position = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 rotation = Vector3(0.0f, 0.0f, 0.0f);
	float fov = 0.45f;
};

/// @brief 角度の線形補間
/// @param start 
/// @param end 
/// @param t 
/// @return 
inline float AngleLerp(float start, float end, float t)
{
	// 角度の差を計算
	float diff = end - start;

	// 差が180度を超える場合は、反対方向に回転するように調整
	while (diff < -std::numbers::pi_v<float>)
		diff += std::numbers::pi_v<float> *2.0f;

	while(diff > std::numbers::pi_v<float>)
		diff -= std::numbers::pi_v<float> *2.0f;

	// 線形補間
	return start + diff * t;
}

/// @brief 回転の線形補間
/// @param start 
/// @param end 
/// @param t 
/// @return 
inline Vector3 LerpRotation(const Vector3& start, const Vector3& end, float t)
{
	return Vector3(AngleLerp(start.x, end.x, t), AngleLerp(start.y, end.y, t), AngleLerp(start.z, end.z, t));
}

/// @brief 線形補間
/// @tparam T 
/// @param start 
/// @param end 
/// @param t 
/// @return 
template<typename T>
inline T Lerp(const T& start, const T& end, float t)
{
	return start + (end - start) * t;
}

/// @brief キーフレームのサンプリング
/// @tparam T 
/// @param keys 
/// @param currentTime 
/// @return 
template<typename T>
inline T SampleKey(const std::vector<Key<T>>& keys, float currentTime)
{
	// キーフレームが存在しない場合はデフォルトの値を返す
	if (keys.empty())
		return T();

	// 最初のキーフレームを超えた場合は最初のキーフレームを返す
	if (currentTime <= keys.front().time)
		return keys.front().value;

	// 最後のキーフレームを超えた場合は最後のキーフレームを返す
	if (currentTime >= keys.back().time)
		return keys.back().value;

	// 次のキーフレームのインデックスを見つける
	size_t nextIdx = 0;
	for (size_t i = 0; i < keys.size(); ++i)
	{
		if (keys[i].time > currentTime)
		{
			nextIdx = i;
			break;
		}
	}

	// 前後のキーフレームを取得
	size_t prevIdx = nextIdx - 1;
	const auto& prevKey = keys[prevIdx];
	const auto& nextKey = keys[nextIdx];

	// 補間係数を計算
	float t = (currentTime - prevKey.time) / (nextKey.time - prevKey.time);
	t = std::clamp(t, 0.0f, 1.0f);

	// 線形補間
	return Lerp(prevKey.value, nextKey.value, t);
}

/// @brief 回転のキーフレームのサンプリング
/// @param keys 
/// @param currentTime 
/// @return 
inline Vector3 SampleRotateKey(const std::vector<Key<Vector3>>& keys, float currentTime)
{
	// キーフレームが存在しない場合はデフォルトの値を返す
	if (keys.empty())
		return Vector3();

	// 最初のキーフレームを超えた場合は最初のキーフレームを返す
	if (currentTime <= keys.front().time)
		return keys.front().value;

	// 最後のキーフレームを超えた場合は最後のキーフレームを返す
	if (currentTime >= keys.back().time)
		return keys.back().value;

	// 次のキーフレームのインデックスを見つける
	size_t nextIdx = 0;
	for (size_t i = 0; i < keys.size(); ++i)
	{
		if (keys[i].time > currentTime)
		{
			nextIdx = i;
			break;
		}
	}

	// 前後のキーフレームを取得
	size_t prevIdx = nextIdx - 1;
	const auto& prevKey = keys[prevIdx];
	const auto& nextKey = keys[nextIdx];

	// 補間係数を計算
	float t = (currentTime - prevKey.time) / (nextKey.time - prevKey.time);
	t = std::clamp(t, 0.0f, 1.0f);

	// 線形補間
	return LerpRotation(prevKey.value, nextKey.value, t);
}

/// @brief カットシーンのサンプリング
/// @param data 
/// @param currentTime 
/// @return 
inline CameraSample SampleCutscene(const KeyframeCutsceneData& data, float currentTime)
{
	CameraSample sample;
	sample.position = SampleKey<Vector3>(data.positionKeys, currentTime);
	sample.rotation = SampleRotateKey(data.rotationKeys, currentTime);
	sample.fov = SampleKey<float>(data.fovKeys, currentTime);
	return sample;
}