#pragma once
#include "../BaseCollisionBase.h"

class CollisionInstanceSegment;

class CollisionBaseSegment : public Engine::BaseCollisionBase
{
public:

	/// @brief コンストラクタ
	/// @param func 
	/// @param name 
	CollisionBaseSegment(std::function<void()> func, const std::string& name);

	/// @brief インスタンスを作成する
	/// @return 
	CollisionInstanceSegment* CreateInstance();
};

