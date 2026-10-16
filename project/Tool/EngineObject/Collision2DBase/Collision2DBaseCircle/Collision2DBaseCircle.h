#pragma once
#include "../BaseCollision2DBase.h"

/// @brief 2D衝突円判定インスタンス
class Collision2DInstanceCircle;

/// @brief 2D衝突円判定
class Collision2DBaseCircle : public Engine::BaseCollision2DBase
{
public:

	/// @brief コンストラクタ
	/// @param name 
	Collision2DBaseCircle(const std::string& name);

	/// @brief インスタンスを作成する
	/// @return 
	Collision2DInstanceCircle* CreateInstance();
};