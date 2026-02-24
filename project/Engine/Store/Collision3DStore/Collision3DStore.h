#pragma once
#include "Collision3DData/CollisionBase3DData.h"

namespace Engine
{
	class Collision3DStore
	{
	public:

		/// @brief 衝突データを取得する
		/// @param hCollision 
		/// @return 
		CollisionBase3DData* GetCollisionData(Collision3DHandle hCollision) { return dataTable_[hCollision].get(); }

		/// @brief 更新処理
		void Update();

		/// @brief 読み込み
		/// @param func 
		/// @param name 
		/// @param type 
		/// @return 
		Collision3DHandle Load(std::function<void()> func, const std::string& name, Collision::Type type);

		/// @brief 全てのインスタンスを削除する
		void DestroyAllInstance() { for (auto& data : dataTable_)data->DestroyAllInstance(); }

		/// @brief インスタンスを作成する
		/// @tparam T 
		/// @param hCollision 
		/// @return 
		template<typename T>
		T* CreateInstance(Collision3DHandle hCollision)
		{
			return static_cast<T*>(dataTable_[hCollision]->CreateInstance());
		}

		/// @brief 衝突対象の設定
		/// @param hCollision 
		/// @param hTargetCollision 
		void SetCollisionTarget(Collision3DHandle hCollision, Collision3DHandle hTargetCollision) { dataTable_[hCollision]->SetCollisionTarget(hTargetCollision); }


	private:

		// データテーブル
		std::vector<std::unique_ptr<CollisionBase3DData>> dataTable_;
	};
}