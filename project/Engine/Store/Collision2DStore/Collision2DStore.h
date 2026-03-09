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
		/// @param name 
		/// @param type 
		/// @return 
		Collision2DHandle Load(const std::string& name, Collision2D::Type type);

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

		/// @brief インスタンスを作成する
		/// @tparam T 
		/// @param name 
		/// @return 
		template<typename T>
		T* CreateInstance(const std::string& name)
		{
			return static_cast<T*>(dataTable_[nameTable_[name]]->CreateInstance());
		}

		/// @brief 衝突対象の設定
		/// @param hCollision 
		/// @param hTargetCollision 
		void SetCollision2DTarget(Collision2DHandle hCollision, Collision2DHandle hTargetCollision) { dataTable_[hCollision]->SetCollisionTarget(hTargetCollision); }

		/// @brief 衝突対象の設定
		/// @param name 
		/// @param targetName 
		void SetCollision2DTarget(const std::string& name, const std::string& targetName) { dataTable_[nameTable_[name]]->SetCollisionTarget(nameTable_[targetName]); }

		/// @brief デバッグ用描画処理
		void DebugDrawLine();


	private:

		// データテーブル
		std::vector<std::unique_ptr<Collision2DBaseData>> dataTable_;

		/// @brief 名前テーブル
		std::unordered_map<std::string, Collision2DHandle> nameTable_;
	};
}