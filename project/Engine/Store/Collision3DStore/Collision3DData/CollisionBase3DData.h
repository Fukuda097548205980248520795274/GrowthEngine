#pragma once
#include <functional>
#include <string>
#include <memory>
#include <list>
#include "Handle/Handle.h"
#include "Data/CollisionData/CollisionData.h"
#include "Application/CollisionInstance/BaseCollisionInstance.h"

namespace Engine
{
	class Collision3DStore;

	class CollisionBase3DData
	{
	public:

		/// @brief コンストラクタ
		/// @param func 
		CollisionBase3DData(std::function<void()> func, const std::string& name, Collision3D::Type type, Collision3DHandle hCollision)
			: func_(func), name_(name), type_(type), hCollision_(hCollision) {
		}

		/// @brief 仮想デストラクタ
		virtual ~CollisionBase3DData() = default;

		/// @brief 初期化
		/// @param collisionStore 
		void Initialize(Collision3DStore* collisionStore);

		/// @brief 更新処理
		void Update();

		/// @brief 衝突対象の設定
		/// @param handle 
		void SetCollisionTarget(Collision3DHandle handle) { collisionTable_.push_back(handle); }

		/// @brief ハンドルを取得する
		/// @return 
		Collision3DHandle GetHandle()const { return hCollision_; }

		/// @brief 種類を取得する
		/// @return 
		Collision3D::Type GetType()const { return type_; }

		/// @brief 名前を取得する
		/// @return 
		std::string GetaName()const { return name_; }

		/// @brief 衝突判定
		void CollisionCheck();

		/// @brief 全てのインスタンスを削除する
		void DestroyAllInstance() { instanceTable_.clear(); }

		/// @brief インスタンステーブルを取得する
		/// @return 
		std::list<std::unique_ptr<BaseCollisionInstance>>& GetInstanceTable() { return instanceTable_; }

		/// @brief インスタンスを作成する
		/// @return 
		virtual void* CreateInstance() = 0;

		/// @brief パラメータを取得する
		/// @return 
		virtual void* GetParam() = 0;


	private:

		/// @brief 衝突ストア
		Collision3DStore* collisionStore_ = nullptr;

		// ハンドル
		Collision3DHandle hCollision_ = 0;

		/// @brief 関数
		std::function<void()> func_ = nullptr;

		/// @brief 名前
		std::string name_{};

		/// @brief 種類
		Collision3D::Type type_;

	protected:

		// 衝突テーブル
		std::list<Collision3DHandle> collisionTable_;


	protected:

		// インスタンステーブル
		std::list<std::unique_ptr<BaseCollisionInstance>> instanceTable_;
	};
}