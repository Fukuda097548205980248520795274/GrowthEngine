#pragma once
#include "../Button.h"

class WeaponGetButton : public Button
{
public:

	// @brief 初期化データ
	struct InitData
	{
		PrefabInstanceSprite* buttonSprite = nullptr;
	};


public:

	/// @brief コンストラクタ
	/// @param initData 
	WeaponGetButton(const InitData& initData);

	/// @brief デストラクタ
	~WeaponGetButton();

	/// @brief 更新処理
	void Update() override;

	/// @brief 描画処理
	void Draw() override;

	/// @brief 有効化を設定する
	/// @param enable 
	void SetEnable(bool enable) { enable_ = enable; }

	/// @brief 壊れたことを通知する
	void Break();


private:

	// 有効化フラグ
	bool enable_ = false;

	/// @brief 前回の有効化フラグ
	bool prevEnable_ = false;

	/// @brief 0.0f ~ 1.0f の範囲でフェードイン・フェードアウトの進行度を表す値
	float t_ = 0.0f;

	// フェードインの時間
	static constexpr float kFadeInDuration = 0.5f;
	float fadeInTimer_ = 0.0f;

	// フェードアウトの時間
	static constexpr float kFadeOutDuration = 0.5f;
	float fadeOutTimer_ = 0.0f;

	// 武器が壊れたかどうか
	bool isBreak_ = false;
};

