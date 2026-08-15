#pragma once
#include "../HUD.h"

class Button : public HUD
{
public:

	/// @brief 入力処理
	virtual void Input() {}

	/// @brief 更新処理
	virtual void Update() override;

	/// @brief 描画処理
	virtual void Draw() override {}


protected:

	/// @brief ボタンのスプライト
	PrefabInstanceSprite* buttonSprite_ = nullptr;

	/// @brief α値の割合
	float alphaRate_ = 0.0f;
};

