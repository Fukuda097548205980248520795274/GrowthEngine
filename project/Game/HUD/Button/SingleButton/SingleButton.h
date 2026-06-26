#pragma once
#include "../Button.h"

class SingleButton : public Button
{
public:

	enum class State
	{
		None,
		FadeIn,
		Input,
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

		/// @brief ボタンのスケール
		Vector2 scale = Vector2(1.0f, 1.0f);

		/// @brief 色
		Vector3 color = Vector3(1.0f, 1.0f, 1.0f);
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

	/// @brief 外側のα値の割合
	float outAlphaRate_ = 0.0f;


private:

	// フェードイン時間
	static constexpr float kFadeInTime = 0.25f;

	// フェードインタイマー
	float fadeInTimer_ = kFadeInTime;


private:

	/// @brief アウトスケーリング時間
	static constexpr float kOutScalingTime = 1.5f;

	/// @brief アウトスケーリングタイマー
	float inScalingTimer_ = kOutScalingTime;

	/// @brief ボタンの外側の大きさ
	static constexpr float kOutScale = 1.0f;


private:

	/// @brief 入力時間
	static constexpr float kInputTime = 0.2f;

	/// @brief 入力タイマー
	float inputTimer_ = kInputTime;

	/// @brief 入力時の外側の大きさ
	static constexpr float kInputOutScale = 2.0f;


private:

	// フェードアウト時間
	static constexpr float kFadeOutTime = 0.25f;

	/// @brief フェードアウトタイマー
	float fadeOutTimer_ = kFadeOutTime;

};