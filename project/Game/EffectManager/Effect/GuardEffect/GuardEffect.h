#pragma once
#include "../Effect.h"

class GuardEffect : public Effect
{
public:

	/// @brief コンストラクタ
	GuardEffect() { startTime_ = 0.15f; duration_ = startTime_; }

	/// @brief 初期化
	/// @param instanceModel 
	/// @param position 
	/// @param rotation 
	void Initialize(PrefabInstanceTube* instanceModel, const Vector3& position, const Vector3& rotation);

	/// @brief 更新処理
	void Update() override;

	/// @brief 描画処理
	void Draw();


private:

	/// @brief インスタンスモデル
	PrefabInstanceTube* instanceModel_ = nullptr;
};

