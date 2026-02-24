#pragma once
#include "../BaseCollisionBase.h"

class CollisionInstancePlane;

class CollisionBasePlane : public Engine::BaseCollisionBase
{
public:

	/// @brief コンストラクタ
	/// @param func 
	/// @param name 
	CollisionBasePlane(std::function<void()> func, const std::string& name);

	/// @brief インスタンスを作成する
	/// @return 
	CollisionInstancePlane* CreateInstance();
};

