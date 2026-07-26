#pragma once
#include "../BaseCollision2DBase.h"

class Collision2DInstanceSprite;

class Collision2DBaseSprite : public Engine::BaseCollision2DBase
{
public:

	/// @brief コンストラクタ
	/// @param name 
	Collision2DBaseSprite(const std::string& name);

	/// @brief インスタンスを作成する
	/// @return 
	Collision2DInstanceSprite* CreateInstance();
};