#include "LightStore.h"
#include "LightData/DirectionalLightData/DirectionalLightData.h"
#include <cassert>

/// @brief 読み込み
/// @param name 
/// @param type 
/// @return 
LightHandle Engine::LightStore::Load(const std::string& name, const std::string& type,
	DX12Heap* heap, ID3D12Device* device, Log* log)
{
	// nullptrチェック
	assert(heap);
	assert(device);

	// 同じライトデータがあるかどうか
	for (auto& data : dataTable_)
	{
		if (data->GetName() == name && data->GetTypeName() == type)
			return data->GetHandle();
	}

	// ハンドル
	LightHandle handle = static_cast<LightHandle>(dataTable_.size());

	// 平行光源
	if (type == "Directional")
	{
		std::unique_ptr<DirectionalLightData> data = std::make_unique<DirectionalLightData>(name, handle);
		data->Initialize(heap, device, log);
		dataTable_.push_back(std::move(data));

		return handle;
	}


	assert(false);
	return 0;
	
}


/// @brief 深度ステンシルをクリア
/// @param commandList 
void Engine::LightStore::ClearDepthStencil(ID3D12GraphicsCommandList* commandList)
{
	// 平行光源を探す
	for (auto& light : dataTable_)
	{
		if (light->GetTypeName() != "Directional")
			continue;

		auto directionalLightData = static_cast<DirectionalLightData*>(light.get());

		// 深度をクリアする
		directionalLightData->ClearDepthStencil(commandList);
	}
}