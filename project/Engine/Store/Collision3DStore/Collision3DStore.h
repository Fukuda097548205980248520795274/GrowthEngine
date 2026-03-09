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
		Collision3DHandle Load(const std::string& name, Collision3D::Type type);

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
		void SetCollision3DTarget(Collision3DHandle hCollision, Collision3DHandle hTargetCollision) { dataTable_[hCollision]->SetCollisionTarget(hTargetCollision); }

		/// @brief 衝突対象の設定
		/// @param name 
		/// @param targetName 
		void SetCollision3DTarget(const std::string& name, const std::string& targetName) { dataTable_[nameTable_[name]]->SetCollisionTarget(nameTable_[targetName]); }

		/// @brief デバッグ用の線を描画する
		void DebugDrawLine();


	private:

		// データテーブル
		std::vector<std::unique_ptr<Collision3DBaseData>> dataTable_;

		/// @brief 名前テーブル
		std::unordered_map<std::string, Collision3DHandle> nameTable_;
	};
}