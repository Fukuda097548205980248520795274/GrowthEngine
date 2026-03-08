#pragma once
#include "../BaseCollision3DBase.h"

class Collision3DInstancePlane;

class Collision3DBasePlane : public Engine::BaseCollision3DBase
{
public:

	/// @brief コンストラクタ
	/// @param name 
	Collision3DBasePlane(const std::string& name);

	/// @brief インスタンスを作成する
	/// @return 
	Collision3DInstancePlane* CreateInstance();
};

