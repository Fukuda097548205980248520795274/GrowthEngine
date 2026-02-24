#pragma once
#include "../BaseCollisionInstance.h"

namespace Engine
{
	class CollisionInstanceAABB : public Engine::BaseCollisionInstance
	{
	public:

		/// @brief コンストラクタ
		/// @param param 
		CollisionInstanceAABB(Collision::AABB* param) : param_(param) { type_ = Collision::Type::AABB; }

		/// @brief パラメータを取得する
		/// @return 
		void* GetParam() override { return param_; }

		/// @brief パラメータ
		Collision::AABB* param_ = nullptr;
	};
}