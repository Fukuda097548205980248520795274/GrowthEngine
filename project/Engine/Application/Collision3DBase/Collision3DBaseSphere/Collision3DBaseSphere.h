#pragma once
#include "../BaseCollision3DBase.h"

class Collision3DInstanceSphere;

class Collision3DBaseSphere : public Engine::BaseCollision3DBase
{
public:

	/// @brief コンストラクタ
	/// @param name 
	Collision3DBaseSphere(const std::string& name);

	/// @brief インスタンスを作成する
	/// @return 
	Collision3DInstanceSphere* CreateInstance();
};

