#pragma once
#include "Data/CollisionData/CollisionData.h"
#include "Handle/Handle.h"
#include <functional>
#include <string>

namespace Engine
{
	class BaseCollision2DBase
	{
	public:

		/// @brief 仮想デストラクタ
		virtual ~BaseCollision2DBase() = default;

		/// @brief ハンドルを取得する
		/// @return 
		Collision2DHandle GetHandle()const { return hCollision_; }

		/// @brief 衝突対象の設定
		/// @param hCollision 
		void SetCollisionTarget(Collision2DHandle hCollision)const;


	protected:

		/// @brief ハンドル
		Collision2DHandle hCollision_ = 0;
	};
}