#pragma once
#include "../Effect.h"

class RepelEffect : public Effect
{
public:

	/// @brief コンストラクタ
	RepelEffect() { startTime_ = 0.04f; duration_ = startTime_; }

	/// @brief 初期化
	/// @param instanceModel 
	/// @param position 
	/// @param rotation 
	void Initialize(PrefabInstanceTube* instanceModel0, PrefabInstanceTube* instanceModel1, PrefabInstanceTube* instanceModel2, const Vector3& position);

	/// @brief 更新処理
	void Update() override;

	/// @brief 描画処理
	void Draw();


private:

	/// @brief インスタンスモデル
	PrefabInstanceTube* instanceModel0_ = nullptr;
	PrefabInstanceTube* instanceModel1_ = nullptr;
	PrefabInstanceTube* instanceModel2_ = nullptr;
};

