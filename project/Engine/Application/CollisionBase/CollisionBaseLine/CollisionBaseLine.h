#pragma once
#include "../BaseCollisionBase.h"

class CollisionInstanceLine;

class CollisionBaseLine : public Engine::BaseCollisionBase
{
public:

	/// @brief コンストラクタ
	/// @param func 
	/// @param name 
	CollisionBaseLine(std::function<void()> func, const std::string& name);

	/// @brief インスタンスを作成する
	/// @return 
	CollisionInstanceLine* CreateInstance();
};

