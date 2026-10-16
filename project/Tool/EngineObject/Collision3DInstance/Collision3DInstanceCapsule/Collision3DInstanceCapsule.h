#pragma once
#include "../BaseCollision3DInstance.h"

/// @brief 3D衝突カプセル判定インスタンス
class Collision3DInstanceCapsule : public Engine::BaseCollision3DInstance
{
public:

	/// @brief コンストラクタ
	/// @param param 
	Collision3DInstanceCapsule(Engine::Collision3D::Capsule* param);

	/// @brief パラメータ
	std::unique_ptr<Engine::Collision3D::Capsule> param_ = nullptr;
};

