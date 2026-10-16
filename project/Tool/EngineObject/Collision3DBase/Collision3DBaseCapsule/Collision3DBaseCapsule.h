#pragma once
#include "../BaseCollision3DBase.h"

/// @brief 3D衝突カプセル判定インスタンス
class Collision3DInstanceCapsule;

/// @brief 3D衝突カプセル判定
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

