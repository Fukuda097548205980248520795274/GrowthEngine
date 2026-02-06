#include "PrimitiveStore.h"

/// @brief 更新処理
/// @param viewProjection 
void Engine::PrimitiveStore::Update(const Matrix4x4& viewProjection)
{
	// 静的モデルデータ
	for (auto& data : staticModelTable_)data->Update(viewProjection);
}


/// @brief 静的モデル読み込み
/// @param model 
/// @param name 
/// @return 
PrimitiveStaticModelHandle Engine::PrimitiveStore::Load(PrimitiveStaticModel* model, ModelStore* modelStore, TextureStore* textureStore, ID3D12Device* device,
	ModelHandle hModel, const std::string& name, Log* log)
{
	// 同じデータがあるかどうか
	for (auto& data : staticModelTable_)
	{
		if (data->GetName() == name)
		{
			return data->GetHandle();
		}
	}

	// ハンドル
	PrimitiveStaticModelHandle handle = static_cast<PrimitiveStaticModelHandle>(staticModelTable_.size());

	std::unique_ptr<PrimitiveStaticModelData> data = std::make_unique<PrimitiveStaticModelData>(name, hModel, handle);
	data->Initialize(modelStore, textureStore, device, log);

	staticModelTable_.push_back(std::move(data));

	return handle;
}