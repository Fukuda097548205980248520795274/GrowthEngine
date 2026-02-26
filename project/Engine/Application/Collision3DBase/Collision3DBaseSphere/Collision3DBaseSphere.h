#pragma once
#include "../BaseCollision3DBase.h"

class Collision3DInstanceSphere;

class Collision3DBaseSphere : public Engine::BaseCollision3DBase
{
public:

	/// @brief コンストラクタ
	/// @param func 
	/// @param name 
	Collision3DBaseSphere(std::function<void()> func, const std::string& name);

	/// @brief インスタンスを作成する
	/// @return 
	Collision3DInstanceSphere* CreateInstance();
};

