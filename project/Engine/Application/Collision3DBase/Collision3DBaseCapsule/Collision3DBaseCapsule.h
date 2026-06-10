#pragma once
#include "../BaseCollision3DBase.h"

class Collision3DInstanceCapsule;

class Collision3DBaseCapsule : public Engine::BaseCollision3DBase
{
public:

	/// @brief コンストラクタ
	/// @param name 
	Collision3DBaseCapsule(const std::string& name);

	/// @brief インスタンスを作成する
	/// @return 
	Collision3DInstanceCapsule* CreateInstance();
};

