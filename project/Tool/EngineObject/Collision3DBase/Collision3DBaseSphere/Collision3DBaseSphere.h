#pragma once
#include "../BaseCollision3DBase.h"

/// @brief 3D衝突球判定インスタンス
class Collision3DInstanceSphere;

/// @brief 3D衝突球判定
class Collision3DBaseSphere : public Engine::BaseCollision3DBase
{
public:

	/// @brief コンストラクタ
	/// @param name 
	Collision3DBaseSphere(const std::string& name);

	/// @brief インスタンスを作成する
	/// @return 
	Collision3DInstanceSphere* CreateInstance();
};

