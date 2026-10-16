#pragma once
#include "../HUD.h"

/// @brief イントロテキスト
class IntroText : public HUD
{
public:

	/// @brief 状態
	enum class State
	{
		FadeIn,
		Stop,
		FadeOut,
	};

	/// @brief 初期化用データ
	struct InitData
	{
		Sprite* buttonSprite = nullptr;
	};


public:

	/// @brief デストラクタ
	~IntroText();

	/// @brief 初期化
	/// @param initData 
	void Initialize(const InitData& initData);

	/// @brief 更新処理
	void Update() override;

	/// @brief 描画処理
	void Draw() override;



private:

	/// @brief ボタンの内側のスプライト
	Sprite* buttonSprite_ = nullptr;

	/// @brief ボタンの状態
	State state_ = State::FadeIn;

	// @brief α値
	float alphaRate_ = 0.0f;


private:

	// フェードイン時間
	static constexpr float kFadeInTime = 0.25f;

	// フェードインタイマー
	float fadeInTimer_ = kFadeInTime;


private:

	/// @brief 停止時間
	static constexpr float kStopTime = 1.5f;

	/// @brief 停止タイマー
	float stopTimer_ = kStopTime;


private:

	// フェードアウト時間
	static constexpr float kFadeOutTime = 0.5f;

	/// @brief フェードアウトタイマー
	float fadeOutTimer_ = kFadeOutTime;
};

