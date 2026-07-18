#pragma once
#include "Math/Vector/Vector3/Vector3.h"
#include <string>
#include <vector>
#include <numbers>
#include <algorithm>

// @brief 補間の種類
enum class InterpolationType
{
	// 等速
	Linear,

	// 徐々に加速
	EaseIn,

	// 徐々に減速
	EaseOut,

	// 加速して減速
	EaseInOut,

	// スプライン曲線
	CatmullRom
};

/// @brief キーの構造体
template<typename T>
struct Key
{
	// @brief 時間
	float time = 0.0f;

	// @brief 値
	T value;

	// @brief 補間の種類
	InterpolationType interpType = InterpolationType::Linear;
};

// / @brief キャラクターのキーフレーム
struct CharacterCutsceneTrack
{
	// キャラクター名
	std::string characterName;

	// キャラクターのキーフレーム
	std::vector<Key<Vector3>> positionKeys;
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

	// キャラクターのキーフレーム
	std::vector<CharacterCutsceneTrack> characterTracks;
};

/// @brief カメラのサンプリング結果を格納する構造体
struct CameraSample
{
	Vector3 position = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 rotation = Vector3(0.0f, 0.0f, 0.0f);
	float fov = 0.45f;
};

/// @brief キャラクターのサンプリング結果を格納する構造体
struct CharacterSample
{
	Vector3 position = Vector3(0.0f, 0.0f, 0.0f);
};

/// @brief 徐々に加速する補間関数
/// @param t 
/// @return 
inline float EaseIn(float t)
{
	return t * t;
}

/// @brief 徐々に減速する補間関数
/// @param t 
/// @return 
inline float EaseOut(float t)
{
	return t * (2.0f - t);
}

/// @brief 加速して減速する補間関数
/// @param t 
/// @return 
inline float EaseInOut(float t)
{
	return t < 0.5f ? 2.0f * t * t : -1.0f + (4.0f - 2.0f * t) * t;
}

/// @brief 補間関数を適用する
/// @param t 
/// @param type 
/// @return 
inline float ApplyEasing(float t, InterpolationType type)
{
	switch (type)
	{
		// 徐々に加速する補間関数
	case InterpolationType::EaseIn:
		return EaseIn(t);

		// 徐々に減速する補間関数
	case InterpolationType::EaseOut:
		return EaseOut(t);

		// 加速して減速する補間関数
	case InterpolationType::EaseInOut:
		return EaseInOut(t);

	default:
		return t;
	}
}

/// @brief Catmull-Romスプライン補間
/// @param p0 
/// @param p1 
/// @param p2 
/// @param p3 
/// @param t 
/// @return 
inline Vector3 CatmullRom(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t)
{
	float t2 = t * t;
	float t3 = t2 * t;
	return (
		p1 * 2.0f +
		(p2 - p0) * t +
		(p0 * 2.0f - p1 * 5.0f + p2 * 4.0f - p3) * t2 +
		(p1 * 3.0f - p0 - p2 * 3.0f + p3) * t3) * 0.5f;
}

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
	float rawT = (currentTime - prevKey.time) / (nextKey.time - prevKey.time);
	rawT = std::clamp(rawT, 0.0f, 1.0f);

	// Vector3型の場合のみ Catmull-Rom スプライン補間を適用
	if constexpr (std::is_same_v<T, Vector3>)
	{
		if (prevKey.interpType == InterpolationType::CatmullRom)
		{
			const auto& p0 = (prevIdx > 0) ? keys[prevIdx - 1].value : prevKey.value;
			const auto& p1 = prevKey.value;
			const auto& p2 = nextKey.value;
			const auto& p3 = (nextIdx + 1 < keys.size()) ? keys[nextIdx + 1].value : nextKey.value;
			return CatmullRom(p0, p1, p2, p3, rawT);
		}
	}

	// 補間係数を計算
	float t = ApplyEasing(rawT, prevKey.interpType);
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
	float rawT = (currentTime - prevKey.time) / (nextKey.time - prevKey.time);
	rawT = std::clamp(rawT, 0.0f, 1.0f);

	// 回転にはスプラインではなくイージング付きの球状/線形補間を使用する
	float t = ApplyEasing(rawT, prevKey.interpType);
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

/// @brief キャラクターのトラックのサンプリング
/// @param track 
/// @param currentTime 
/// @return 
inline CharacterSample SampleCharacterTrack(const CharacterCutsceneTrack& track, float currentTime)
{
	CharacterSample sample;
	sample.position = SampleKey<Vector3>(track.positionKeys, currentTime);
	return sample;
}