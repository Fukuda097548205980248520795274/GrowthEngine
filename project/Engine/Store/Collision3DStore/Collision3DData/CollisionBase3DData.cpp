#include "CollisionBase3DData.h"
#include "../Collision3DStore.h"

#include "CollisionAABBData/CollisionAABBData.h"

#include "Application/CollisionInstance/CollisionInstanceAABB/CollisionInstanceAABB.h"

#include "Func/CollisionFunc/CollisionFunc.h"
#include <cassert>

/// @brief 初期化
/// @param collisionStore 
void Engine::CollisionBase3DData::Initialize(Collision3DStore* collisionStore)
{
	// nullptrチェック
	assert(collisionStore);

	// 引数を受け取る
	collisionStore_ = collisionStore;
}

/// @brief 更新処理
void Engine::CollisionBase3DData::Update()
{
	instanceTable_.remove_if([](std::unique_ptr<BaseCollisionInstance>& instance) {if (instance->IsDelete()) { return true; }return false; });
}

/// @brief 衝突判定
void Engine::CollisionBase3DData::CollisionCheck()
{
	// 登録されているすべてのデータ
	for (auto& hCollision : collisionTable_)
	{
		// 衝突データ
		CollisionBase3DData* data = collisionStore_->GetCollisionData(hCollision);

		// 衝突対象のテーブル
		auto& instanceTable = data->GetInstanceTable();

		// インスタンス同士の当たり判定
		for (auto& myInstance : instanceTable_) for(auto& yourInstance : instanceTable)
		{
			
			// AABB AABB
			if (myInstance->GetType() == Collision::Type::AABB && yourInstance->GetType() == Collision::Type::AABB)
			{
				if (CollisionCheckFunc(*static_cast<Collision::AABB*>(myInstance->GetParam()), *static_cast<Collision::AABB*>(myInstance->GetParam())))
					func_();

				continue;
			}

		}
	}
}