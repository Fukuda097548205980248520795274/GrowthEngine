#pragma once
#include "Data/CollisionData/CollisionData.h"
#include "Handle/Handle.h"

namespace Engine
{
	class BaseCollision2DInstance
	{
	public:

		/// @brief 仮想デストラクタ
		virtual ~BaseCollision2DInstance() = default;

		/// @brief 種類を取得する
		/// @return 
		Collision2D::Type GetType()const { return type_; }

		/// @brief 削除したかどうか
		/// @return 
		bool IsDelete()const { return isDelete_; }

		/// @brief 削除
		void Delete() { isDelete_ = true; }

		/// @brief パラメータを取得する
		/// @return 
		virtual void* GetParam() = 0;

	protected:

		/// @brief 種類
		Collision2D::Type type_;

	private:

		/// @brief 削除フラグ
		bool isDelete_ = false;
	};
}