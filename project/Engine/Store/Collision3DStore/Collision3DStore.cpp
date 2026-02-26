#include "Collision3DStore.h"
#include "Collision3DData/Collision3DSphereData/Collision3DSphereData.h"
#include "Collision3DData/Collision3DAABBData/Collision3DAABBData.h"
#include "Collision3DData/Collision3DOBBData/Collision3DOBBData.h"
#include "Collision3DData/Collision3DPlaneData/Collision3DPlaneData.h"
#include "Collision3DData/Collision3DLineData/Collision3DLineData.h"
#include "Collision3DData/Collision3DRayData/Collision3DRayData.h"
#include "Collision3DData/Collision3DSegmentData/Collision3DSegmentData.h"

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
		std::unique_ptr<Collision3DSphereData> data = std::make_unique<Collision3DSphereData>(func, name, type, handle);
		data->Initialize(this);
		dataTable_.push_back(std::move(data));
		return handle;
	}

	// AABB
	if (type == Collision3D::Type::AABB)
	{
		std::unique_ptr<Collision3DAABBData> data = std::make_unique<Collision3DAABBData>(func, name, type, handle);
		data->Initialize(this);
		dataTable_.push_back(std::move(data));
		return handle;
	}

	// OBB
	if (type == Collision3D::Type::OBB)
	{
		std::unique_ptr<Collision3DOBBData> data = std::make_unique<Collision3DOBBData>(func, name, type, handle);
		data->Initialize(this);
		dataTable_.push_back(std::move(data));
		return handle;
	}

	// 平面
	if (type == Collision3D::Type::Plane)
	{
		std::unique_ptr<Collision3DPlaneData> data = std::make_unique<Collision3DPlaneData>(func, name, type, handle);
		data->Initialize(this);
		dataTable_.push_back(std::move(data));
		return handle;
	}

	// 直線
	if (type == Collision3D::Type::Line)
	{
		std::unique_ptr<Collision3DLineData> data = std::make_unique<Collision3DLineData>(func, name, type, handle);
		data->Initialize(this);
		dataTable_.push_back(std::move(data));
		return handle;
	}

	// 半直線
	if (type == Collision3D::Type::Ray)
	{
		std::unique_ptr<Collision3DRayData> data = std::make_unique<Collision3DRayData>(func, name, type, handle);
		data->Initialize(this);
		dataTable_.push_back(std::move(data));
		return handle;
	}

	// 線分
	if (type == Collision3D::Type::Segment)
	{
		std::unique_ptr<Collision3DSegmentData> data = std::make_unique<Collision3DSegmentData>(func, name, type, handle);
		data->Initialize(this);
		dataTable_.push_back(std::move(data));
		return handle;
	}

	assert(false);
	return 0;
}