#pragma once
#include "../Tutorial.h"

class AttackTutorial : public Tutorial
{
public:

	// 初期化用データ
	struct InitData
	{
		// 練習時間
		float practiceTime = 0.0f;

		// スプライト
		Sprite* sprite = nullptr;

		// プレイヤー
		Player* player = nullptr;

		// 攻撃の最大回数
		int attackMaxCount = 0;
	};


public:

	/// @brief 初期化処理
	/// @param initData 
	void Initialize(const InitData& initData);

	/// @brief 更新処理
	void Update() override;


private:

	/// @brief 攻撃の回数
	int attackCount_ = 0;

	/// @brief 攻撃の最大回数
	int attackMaxCount_ = 0;
};

