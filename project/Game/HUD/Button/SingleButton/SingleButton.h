#pragma once
#include "../Button.h"

class SingleButton : public Button
{
public:

	enum class State
	{
		None,
		FadeIn,
		OutScaling,
		FadeOut,
	};

	struct InitData
	{
		/// @brief ボタンのスプライト
		PrefabInstanceSprite* buttonSprite = nullptr;

		/// @brief ボタンの内側のスプライト
		PrefabInstanceSprite* buttonInSprite = nullptr;

		/// @brief ボタンの外側のスプライト
		PrefabInstanceSprite* buttonOutSprite = nullptr;

		/// @brief ボタンの位置
		Vector2 position = Vector2(0.0f, 0.0f);
	};


public:

	/// @brief デストラクタ
	~SingleButton();

	/// @brief 初期化
	/// @param initData 
	void Initialize(const InitData& initData);

	/// @brief 更新処理
	void Update() override;

	/// @brief 描画処理
	void Draw() override;

	/// @brief 入力処理
	void Input() override;

	/// @brief フェードインを開始する
	void FadeIn();

	/// @brief フェードアウトを開始する
	void FadeOut();



private:

	/// @brief ボタンの内側のスプライト
	PrefabInstanceSprite* buttonInSprite_ = nullptr;

	/// @brief ボタンの外側のスプライト
	PrefabInstanceSprite* buttonOutSprite_ = nullptr;

	/// @brief ボタンの状態
	State state_ = State::None;


private:

	// フェードイン時間
	static constexpr float kFadeInTime = 0.5f;

	// フェードインタイマー
	float fadeInTimer_ = kFadeInTime;


private:

	/// @brief アウトスケーリング時間
	static constexpr float kOutScalingTime = 1.0f;

	/// @brief アウトスケーリングタイマー
	float inScalingTimer_ = kOutScalingTime;


private:

	// フェードアウト時間
	static constexpr float kFadeOutTime = 0.5f;

	/// @brief フェードアウトタイマー
	float fadeOutTimer_ = kFadeOutTime;

};