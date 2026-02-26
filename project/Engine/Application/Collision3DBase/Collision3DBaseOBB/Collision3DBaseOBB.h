#pragma once
#include "../BaseCollision3DBase.h"

class Collision3DInstanceOBB;

class Collision3DBaseOBB : public Engine::BaseCollision3DBase
{
public:

	/// @brief コンストラクタ
	/// @param func 
	/// @param name 
	Collision3DBaseOBB(std::function<void()> func, const std::string& name);

	/// @brief インスタンスを作成する
	/// @return 
	Collision3DInstanceOBB* CreateInstance();
};

