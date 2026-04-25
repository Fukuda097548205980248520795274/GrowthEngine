#pragma once
#include "GrowthEngine.h"

enum class ColliderType
{
	// ダメージ判定
	Hurtbox,

	// 攻撃判定
	Hitbox,

	// 押し出し判定
	Pushbox
};

class Entity;

class AppCollider
{
public:

	// コライダー
	Engine::BaseCollision3DInstance* collider_ = nullptr;

	/// @brief 保持者
	Entity* owner_ = nullptr;

	/// @brief 種類
	ColliderType type_;

	/// @brief 衝突したかどうか
	/// @return 
	bool IsHit()const { return collider_->isCollision_; }
};

