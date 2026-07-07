#pragma once
#include "../Tutorial.h"

class RageTutorial : public Tutorial
{
public:

	// 初期化用データ
	struct InitData
	{
		// 練習時間
		float practiceTime = 0.0f;

		// プレイヤー
		Player* player = nullptr;

		/// @brief ボタンHUD
		TriggerButton* buttonHud = nullptr;
	};


public:

	/// @brief デストラクタ
	~RageTutorial();

	/// @brief 初期化処理
	/// @param initData 
	void Initialize(const InitData& initData);

	/// @brief 更新処理
	void Update() override;

	/// @brief 描画処理
	void Draw() override;




private:

	/// @brief プレイヤー
	TriggerButton* buttonHud_ = nullptr;
};

