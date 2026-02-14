#include "PrimitiveStore.h"
#include "PrimitiveData/PrimitiveStaticModelData/PrimitiveStaticModelData.h"
#include "PrimitiveData/PrimitiveAnimationModelData/PrimitiveAnimationModelData.h"

/// @brief 更新処理
/// @param viewProjection 
void Engine::PrimitiveStore::Update(const Matrix4x4& viewProjection)
{
	// 全てのプリミティブ
	for (auto& data : dataTable_)data->Update(viewProjection);
}

/// @brief シャドウマップ用の更新処理
/// @param viewProjection 
void Engine::PrimitiveStore::ShadowMapUpdate(const Matrix4x4& viewProjection)
{
	// 全てのプリミティブ
	for (auto& data : dataTable_)data->ShadowMapUpdate(viewProjection);
}

/// @brief シャドウマップ用の描画処理
/// @param commandList 
/// @param pso 
void Engine::PrimitiveStore::ShadowMapDraw(ID3D12GraphicsCommandList* commandList, BasePSOShadowMap* pso)
{
	// 静的モデルデータ
	for (auto& data : dataTable_)
	{
		// 静的モデル
		if (data->GetTypeName() == "StaticModel")
		{
			auto p = static_cast<PrimitiveStaticModelData*>(data.get());
			p->Register(commandList, pso);
		}

		// アニメーションモデル
		if (data->GetTypeName() == "AnimationModel")
		{
			auto p = static_cast<PrimitiveAnimationModelData*>(data.get());
			p->Register(commandList, pso);
		}
	}
}


/// @brief 読み込み
/// @param modelStore 
/// @param textureStore 
/// @param device 
/// @param hModel 
/// @param name 
/// @param type 
/// @param log 
/// @return 
PrimitiveHandle Engine::PrimitiveStore::Load(ModelStore* modelStore, TextureStore* textureStore, AnimationStore* animationStore, ID3D12Device* device,
	ModelHandle hModel, AnimationHandle hAnimation, const std::string& name, const std::string& type, Log* log)
{
	// 同じデータがあるかどうか
	for (auto& data : dataTable_)
	{
		if (data->GetName() == name && data->GetTypeName() == type)
			return data->GetHandle();
	}

	// ハンドル
	PrimitiveHandle handle = static_cast<PrimitiveHandle>(dataTable_.size());


	// 静的モデル
	if (type == "StaticModel")
	{
		std::unique_ptr<PrimitiveStaticModelData> data = std::make_unique<PrimitiveStaticModelData>(name, hModel, handle);
		data->Initialize(modelStore, textureStore, device, log);
		dataTable_.push_back(std::move(data));
		return handle;
	}

	// アニメーションモデル
	if (type == "AnimationModel")
	{
		std::unique_ptr<PrimitiveAnimationModelData> data = std::make_unique<PrimitiveAnimationModelData>(name, hModel, hAnimation, handle);
		data->Initialize(modelStore, textureStore, animationStore, device, log);
		dataTable_.push_back(std::move(data));
		return handle;
	}

	assert(false);
	return handle;
}

/// @brief コマンドリストに登録する
/// @param commandList 
/// @param handle 
/// @param meshIndex 
void Engine::PrimitiveStore::Register(ID3D12GraphicsCommandList* commandList, PrimitiveHandle handle, BasePSOModel* pso, LightStore* lightStore)
{
	// 静的モデル
	if (dataTable_[handle]->GetTypeName() == "StaticModel")
	{
		auto p = static_cast<PrimitiveStaticModelData*>(dataTable_[handle].get());
		p->Register(commandList, pso, lightStore);
	}

	// アニメーションモデル
	if (dataTable_[handle]->GetTypeName() == "AnimationModel")
	{
		auto p = static_cast<PrimitiveAnimationModelData*>(dataTable_[handle].get());
		p->Register(commandList, pso, lightStore);
	}
}