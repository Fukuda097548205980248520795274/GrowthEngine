#pragma once
#include "Data/CollisionData/CollisionData.h"
#include "Handle/Handle.h"
#include <memory>

namespace Engine
{
	class BaseCollision3DInstance
	{
	public:

		/// @brief 仮想デストラクタ
		virtual ~BaseCollision3DInstance() = default;

		/// @brief 種類を取得する
		/// @return 
		Collision3D::Type GetType()const { return type_; }

		/// @brief 削除したかどうか
		/// @return 
		bool IsDelete()const { return isDelete_; }

		/// @brief 削除
		void Delete() { isDelete_ = true; }

	protected:

		/// @brief 種類
		Collision3D::Type type_;

	private:

		/// @brief 削除フラグ
		bool isDelete_ = false;
	};
}