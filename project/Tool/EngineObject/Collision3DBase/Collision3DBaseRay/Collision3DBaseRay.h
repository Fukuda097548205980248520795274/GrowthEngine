#pragma once
#include "../BaseCollision3DBase.h"

/// @brief 3D衝突半直線判定インスタンス
class Collision3DInstanceRay;

/// @brief 3D衝突半直線判定
class Collision3DBaseRay : public Engine::BaseCollision3DBase
{
public:

	/// @brief コンストラクタ
	/// @param name 
	Collision3DBaseRay(const std::string& name);

	/// @brief インスタンスを作成する
	/// @return 
	Collision3DInstanceRay* CreateInstance();
};

