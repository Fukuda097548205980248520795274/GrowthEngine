#pragma once
#include "../BaseCollisionBase.h"

class CollisionInstanceAABB;

class CollisionBaseAABB : public Engine::BaseCollisionBase
{
public:

	/// @brief コンストラクタ
	/// @param func 
	/// @param name 
	CollisionBaseAABB(std::function<void()> func, const std::string& name);

	/// @brief インスタンスを作成する
	/// @return 
	CollisionInstanceAABB* CreateInstance();
};

