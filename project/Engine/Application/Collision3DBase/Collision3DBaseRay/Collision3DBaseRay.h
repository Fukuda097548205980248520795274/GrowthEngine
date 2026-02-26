#pragma once
#include "../BaseCollision3DBase.h"

class Collision3DInstanceRay;

class Collision3DBaseRay : public Engine::BaseCollision3DBase
{
public:

	/// @brief コンストラクタ
	/// @param func 
	/// @param name 
	Collision3DBaseRay(std::function<void()> func, const std::string& name);

	/// @brief インスタンスを作成する
	/// @return 
	Collision3DInstanceRay* CreateInstance();
};

