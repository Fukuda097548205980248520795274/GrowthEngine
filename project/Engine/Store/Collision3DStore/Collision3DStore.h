#pragma once
#include "Collision3DData/Collision3DBaseData.h"

namespace Engine
{
	class Collision3DStore
	{
	public:

		/// @brief 衝突データを取得する
		/// @param hCollision 
		/// @return 
		Collision3DBaseData* GetCollisionData(Collision3DHandle hCollision) { return dataTable_[hCollision].get(); }

		/// @brief 更新処理
		void Update();

		/// @brief 読み込み
		/// @param func 
		/// @param name 
		/// @param type 
		/// @return 
		Collision3DHandle Load(std::function<void()> func, const std::string& name, Collision3D::Type type);

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
		std::vector<std::unique_ptr<Collision3DBaseData>> dataTable_;
	};
}