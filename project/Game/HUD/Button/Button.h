#pragma once
#include "../HUD.h"

class Button : public HUD
{
private:

	/// @brief 入力処理
	virtual void Input() = 0;


protected:

	/// @brief ボタンのスプライト
	Sprite* buttonSprite;

	/// @brief α値の割合
	float alphaRate = 1.0f;
};

