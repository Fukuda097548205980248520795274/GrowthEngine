#pragma once
#include <functional>
#include <string>
#include <memory>
#include <list>
#include "Handle/Handle.h"
#include "Data/CollisionData/CollisionData.h"
#include "Application/Collision2DInstance/BaseCollision2DInstance.h"

namespace Engine
{
	class Collision2DStore;

	class Collision2DBaseData
	{
	public:

		/// @brief コンストラクタ
		/// @param func 
		Collision2DBaseData(std::function<void()> func, const std::string& name, Collision2D::Type type, Collision2DHandle hCollision)
			: func_(func), name_(name), type_(type), hCollision_(hCollision) {
		}

		/// @brief 仮想デストラクタ
		virtual ~Collision2DBaseData() = default;

		/// @brief 初期化
		/// @param collisionStore 
		void Initialize(Collision2DStore* collisionStore);

		/// @brief 更新処理
		void Update();

		/// @brief 衝突対象の設定
		/// @param handle 
		void SetCollisionTarget(Collision2DHandle handle) { collisionTable_.push_back(handle); }

		/// @brief ハンドルを取得する
		/// @return 
		Collision2DHandle GetHandle()const { return hCollision_; }

		/// @brief 種類を取得する
		/// @return 
		Collision2D::Type GetType()const { return type_; }

		/// @brief 名前を取得する
		/// @return 
		std::string GetaName()const { return name_; }

		/// @brief 衝突判定
		void CollisionCheck();

		/// @brief 全てのインスタンスを削除する
		void DestroyAllInstance() { instanceTable_.clear(); }

		/// @brief インスタンステーブルを取得する
		/// @return 
		std::list<std::unique_ptr<BaseCollision2DInstance>>& GetInstanceTable() { return instanceTable_; }

		/// @brief インスタンスを作成する
		/// @return 
		virtual void* CreateInstance() = 0;

		/// @brief パラメータを取得する
		/// @return 
		virtual void* GetParam() = 0;


	private:

		/// @brief 衝突ストア
		Collision2DStore* collisionStore_ = nullptr;

		// ハンドル
		Collision2DHandle hCollision_ = 0;

		/// @brief 関数
		std::function<void()> func_ = nullptr;

		/// @brief 名前
		std::string name_{};

		/// @brief 種類
		Collision2D::Type type_;

	protected:

		// 衝突テーブル
		std::list<Collision2DHandle> collisionTable_;


	protected:

		// インスタンステーブル
		std::list<std::unique_ptr<BaseCollision2DInstance>> instanceTable_;
	};
}