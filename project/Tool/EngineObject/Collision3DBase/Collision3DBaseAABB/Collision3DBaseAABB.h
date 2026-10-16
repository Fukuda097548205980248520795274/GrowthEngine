#pragma once
#include "../BaseCollision3DBase.h"

/// @brief 3D衝突AABB判定
class Collision3DInstanceAABB;

/// @brief 3D衝突AABB判定
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

