#include "Collision3DStore.h"
#include "Collision3DData/CollisionAABBData/CollisionAABBData.h"

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
Collision3DHandle Engine::Collision3DStore::Load(std::function<void()> func, const std::string& name, Collision::Type type)
{
	// 同じデータがあるかどうか
	for (auto& data : dataTable_)
	{
		if (name == data->GetaName() && type == data->GetType())
			return data->GetHandle();
	}

	// ハンドルを取得
	Collision3DHandle handle = static_cast<uint32_t>(dataTable_.size());

	// AABB
	if (type == Collision::Type::AABB)
	{
		std::unique_ptr<CollisionAABBData> data = std::make_unique<CollisionAABBData>(func, name, type, handle);
		data->Initialize(this);
		dataTable_.push_back(std::move(data));
		return handle;
	}

	assert(false);
	return 0;
}