#pragma once
#include "../HUD.h"

class Player;
class Timer;
class MashButton;
class SingleButton;

class Tutorial : public HUD
{
public:

	// 状態
	enum class State
	{
		Practice,
		Clear,
		FadeOut,
	};


public:

	/// @brief 仮想デストラクタ
	virtual ~Tutorial() = default;

	/// @brief コンストラクタ
	Tutorial() : HUD() { state_ = State::Practice; }

	/// @brief 更新処理
	virtual void Update() override;

	/// @brief 描画処理
	virtual void Draw()override {}

	/// @brief チュートリアルかどうかを判定する
	/// @return 
	bool IsTutorial() const override { return state_ == State::Practice; }


protected:

	// 練習タイマー
	float practiceTimer_ = 0.0f;

	// 練習時間
	float practiceTime_ = 0.0f;


protected:

	// クリア時間
	constexpr static float kClearTime = 1.0f;

	// クリアタイマー
	float clearTimer_ = kClearTime;


protected:

	// フェードアウト時間
	constexpr static float kFadeOutTime = 1.0f;

	// フェードアウトタイマー
	float fadeOutTimer_ = kFadeOutTime;


protected:

	// プレイヤー
	Player* player_ = nullptr;


protected:

	// 状態
	State state_ = State::Practice;
};

