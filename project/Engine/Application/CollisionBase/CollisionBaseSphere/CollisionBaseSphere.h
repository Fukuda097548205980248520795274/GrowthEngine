#pragma once
#include "../BaseCollisionBase.h"

class CollisionInstanceSphere;

class CollisionBaseSphere : public Engine::BaseCollisionBase
{
public:

	/// @brief コンストラクタ
	/// @param func 
	/// @param name 
	CollisionBaseSphere(std::function<void()> func, const std::string& name);

	/// @brief インスタンスを作成する
	/// @return 
	CollisionInstanceSphere* CreateInstance();
};

