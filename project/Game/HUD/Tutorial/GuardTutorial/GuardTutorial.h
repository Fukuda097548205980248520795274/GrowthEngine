#pragma once
#include "../Tutorial.h"

class GuardTutorial : public Tutorial
{
public:

	// 初期化用データ
	struct InitData
	{
		// 練習時間
		float practiceTime = 0.0f;

		// プレイヤー
		Player* player = nullptr;

		// ガードの最大回数
		int guardMaxCount = 0;

		/// @brief スプライト
		Sprite* sprite = nullptr;
	};


public:

	/// @brief 初期化処理
	/// @param initData 
	void Initialize(const InitData& initData);

	/// @brief 更新処理
	void Update() override;


private:

	/// @brief ガードの回数
	int guardCount_ = 0;

	/// @brief ガードの最大回数
	int guardMaxCount_ = 0;
};

