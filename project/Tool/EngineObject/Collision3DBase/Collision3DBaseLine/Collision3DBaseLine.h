#pragma once
#include "../BaseCollision3DBase.h"

/// @brief 3D衝突直線判定インスタンス
class Collision3DInstanceLine;

/// @brief 3D衝突直線判定
class Collision3DBaseLine : public Engine::BaseCollision3DBase
{
public:

	/// @brief コンストラクタ
	/// @param name 
	Collision3DBaseLine(const std::string& name);

	/// @brief インスタンスを作成する
	/// @return 
	Collision3DInstanceLine* CreateInstance();
};

