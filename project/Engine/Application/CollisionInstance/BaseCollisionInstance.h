#pragma once
#include "Data/CollisionData/CollisionData.h"
#include "Handle/Handle.h"

namespace Engine
{
	class BaseCollisionInstance
	{
	public:

		/// @brief 仮想デストラクタ
		virtual ~BaseCollisionInstance() = default;

		/// @brief 種類を取得する
		/// @return 
		Collision::Type GetType()const { return type_; }

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
		Collision::Type type_;

		/// @brief ハンドルを取得する
		Collision3DHandle handle_ = 0;

	private:

		/// @brief 削除フラグ
		bool isDelete_ = false;
	};
}