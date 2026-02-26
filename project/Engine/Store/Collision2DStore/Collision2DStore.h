#pragma once
#include "Collision2DData/Collision2DBaseData.h"

namespace Engine
{
	class Collision2DStore
	{
	public:

		/// @brief 衝突データを取得する
		/// @param hCollision 
		/// @return 
		Collision2DBaseData* GetCollisionData(Collision2DHandle hCollision) { return dataTable_[hCollision].get(); }

		/// @brief 更新処理
		void Update();

		/// @brief 読み込み
		/// @param func 
		/// @param name 
		/// @param type 
		/// @return 
		Collision2DHandle Load(std::function<void()> func, const std::string& name, Collision2D::Type type);

		/// @brief 全てのインスタンスを削除する
		void DestroyAllInstance() { for (auto& data : dataTable_)data->DestroyAllInstance(); }

		/// @brief インスタンスを作成する
		/// @tparam T 
		/// @param hCollision 
		/// @return 
		template<typename T>
		T* CreateInstance(Collision2DHandle hCollision)
		{
			return static_cast<T*>(dataTable_[hCollision]->CreateInstance());
		}

		/// @brief 衝突対象の設定
		/// @param hCollision 
		/// @param hTargetCollision 
		void SetCollision2DTarget(Collision2DHandle hCollision, Collision2DHandle hTargetCollision) { dataTable_[hCollision]->SetCollisionTarget(hTargetCollision); }


	private:

		// データテーブル
		std::vector<std::unique_ptr<Collision2DBaseData>> dataTable_;
	};
}