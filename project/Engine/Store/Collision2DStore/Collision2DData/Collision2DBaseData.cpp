#include "Collision2DBaseData.h"
#include "../Collision2DStore.h"

#include "Func/CollisionFunc/CollisionFunc.h"
#include <cassert>

#include "Application/Collision2DInstance/Collision2DInstanceCircle/Collision2DInstanceCircle.h"
#include "Application/Collision2DInstance/Collision2DInstanceSprite/Collision2DInstanceSprite.h"

/// @brief 初期化
/// @param collisionStore 
void Engine::Collision2DBaseData::Initialize(Collision2DStore* collisionStore)
{
	// nullptrチェック
	assert(collisionStore);

	// 引数を受け取る
	collisionStore_ = collisionStore;
}

/// @brief 更新処理
void Engine::Collision2DBaseData::Update()
{
	instanceTable_.remove_if([](std::unique_ptr<BaseCollision2DInstance>& instance) {if (instance->IsDelete()) { return true; }return false; });
}

/// @brief 衝突判定
void Engine::Collision2DBaseData::CollisionCheck()
{
	// 登録されているすべてのデータ
	for (auto& hCollision : collisionTable_)
	{
		// 衝突データ
		Collision2DBaseData* data = collisionStore_->GetCollisionData(hCollision);

		// 衝突対象のテーブル
		auto& instanceTable = data->GetInstanceTable();

		// インスタンス同士の当たり判定
		for (auto& myInstance : instanceTable_) for (auto& yourInstance : instanceTable)
		{
			// 円 円
			if (myInstance->GetType() == Collision2D::Type::Circle && yourInstance->GetType() == Collision2D::Type::Circle)
			{
				if (CollisionCheckFunc(*static_cast<Collision2DInstanceCircle*>(myInstance.get())->param_, *static_cast<Collision2DInstanceCircle*>(yourInstance.get())->param_))
					func_();

				continue;
			}

			// 矩形 矩形
			if (myInstance->GetType() == Collision2D::Type::Sprite && yourInstance->GetType() == Collision2D::Type::Sprite)
			{
				if (CollisionCheckFunc(*static_cast<Collision2DInstanceSprite*>(myInstance.get())->param_, *static_cast<Collision2DInstanceSprite*>(yourInstance.get())->param_))
					func_();

				continue;
			}

			// 円 矩形
			if (myInstance->GetType() == Collision2D::Type::Circle && yourInstance->GetType() == Collision2D::Type::Sprite)
			{
				if (CollisionCheckFunc(*static_cast<Collision2DInstanceCircle*>(myInstance.get())->param_, *static_cast<Collision2DInstanceSprite*>(yourInstance.get())->param_))
					func_();

				continue;
			}

			// 矩形 円
			if (myInstance->GetType() == Collision2D::Type::Sprite && yourInstance->GetType() == Collision2D::Type::Circle)
			{
				if (CollisionCheckFunc(*static_cast<Collision2DInstanceCircle*>(yourInstance.get())->param_, *static_cast<Collision2DInstanceSprite*>(myInstance.get())->param_))
					func_();

				continue;
			}
		}
	}
}