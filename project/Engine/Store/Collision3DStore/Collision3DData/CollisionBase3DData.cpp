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
			// 球 球
			if (myInstance->GetType() == Collision::Type::Sphere && yourInstance->GetType() == Collision::Type::Sphere)
			{
				if (CollisionCheckFunc(*static_cast<Collision::Sphere*>(myInstance->GetParam()), *static_cast<Collision::Sphere*>(yourInstance->GetParam())))
					func_();

				continue;
			}
			
			// AABB AABB
			if (myInstance->GetType() == Collision::Type::AABB && yourInstance->GetType() == Collision::Type::AABB)
			{
				if (CollisionCheckFunc(*static_cast<Collision::AABB*>(myInstance->GetParam()), *static_cast<Collision::AABB*>(yourInstance->GetParam())))
					func_();

				continue;
			}

			// OBB OBB
			if (myInstance->GetType() == Collision::Type::OBB && yourInstance->GetType() == Collision::Type::OBB)
			{
				if (CollisionCheckFunc(*static_cast<Collision::OBB*>(myInstance->GetParam()), *static_cast<Collision::OBB*>(yourInstance->GetParam())))
					func_();

				continue;
			}

			// 球 AABB
			if (myInstance->GetType() == Collision::Type::Sphere && yourInstance->GetType() == Collision::Type::AABB ||
				myInstance->GetType() == Collision::Type::AABB && yourInstance->GetType() == Collision::Type::Sphere)
			{
				if (CollisionCheckFunc(*static_cast<Collision::Sphere*>(myInstance->GetParam()), *static_cast<Collision::AABB*>(yourInstance->GetParam())))
					func_();

				continue;
			}

			// 球 OBB
			if (myInstance->GetType() == Collision::Type::Sphere && yourInstance->GetType() == Collision::Type::OBB ||
				myInstance->GetType() == Collision::Type::OBB && yourInstance->GetType() == Collision::Type::Sphere)
			{
				if (CollisionCheckFunc(*static_cast<Collision::Sphere*>(myInstance->GetParam()), *static_cast<Collision::OBB*>(yourInstance->GetParam())))
					func_();

				continue;
			}

			// AABB OBB
			if (myInstance->GetType() == Collision::Type::AABB && yourInstance->GetType() == Collision::Type::OBB ||
				myInstance->GetType() == Collision::Type::OBB && yourInstance->GetType() == Collision::Type::AABB)
			{
				if (CollisionCheckFunc(*static_cast<Collision::AABB*>(myInstance->GetParam()), *static_cast<Collision::OBB*>(yourInstance->GetParam())))
					func_();

				continue;
			}

			// 球 平面
			if (myInstance->GetType() == Collision::Type::Sphere && yourInstance->GetType() == Collision::Type::Plane ||
				myInstance->GetType() == Collision::Type::Plane && yourInstance->GetType() == Collision::Type::Sphere)
			{
				if (CollisionCheckFunc(*static_cast<Collision::Plane*>(myInstance->GetParam()), *static_cast<Collision::Sphere*>(yourInstance->GetParam())))
					func_();

				continue;
			}

			// 平面 直線
			if (myInstance->GetType() == Collision::Type::Plane && yourInstance->GetType() == Collision::Type::Line ||
				myInstance->GetType() == Collision::Type::Line && yourInstance->GetType() == Collision::Type::Plane)
			{
				if (CollisionCheckFunc(*static_cast<Collision::Plane*>(myInstance->GetParam()), *static_cast<Collision::Line*>(yourInstance->GetParam())))
					func_();

				continue;
			}

			// 平面 半直線
			if (myInstance->GetType() == Collision::Type::Plane && yourInstance->GetType() == Collision::Type::Ray ||
				myInstance->GetType() == Collision::Type::Ray && yourInstance->GetType() == Collision::Type::Plane)
			{
				if (CollisionCheckFunc(*static_cast<Collision::Plane*>(myInstance->GetParam()), *static_cast<Collision::Ray*>(yourInstance->GetParam())))
					func_();

				continue;
			}

			// 平面 線分
			if (myInstance->GetType() == Collision::Type::Plane && yourInstance->GetType() == Collision::Type::Segment ||
				myInstance->GetType() == Collision::Type::Segment && yourInstance->GetType() == Collision::Type::Plane)
			{
				if (CollisionCheckFunc(*static_cast<Collision::Plane*>(myInstance->GetParam()), *static_cast<Collision::Segment*>(yourInstance->GetParam())))
					func_();

				continue;
			}

			// AABB 直線
			if (myInstance->GetType() == Collision::Type::AABB && yourInstance->GetType() == Collision::Type::Line ||
				myInstance->GetType() == Collision::Type::Line && yourInstance->GetType() == Collision::Type::AABB)
			{
				if (CollisionCheckFunc(*static_cast<Collision::AABB*>(myInstance->GetParam()), *static_cast<Collision::Line*>(yourInstance->GetParam())))
					func_();

				continue;
			}

			// AABB 半直線
			if (myInstance->GetType() == Collision::Type::AABB && yourInstance->GetType() == Collision::Type::Ray ||
				myInstance->GetType() == Collision::Type::Ray && yourInstance->GetType() == Collision::Type::AABB)
			{
				if (CollisionCheckFunc(*static_cast<Collision::AABB*>(myInstance->GetParam()), *static_cast<Collision::Ray*>(yourInstance->GetParam())))
					func_();

				continue;
			}

			// AABB 線分
			if (myInstance->GetType() == Collision::Type::AABB && yourInstance->GetType() == Collision::Type::Segment ||
				myInstance->GetType() == Collision::Type::Segment && yourInstance->GetType() == Collision::Type::AABB)
			{
				if (CollisionCheckFunc(*static_cast<Collision::AABB*>(myInstance->GetParam()), *static_cast<Collision::Segment*>(yourInstance->GetParam())))
					func_();

				continue;
			}

			// OBB 直線
			if (myInstance->GetType() == Collision::Type::OBB && yourInstance->GetType() == Collision::Type::Line ||
				myInstance->GetType() == Collision::Type::Line && yourInstance->GetType() == Collision::Type::OBB)
			{
				if (CollisionCheckFunc(*static_cast<Collision::OBB*>(myInstance->GetParam()), *static_cast<Collision::Line*>(yourInstance->GetParam())))
					func_();

				continue;
			}

			// OBB 半直線
			if (myInstance->GetType() == Collision::Type::OBB && yourInstance->GetType() == Collision::Type::Ray ||
				myInstance->GetType() == Collision::Type::Ray && yourInstance->GetType() == Collision::Type::OBB)
			{
				if (CollisionCheckFunc(*static_cast<Collision::OBB*>(myInstance->GetParam()), *static_cast<Collision::Ray*>(yourInstance->GetParam())))
					func_();

				continue;
			}

			// OBB 線分
			if (myInstance->GetType() == Collision::Type::OBB && yourInstance->GetType() == Collision::Type::Segment ||
				myInstance->GetType() == Collision::Type::Segment && yourInstance->GetType() == Collision::Type::OBB)
			{
				if (CollisionCheckFunc(*static_cast<Collision::OBB*>(myInstance->GetParam()), *static_cast<Collision::Segment*>(yourInstance->GetParam())))
					func_();

				continue;
			}
		}
	}
}