#include "Collision3DStore.h"
#include "Collision3DData/CollisionSphereData/CollisionSphereData.h"
#include "Collision3DData/CollisionAABBData/CollisionAABBData.h"
#include "Collision3DData/CollisionOBBData/CollisionOBBData.h"
#include "Collision3DData/CollisionPlaneData/CollisionPlaneData.h"
#include "Collision3DData/CollisionLineData/CollisionLineData.h"
#include "Collision3DData/CollisionRayData/CollisionRayData.h"
#include "Collision3DData/CollisionSegmentData/CollisionSegmentData.h"

#include <cassert>

/// @brief 更新処理
void Engine::Collision3DStore::Update()
{
	for (auto& data : dataTable_)data->Update();

	for (auto& data : dataTable_)data->CollisionCheck();
}

/// @brief 読み込み
/// @param name 
/// @param type 
/// @return 
Collision3DHandle Engine::Collision3DStore::Load(std::function<void()> func, const std::string& name, Collision3D::Type type)
{
	// 同じデータがあるかどうか
	for (auto& data : dataTable_)
	{
		if (name == data->GetaName() && type == data->GetType())
			return data->GetHandle();
	}

	// ハンドルを取得
	Collision3DHandle handle = static_cast<uint32_t>(dataTable_.size());

	// 球
	if (type == Collision3D::Type::Sphere)
	{
		std::unique_ptr<CollisionSphereData> data = std::make_unique<CollisionSphereData>(func, name, type, handle);
		data->Initialize(this);
		dataTable_.push_back(std::move(data));
		return handle;
	}

	// AABB
	if (type == Collision3D::Type::AABB)
	{
		std::unique_ptr<CollisionAABBData> data = std::make_unique<CollisionAABBData>(func, name, type, handle);
		data->Initialize(this);
		dataTable_.push_back(std::move(data));
		return handle;
	}

	// OBB
	if (type == Collision3D::Type::OBB)
	{
		std::unique_ptr<CollisionOBBData> data = std::make_unique<CollisionOBBData>(func, name, type, handle);
		data->Initialize(this);
		dataTable_.push_back(std::move(data));
		return handle;
	}

	// 平面
	if (type == Collision3D::Type::Plane)
	{
		std::unique_ptr<CollisionPlaneData> data = std::make_unique<CollisionPlaneData>(func, name, type, handle);
		data->Initialize(this);
		dataTable_.push_back(std::move(data));
		return handle;
	}

	// 直線
	if (type == Collision3D::Type::Line)
	{
		std::unique_ptr<CollisionLineData> data = std::make_unique<CollisionLineData>(func, name, type, handle);
		data->Initialize(this);
		dataTable_.push_back(std::move(data));
		return handle;
	}

	// 半直線
	if (type == Collision3D::Type::Ray)
	{
		std::unique_ptr<CollisionRayData> data = std::make_unique<CollisionRayData>(func, name, type, handle);
		data->Initialize(this);
		dataTable_.push_back(std::move(data));
		return handle;
	}

	// 線分
	if (type == Collision3D::Type::Segment)
	{
		std::unique_ptr<CollisionSegmentData> data = std::make_unique<CollisionSegmentData>(func, name, type, handle);
		data->Initialize(this);
		dataTable_.push_back(std::move(data));
		return handle;
	}

	assert(false);
	return 0;
}