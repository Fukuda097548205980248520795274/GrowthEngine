#pragma once
#include "../BaseCollision3DBase.h"

class Collision3DInstanceLine;

class Collision3DBaseLine : public Engine::BaseCollision3DBase
{
public:

	/// @brief コンストラクタ
	/// @param func 
	/// @param name 
	Collision3DBaseLine(std::function<void()> func, const std::string& name);

	/// @brief インスタンスを作成する
	/// @return 
	Collision3DInstanceLine* CreateInstance();
};

