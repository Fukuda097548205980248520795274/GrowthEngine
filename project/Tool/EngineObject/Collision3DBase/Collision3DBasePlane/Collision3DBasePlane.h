#pragma once
#include "../BaseCollision3DBase.h"

/// @brief 3D衝突平面判定インスタンス
class Collision3DInstancePlane;

/// @brief 3D衝突平面判定
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

