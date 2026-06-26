#pragma once
#include "../Button.h"

class MashButton : public Button
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

		// @brief 色
		Vector3 color = Vector3(1.0f, 1.0f, 1.0f);
	};


public:

	/// @brief デストラクタ
	~MashButton();

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

	/// @brief 外側のスプライトのα値の割合
	float outAlphaRate_ = 1.0f;


private:

	// フェードイン時間
	static constexpr float kFadeInTime = 0.5f;

	// フェードインタイマー
	float fadeInTimer_ = kFadeInTime;


private:

	// フェードアウト時間
	static constexpr float kFadeOutTime = 0.5f;

	/// @brief フェードアウトタイマー
	float fadeOutTimer_ = kFadeOutTime;


private:

	/// @brief 入力時間
	static constexpr float kInputTime = 0.2f;

	/// @brief 入力タイマー
	float inputTimer_ = kInputTime;

	/// @brief ボタンの外側の大きさ
	static constexpr float kInputOutScale = 1.2f;


private:

	/// @brief 外側のスケールパラメータ
	float outScaleParameter_ = 0.0f;

	/// @brief 外側のスケールパラメータの速度
	static constexpr float kOutScaleParameterSpeed = 6.0f;

};

