#include "Collision3DBaseData.h"
#include "../Collision3DStore.h"

#include "Func/CollisionFunc/CollisionFunc.h"
#include <cassert>

#include "Application/Collision3DInstance/Collision3DInstanceSphere/Collision3DInstanceSphere.h"
#include "Application/Collision3DInstance/Collision3DInstanceAABB/Collision3DInstanceAABB.h"
#include "Application/Collision3DInstance/Collision3DInstanceOBB/Collision3DInstanceOBB.h"
#include "Application/Collision3DInstance/Collision3DInstancePlane/Collision3DInstancePlane.h"
#include "Application/Collision3DInstance/Collision3DInstanceLine/Collision3DInstanceLine.h"
#include "Application/Collision3DInstance/Collision3DInstanceRay/Collision3DInstanceRay.h"
#include "Application/Collision3DInstance/Collision3DInstanceSegment/Collision3DInstanceSegment.h"

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
				if (CollisionCheckFunc(*static_cast<Collision3DInstanceSphere*>(myInstance.get())->param_, *static_cast<Collision3DInstanceSphere*>(yourInstance.get())->param_))
					func_();

				continue;
			}
			
			// AABB AABB
			if (myInstance->GetType() == Collision3D::Type::AABB && yourInstance->GetType() == Collision3D::Type::AABB)
			{
				if (CollisionCheckFunc(*static_cast<Collision3DInstanceAABB*>(myInstance.get())->param_, *static_cast<Collision3DInstanceAABB*>(yourInstance.get())->param_))
					func_();

				continue;
			}

			// OBB OBB
			if (myInstance->GetType() == Collision3D::Type::OBB && yourInstance->GetType() == Collision3D::Type::OBB)
			{
				if (CollisionCheckFunc(*static_cast<Collision3DInstanceOBB*>(myInstance.get())->param_, *static_cast<Collision3DInstanceOBB*>(yourInstance.get())->param_))
					func_();

				continue;
			}

			// 球 AABB
			if (myInstance->GetType() == Collision3D::Type::Sphere && yourInstance->GetType() == Collision3D::Type::AABB)
			{
				if (CollisionCheckFunc(*static_cast<Collision3DInstanceSphere*>(myInstance.get())->param_, *static_cast<Collision3DInstanceAABB*>(yourInstance.get())->param_))
					func_();

				continue;
			}

			// AABB 球
			if (myInstance->GetType() == Collision3D::Type::AABB && yourInstance->GetType() == Collision3D::Type::Sphere)
			{
				if (CollisionCheckFunc(*static_cast<Collision3DInstanceSphere*>(yourInstance.get())->param_, *static_cast<Collision3DInstanceAABB*>(myInstance.get())->param_))
					func_();

				continue;
			}

			// 球 OBB
			if (myInstance->GetType() == Collision3D::Type::Sphere && yourInstance->GetType() == Collision3D::Type::OBB)
			{
				if (CollisionCheckFunc(*static_cast<Collision3DInstanceSphere*>(myInstance.get())->param_, *static_cast<Collision3DInstanceOBB*>(yourInstance.get())->param_))
					func_();

				continue;
			}

			// OBB 球
			if (myInstance->GetType() == Collision3D::Type::OBB && yourInstance->GetType() == Collision3D::Type::Sphere)
			{
				if (CollisionCheckFunc(*static_cast<Collision3DInstanceSphere*>(yourInstance.get())->param_, *static_cast<Collision3DInstanceOBB*>(myInstance.get())->param_))
					func_();

				continue;
			}

			// AABB OBB
			if (myInstance->GetType() == Collision3D::Type::AABB && yourInstance->GetType() == Collision3D::Type::OBB)
			{
				if (CollisionCheckFunc(*static_cast<Collision3DInstanceAABB*>(myInstance.get())->param_, *static_cast<Collision3DInstanceOBB*>(yourInstance.get())->param_))
					func_();

				continue;
			}

			// OBB AABB
			if (myInstance->GetType() == Collision3D::Type::OBB && yourInstance->GetType() == Collision3D::Type::AABB)
			{
				if (CollisionCheckFunc(*static_cast<Collision3DInstanceAABB*>(yourInstance.get())->param_, *static_cast<Collision3DInstanceOBB*>(myInstance.get())->param_))
					func_();

				continue;
			}

			// 平面　球
			if (myInstance->GetType() == Collision3D::Type::Plane && yourInstance->GetType() == Collision3D::Type::Sphere)
			{
				if (CollisionCheckFunc(*static_cast<Collision3DInstancePlane*>(myInstance.get())->param_, *static_cast<Collision3DInstanceSphere*>(yourInstance.get())->param_))
					func_();

				continue;
			}

			// 球 平面
			if (myInstance->GetType() == Collision3D::Type::Sphere && yourInstance->GetType() == Collision3D::Type::Plane)
			{
				if (CollisionCheckFunc(*static_cast<Collision3DInstancePlane*>(yourInstance.get())->param_, *static_cast<Collision3DInstanceSphere*>(myInstance.get())->param_))
					func_();

				continue;
			}

			// 平面 直線
			if (myInstance->GetType() == Collision3D::Type::Plane && yourInstance->GetType() == Collision3D::Type::Line)
			{
				if (CollisionCheckFunc(*static_cast<Collision3DInstancePlane*>(myInstance.get())->param_, *static_cast<Collision3DInstanceLine*>(yourInstance.get())->param_))
					func_();

				continue;
			}

			// 直線 平面
			if (myInstance->GetType() == Collision3D::Type::Line && yourInstance->GetType() == Collision3D::Type::Plane)
			{
				if (CollisionCheckFunc(*static_cast<Collision3DInstancePlane*>(yourInstance.get())->param_, *static_cast<Collision3DInstanceLine*>(myInstance.get())->param_))
					func_();

				continue;
			}

			// 平面 半直線
			if (myInstance->GetType() == Collision3D::Type::Plane && yourInstance->GetType() == Collision3D::Type::Ray)
			{
				if (CollisionCheckFunc(*static_cast<Collision3DInstancePlane*>(myInstance.get())->param_, *static_cast<Collision3DInstanceRay*>(yourInstance.get())->param_))
					func_();

				continue;
			}

			// 半直線 平面
			if (myInstance->GetType() == Collision3D::Type::Ray && yourInstance->GetType() == Collision3D::Type::Plane)
			{
				if (CollisionCheckFunc(*static_cast<Collision3DInstancePlane*>(yourInstance.get())->param_, *static_cast<Collision3DInstanceRay*>(myInstance.get())->param_))
					func_();

				continue;
			}

			// 平面 線分
			if (myInstance->GetType() == Collision3D::Type::Plane && yourInstance->GetType() == Collision3D::Type::Segment)
			{
				if (CollisionCheckFunc(*static_cast<Collision3DInstancePlane*>(myInstance.get())->param_, *static_cast<Collision3DInstanceSegment*>(yourInstance.get())->param_))
					func_();

				continue;
			}

			// 線分 平面
			if (myInstance->GetType() == Collision3D::Type::Segment && yourInstance->GetType() == Collision3D::Type::Plane)
			{
				if (CollisionCheckFunc(*static_cast<Collision3DInstancePlane*>(yourInstance.get())->param_, *static_cast<Collision3DInstanceSegment*>(myInstance.get())->param_))
					func_();

				continue;
			}

			// AABB 直線
			if (myInstance->GetType() == Collision3D::Type::AABB && yourInstance->GetType() == Collision3D::Type::Line)
			{
				if (CollisionCheckFunc(*static_cast<Collision3DInstanceAABB*>(myInstance.get())->param_, *static_cast<Collision3DInstanceLine*>(yourInstance.get())->param_))
					func_();

				continue;
			}

			// 直線 AABB
			if (myInstance->GetType() == Collision3D::Type::Line && yourInstance->GetType() == Collision3D::Type::AABB)
			{
				if (CollisionCheckFunc(*static_cast<Collision3DInstanceAABB*>(yourInstance.get())->param_, *static_cast<Collision3DInstanceLine*>(myInstance.get())->param_))
					func_();

				continue;
			}

			// AABB 半直線
			if (myInstance->GetType() == Collision3D::Type::AABB && yourInstance->GetType() == Collision3D::Type::Ray)
			{
				if (CollisionCheckFunc(*static_cast<Collision3DInstanceAABB*>(myInstance.get())->param_, *static_cast<Collision3DInstanceRay*>(yourInstance.get())->param_))
					func_();

				continue;
			}

			// 半直線 AABB
			if (myInstance->GetType() == Collision3D::Type::Ray && yourInstance->GetType() == Collision3D::Type::AABB)
			{
				if (CollisionCheckFunc(*static_cast<Collision3DInstanceAABB*>(yourInstance.get())->param_, *static_cast<Collision3DInstanceRay*>(myInstance.get())->param_))
					func_();

				continue;
			}

			// AABB 線分
			if (myInstance->GetType() == Collision3D::Type::AABB && yourInstance->GetType() == Collision3D::Type::Segment)
			{
				if (CollisionCheckFunc(*static_cast<Collision3DInstanceAABB*>(myInstance.get())->param_, *static_cast<Collision3DInstanceSegment*>(yourInstance.get())->param_))
					func_();

				continue;
			}

			// 線分 AABB
			if (myInstance->GetType() == Collision3D::Type::Segment && yourInstance->GetType() == Collision3D::Type::AABB)
			{
				if (CollisionCheckFunc(*static_cast<Collision3DInstanceAABB*>(yourInstance.get())->param_, *static_cast<Collision3DInstanceSegment*>(myInstance.get())->param_))
					func_();

				continue;
			}

			// OBB 直線
			if (myInstance->GetType() == Collision3D::Type::OBB && yourInstance->GetType() == Collision3D::Type::Line)
			{
				if (CollisionCheckFunc(*static_cast<Collision3DInstanceOBB*>(myInstance.get())->param_, *static_cast<Collision3DInstanceLine*>(yourInstance.get())->param_))
					func_();

				continue;
			}

			// 直線 OBB
			if (myInstance->GetType() == Collision3D::Type::Line && yourInstance->GetType() == Collision3D::Type::OBB)
			{
				if (CollisionCheckFunc(*static_cast<Collision3DInstanceOBB*>(yourInstance.get())->param_, *static_cast<Collision3DInstanceLine*>(myInstance.get())->param_))
					func_();

				continue;
			}

			// OBB 半直線
			if (myInstance->GetType() == Collision3D::Type::OBB && yourInstance->GetType() == Collision3D::Type::Ray)
			{
				if (CollisionCheckFunc(*static_cast<Collision3DInstanceOBB*>(myInstance.get())->param_, *static_cast<Collision3DInstanceRay*>(yourInstance.get())->param_))
					func_();

				continue;
			}

			// 半直線 OBB
			if (myInstance->GetType() == Collision3D::Type::Ray && yourInstance->GetType() == Collision3D::Type::OBB)
			{
				if (CollisionCheckFunc(*static_cast<Collision3DInstanceOBB*>(yourInstance.get())->param_, *static_cast<Collision3DInstanceRay*>(myInstance.get())->param_))
					func_();

				continue;
			}

			// OBB 線分 
			if (myInstance->GetType() == Collision3D::Type::OBB && yourInstance->GetType() == Collision3D::Type::Segment)
			{
				if (CollisionCheckFunc(*static_cast<Collision3DInstanceOBB*>(myInstance.get())->param_, *static_cast<Collision3DInstanceSegment*>(yourInstance.get())->param_))
					func_();

				continue;
			}

			// 線分 OBB
			if (myInstance->GetType() == Collision3D::Type::Segment && yourInstance->GetType() == Collision3D::Type::OBB)
			{
				if (CollisionCheckFunc(*static_cast<Collision3DInstanceOBB*>(yourInstance.get())->param_, *static_cast<Collision3DInstanceSegment*>(myInstance.get())->param_))
					func_();

				continue;
			}
		}
	}
}