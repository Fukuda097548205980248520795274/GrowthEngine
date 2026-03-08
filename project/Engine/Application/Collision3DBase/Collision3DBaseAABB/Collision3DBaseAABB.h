#pragma once
#include "../BaseCollision3DBase.h"

class Collision3DInstanceAABB;

class Collision3DBaseAABB : public Engine::BaseCollision3DBase
{
public:

	/// @brief コンストラクタ
	/// @param name 
	Collision3DBaseAABB(const std::string& name);

	/// @brief インスタンスを作成する
	/// @return 
	Collision3DInstanceAABB* CreateInstance();
};

