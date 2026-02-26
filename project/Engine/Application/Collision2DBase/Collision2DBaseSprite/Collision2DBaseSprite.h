#pragma once
#include "../BaseCollision2DBase.h"

class Collision2DInstanceSprite;

class Collision2DBaseSprite : public Engine::BaseCollision2DBase
{
public:

	/// @brief コンストラクタ
	/// @param func 
	/// @param name 
	Collision2DBaseSprite(std::function<void()> func, const std::string& name);

	/// @brief インスタンスを作成する
	/// @return 
	Collision2DInstanceSprite* CreateInstance();
};