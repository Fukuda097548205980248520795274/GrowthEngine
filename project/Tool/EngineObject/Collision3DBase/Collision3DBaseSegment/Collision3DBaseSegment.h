#pragma once
#include "../BaseCollision3DBase.h"

/// @brief 3D衝突線分判定インスタンス
class Collision3DInstanceSegment;

/// @brief 3D衝突線分判定
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

