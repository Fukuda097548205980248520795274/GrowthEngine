#pragma once
#include "../BaseCollisionBase.h"

class CollisionInstanceOBB;

class CollisionBaseOBB : public Engine::BaseCollisionBase
{
public:

	/// @brief コンストラクタ
	/// @param func 
	/// @param name 
	CollisionBaseOBB(std::function<void()> func, const std::string& name);

	/// @brief インスタンスを作成する
	/// @return 
	CollisionInstanceOBB* CreateInstance();
};

