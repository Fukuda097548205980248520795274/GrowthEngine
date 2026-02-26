#include "Collision2DStore.h"
#include "Collision2DData/Collision2DCircleData/Collision2DCircleData.h"
#include "Collision2DData/Collision2DSpriteData/Collision2DSpriteData.h"

#include <cassert>

/// @brief 更新処理
void Engine::Collision2DStore::Update()
{
	for (auto& data : dataTable_)data->Update();

	for (auto& data : dataTable_)data->CollisionCheck();
}

/// @brief 読み込み
/// @param name 
/// @param type 
/// @return 
Collision2DHandle Engine::Collision2DStore::Load(std::function<void()> func, const std::string& name, Collision2D::Type type)
{
	// 同じデータがあるかどうか
	for (auto& data : dataTable_)
	{
		if (name == data->GetaName() && type == data->GetType())
			return data->GetHandle();
	}

	// ハンドルを取得
	Collision2DHandle handle = static_cast<uint32_t>(dataTable_.size());

	// 円
	if (type == Collision2D::Type::Circle)
	{
		std::unique_ptr<Collision2DCircleData> data = std::make_unique<Collision2DCircleData>(func, name, type, handle);
		data->Initialize(this);
		dataTable_.push_back(std::move(data));
		return handle;
	}

	// 矩形
	if (type == Collision2D::Type::Sprite)
	{
		std::unique_ptr<Collision2DSpriteData> data = std::make_unique<Collision2DSpriteData>(func, name, type, handle);
		data->Initialize(this);
		dataTable_.push_back(std::move(data));
		return handle;
	}

	assert(false);
	return 0;
}