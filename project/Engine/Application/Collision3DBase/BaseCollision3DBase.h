#pragma once
#include "Data/CollisionData/CollisionData.h"
#include "Handle/Handle.h"
#include <functional>
#include <string>

namespace Engine
{
	class BaseCollision3DBase
	{
	public:

		/// @brief 仮想デストラクタ
		virtual ~BaseCollision3DBase() = default;

		/// @brief コンストラクタ
		/// @param name 
		BaseCollision3DBase(const std::string& name) : name_(name){}

		/// @brief ハンドルを取得する
		/// @return 
		Collision3DHandle GetHandle()const { return hCollision_; }

		/// @brief 衝突対象の設定
		/// @param hCollision 
		void SetCollisionTarget(Collision3DHandle hCollision)const;


	protected:

		/// @brief ハンドル
		Collision3DHandle hCollision_ = 0;

		/// @brief 名前
		std::string name_{};
	};
}