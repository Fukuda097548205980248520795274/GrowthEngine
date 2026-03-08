#pragma once
#include "../BaseCollision2DBase.h"

class Collision2DInstanceCircle;

class Collision2DBaseCircle : public Engine::BaseCollision2DBase
{
public:

	/// @brief コンストラクタ
	/// @param name 
	Collision2DBaseCircle(const std::string& name);

	/// @brief インスタンスを作成する
	/// @return 
	Collision2DInstanceCircle* CreateInstance();
};