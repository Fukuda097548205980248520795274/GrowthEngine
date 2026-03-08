#pragma once
#include "../BaseCollision3DBase.h"

class Collision3DInstanceSegment;

class Collision3DBaseSegment : public Engine::BaseCollision3DBase
{
public:

	/// @brief コンストラクタ
	/// @param name 
	Collision3DBaseSegment(const std::string& name);

	/// @brief インスタンスを作成する
	/// @return 
	Collision3DInstanceSegment* CreateInstance();
};

