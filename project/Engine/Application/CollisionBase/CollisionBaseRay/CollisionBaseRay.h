#pragma once
#include "../BaseCollisionBase.h"

class CollisionInstanceRay;

class CollisionBaseRay : public Engine::BaseCollisionBase
{
public:

	/// @brief コンストラクタ
	/// @param func 
	/// @param name 
	CollisionBaseRay(std::function<void()> func, const std::string& name);

	/// @brief インスタンスを作成する
	/// @return 
	CollisionInstanceRay* CreateInstance();
};

