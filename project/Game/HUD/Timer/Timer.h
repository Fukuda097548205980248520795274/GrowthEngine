#pragma once
#include "../HUD.h"

class Timer : public HUD
{
public:

	struct InitData
	{
		// 時間用スプライト
		PrefabInstanceSprite* timerSprite[4];

		// コンマ用スプライト
		PrefabInstanceSprite* commaSprite;

		// 現在の時間
		float currentTime = 0.0f;
	};


public:

	/// @brief デストラクタ
	~Timer();

	/// @brief 初期化
	/// @param initData 
	void Initialize(const InitData& initData);

	/// @brief 更新処理
	void Update() override;

	/// @brief 描画処理
	void Draw() override;



private:

	// 現在の時間
	float currentTime_ = 0.0f;

	// 時間用スプライト
	PrefabInstanceSprite* timerSprite_[4];

	// コンマ用スプライト
	PrefabInstanceSprite* commaSprite_;

	// 時間の各桁の値
	int digit_[4] = { 0,0,0,0 };
};

