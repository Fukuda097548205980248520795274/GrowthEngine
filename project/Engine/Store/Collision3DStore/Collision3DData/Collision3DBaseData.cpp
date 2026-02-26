#include "Collision3DBaseData.h"
#include "../Collision3DStore.h"

#include "Func/CollisionFunc/CollisionFunc.h"
#include <cassert>

/// @brief 初期化
/// @param collisionStore 
void Engine::Collision3DBaseData::Initialize(Collision3DStore* collisionStore)
{
	// nullptrチェック
	assert(collisionStore);

	// 引数を受け取る
	collisionStore_ = collisionStore;
}

/// @brief 更新処理
void Engine::Collision3DBaseData::Update()
{
	instanceTable_.remove_if([](std::unique_ptr<BaseCollision3DInstance>& instance) {if (instance->IsDelete()) { return true; }return false; });
}

/// @brief 衝突判定
void Engine::Collision3DBaseData::CollisionCheck()
{
	// 登録されているすべてのデータ
	for (auto& hCollision : collisionTable_)
	{
		// 衝突データ
		Collision3DBaseData* data = collisionStore_->GetCollisionData(hCollision);

		// 衝突対象のテーブル
		auto& instanceTable = data->GetInstanceTable();

		// インスタンス同士の当たり判定
		for (auto& myInstance : instanceTable_) for(auto& yourInstance : instanceTable)
		{
			// 球 球
			if (myInstance->GetType() == Collision3D::Type::Sphere && yourInstance->GetType() == Collision3D::Type::Sphere)
			{
				if (CollisionCheckFunc(*static_cast<Collision3D::Sphere*>(myInstance->GetParam()), *static_cast<Collision3D::Sphere*>(yourInstance->GetParam())))
					func_();

				continue;
			}
			
			// AABB AABB
			if (myInstance->GetType() == Collision3D::Type::AABB && yourInstance->GetType() == Collision3D::Type::AABB)
			{
				if (CollisionCheckFunc(*static_cast<Collision3D::AABB*>(myInstance->GetParam()), *static_cast<Collision3D::AABB*>(yourInstance->GetParam())))
					func_();

				continue;
			}

			// OBB OBB
			if (myInstance->GetType() == Collision3D::Type::OBB && yourInstance->GetType() == Collision3D::Type::OBB)
			{
				if (CollisionCheckFunc(*static_cast<Collision3D::OBB*>(myInstance->GetParam()), *static_cast<Collision3D::OBB*>(yourInstance->GetParam())))
					func_();

				continue;
			}

			// 球 AABB
			if (myInstance->GetType() == Collision3D::Type::Sphere && yourInstance->GetType() == Collision3D::Type::AABB ||
				myInstance->GetType() == Collision3D::Type::AABB && yourInstance->GetType() == Collision3D::Type::Sphere)
			{
				if (CollisionCheckFunc(*static_cast<Collision3D::Sphere*>(myInstance->GetParam()), *static_cast<Collision3D::AABB*>(yourInstance->GetParam())))
					func_();

				continue;
			}

			// 球 OBB
			if (myInstance->GetType() == Collision3D::Type::Sphere && yourInstance->GetType() == Collision3D::Type::OBB ||
				myInstance->GetType() == Collision3D::Type::OBB && yourInstance->GetType() == Collision3D::Type::Sphere)
			{
				if (CollisionCheckFunc(*static_cast<Collision3D::Sphere*>(myInstance->GetParam()), *static_cast<Collision3D::OBB*>(yourInstance->GetParam())))
					func_();

				continue;
			}

			// AABB OBB
			if (myInstance->GetType() == Collision3D::Type::AABB && yourInstance->GetType() == Collision3D::Type::OBB ||
				myInstance->GetType() == Collision3D::Type::OBB && yourInstance->GetType() == Collision3D::Type::AABB)
			{
				if (CollisionCheckFunc(*static_cast<Collision3D::AABB*>(myInstance->GetParam()), *static_cast<Collision3D::OBB*>(yourInstance->GetParam())))
					func_();

				continue;
			}

			// 球 平面
			if (myInstance->GetType() == Collision3D::Type::Sphere && yourInstance->GetType() == Collision3D::Type::Plane ||
				myInstance->GetType() == Collision3D::Type::Plane && yourInstance->GetType() == Collision3D::Type::Sphere)
			{
				if (CollisionCheckFunc(*static_cast<Collision3D::Plane*>(myInstance->GetParam()), *static_cast<Collision3D::Sphere*>(yourInstance->GetParam())))
					func_();

				continue;
			}

			// 平面 直線
			if (myInstance->GetType() == Collision3D::Type::Plane && yourInstance->GetType() == Collision3D::Type::Line ||
				myInstance->GetType() == Collision3D::Type::Line && yourInstance->GetType() == Collision3D::Type::Plane)
			{
				if (CollisionCheckFunc(*static_cast<Collision3D::Plane*>(myInstance->GetParam()), *static_cast<Collision3D::Line*>(yourInstance->GetParam())))
					func_();

				continue;
			}

			// 平面 半直線
			if (myInstance->GetType() == Collision3D::Type::Plane && yourInstance->GetType() == Collision3D::Type::Ray ||
				myInstance->GetType() == Collision3D::Type::Ray && yourInstance->GetType() == Collision3D::Type::Plane)
			{
				if (CollisionCheckFunc(*static_cast<Collision3D::Plane*>(myInstance->GetParam()), *static_cast<Collision3D::Ray*>(yourInstance->GetParam())))
					func_();

				continue;
			}

			// 平面 線分
			if (myInstance->GetType() == Collision3D::Type::Plane && yourInstance->GetType() == Collision3D::Type::Segment ||
				myInstance->GetType() == Collision3D::Type::Segment && yourInstance->GetType() == Collision3D::Type::Plane)
			{
				if (CollisionCheckFunc(*static_cast<Collision3D::Plane*>(myInstance->GetParam()), *static_cast<Collision3D::Segment*>(yourInstance->GetParam())))
					func_();

				continue;
			}

			// AABB 直線
			if (myInstance->GetType() == Collision3D::Type::AABB && yourInstance->GetType() == Collision3D::Type::Line ||
				myInstance->GetType() == Collision3D::Type::Line && yourInstance->GetType() == Collision3D::Type::AABB)
			{
				if (CollisionCheckFunc(*static_cast<Collision3D::AABB*>(myInstance->GetParam()), *static_cast<Collision3D::Line*>(yourInstance->GetParam())))
					func_();

				continue;
			}

			// AABB 半直線
			if (myInstance->GetType() == Collision3D::Type::AABB && yourInstance->GetType() == Collision3D::Type::Ray ||
				myInstance->GetType() == Collision3D::Type::Ray && yourInstance->GetType() == Collision3D::Type::AABB)
			{
				if (CollisionCheckFunc(*static_cast<Collision3D::AABB*>(myInstance->GetParam()), *static_cast<Collision3D::Ray*>(yourInstance->GetParam())))
					func_();

				continue;
			}

			// AABB 線分
			if (myInstance->GetType() == Collision3D::Type::AABB && yourInstance->GetType() == Collision3D::Type::Segment ||
				myInstance->GetType() == Collision3D::Type::Segment && yourInstance->GetType() == Collision3D::Type::AABB)
			{
				if (CollisionCheckFunc(*static_cast<Collision3D::AABB*>(myInstance->GetParam()), *static_cast<Collision3D::Segment*>(yourInstance->GetParam())))
					func_();

				continue;
			}

			// OBB 直線
			if (myInstance->GetType() == Collision3D::Type::OBB && yourInstance->GetType() == Collision3D::Type::Line ||
				myInstance->GetType() == Collision3D::Type::Line && yourInstance->GetType() == Collision3D::Type::OBB)
			{
				if (CollisionCheckFunc(*static_cast<Collision3D::OBB*>(myInstance->GetParam()), *static_cast<Collision3D::Line*>(yourInstance->GetParam())))
					func_();

				continue;
			}

			// OBB 半直線
			if (myInstance->GetType() == Collision3D::Type::OBB && yourInstance->GetType() == Collision3D::Type::Ray ||
				myInstance->GetType() == Collision3D::Type::Ray && yourInstance->GetType() == Collision3D::Type::OBB)
			{
				if (CollisionCheckFunc(*static_cast<Collision3D::OBB*>(myInstance->GetParam()), *static_cast<Collision3D::Ray*>(yourInstance->GetParam())))
					func_();

				continue;
			}

			// OBB 線分
			if (myInstance->GetType() == Collision3D::Type::OBB && yourInstance->GetType() == Collision3D::Type::Segment ||
				myInstance->GetType() == Collision3D::Type::Segment && yourInstance->GetType() == Collision3D::Type::OBB)
			{
				if (CollisionCheckFunc(*static_cast<Collision3D::OBB*>(myInstance->GetParam()), *static_cast<Collision3D::Segment*>(yourInstance->GetParam())))
					func_();

				continue;
			}
		}
	}
}