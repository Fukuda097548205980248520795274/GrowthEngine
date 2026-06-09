#pragma once
#include "GrowthEngine.h"

class CameraShake
{
public:

	/// @brief シェイクを開始する
	/// @param duration 
	/// @param magnitude 
	/// @param direction 
	void Shake(float duration, float magnitude, const Vector3& direction = Vector3(1.0f, 1.0f, 1.0f));

	/// @brief 更新処理
	/// @param dt 
	void Update(float dt);

	/// @brief シェイクのオフセットを取得する
	/// @return 
	Vector3 GetShakeOffset() const;


private:

	// シェイクの経過時間
	float timer_ = 0.0f;

	// シェイクの総時間
	float duration_ = 0.0f;

	// シェイクの強さ（位置の最大オフセット）
	float magnitude_ = 0.5f;

	// シェイクの方向（ランダムに変化させる）
	Vector3 direction_ = Vector3(1.0f, 1.0f, 1.0f);
};

