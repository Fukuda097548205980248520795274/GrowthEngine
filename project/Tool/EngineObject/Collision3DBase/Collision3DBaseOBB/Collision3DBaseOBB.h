#pragma once
#include "../BaseCollision3DBase.h"

/// @brief 3D衝突OBB判定インスタンス
class Collision3DInstanceOBB;

/// @brief 3D衝突OBB判定
class Collision3DBaseOBB : public Engine::BaseCollision3DBase
{
public:

	/// @brief コンストラクタ
	/// @param name 
	Collision3DBaseOBB(const std::string& name);

	/// @brief インスタンスを作成する
	/// @return 
	Collision3DInstanceOBB* CreateInstance();
};

