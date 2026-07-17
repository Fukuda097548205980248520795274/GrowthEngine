#pragma once
#include "Math/Vector/Vector3/Vector3.h"
#include <string>
#include <vector>
#include <numbers>
#include <algorithm>

/// @brief カメラのキーフレーム
struct CameraKeyframe
{
	// 時間
	float time = 0.0f;

	// 位置
	Vector3 position = Vector3(0.0f, 0.0f, 0.0f);

	// 回転
	Vector3 rotation = Vector3(0.0f, 0.0f, 0.0f);

	// 画角
	float fov = 0.45f;
};

/// @brief キーフレームカットシーンデータ
struct KeyframeCutsceneData
{
	// 名前
	std::string name;

	// 時間
	float duration = 0.0f;

	// カメラのキーフレーム
	std::vector<CameraKeyframe> keyframes;
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

/// @brief カットシーンのサンプリング
/// @param data 
/// @param cutsceneTime 
/// @return 
inline CameraKeyframe SampleCutscene(const KeyframeCutsceneData& data, float currentTime)
{
	// キーフレームが存在しない場合はデフォルトのキーフレームを返す
	if (data.keyframes.empty()) 
		return CameraKeyframe();

	// 最初のキーフレームを超えた場合は最初のキーフレームを返す
	if (data.keyframes.size() == 1 || currentTime <= data.keyframes.front().time)
		return data.keyframes.front();

	// 最後のキーフレームを超えた場合は最後のキーフレームを返す
	if (currentTime >= data.keyframes.back().time)
		return data.keyframes.back();

	// 次のキーフレームのインデックスを見つける
	size_t nextIdx = 0;
	for (size_t i = 0; i < data.keyframes.size(); ++i)
	{
		// 現在の時間よりも後のキーフレームを見つける
		if (data.keyframes[i].time > currentTime)
		{
			nextIdx = i;
			break;
		}
	}

	// 前後のキーフレームを取得
	size_t prevIdx = nextIdx - 1;
	const auto& prevKey = data.keyframes[prevIdx];
	const auto& nextKey = data.keyframes[nextIdx];

	// 補間係数を計算
	float t = (currentTime - prevKey.time) / (nextKey.time - prevKey.time);
	t = std::clamp(t, 0.0f, 1.0f);

	// 線形補間
	CameraKeyframe result;
	result.time = currentTime;
	result.position = Lerp(prevKey.position, nextKey.position, t);
	result.rotation = LerpRotation(prevKey.rotation, nextKey.rotation, t);
	result.fov = prevKey.fov + (nextKey.fov - prevKey.fov) * t;

	return result;
}