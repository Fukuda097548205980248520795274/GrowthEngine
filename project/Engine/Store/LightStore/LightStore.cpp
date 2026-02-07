#include "LightStore.h"
#include "LightData/DirectionalLightData/DirectionalLightData.h"
#include <cassert>

/// @brief 読み込み
/// @param name 
/// @param type 
/// @return 
LightHandle Engine::LightStore::Load(const std::string& name, const std::string& type)
{
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
		dataTable_.push_back(std::move(data));

		return handle;
	}


	assert(false);
	return 0;
	
}

/// @brief パラメータを取得する
/// @param handle 
/// @return 
void* Engine::LightStore::GetParam(LightHandle handle)
{
	BaseLightData* data = dataTable_[handle].get();

	// 平行光源
	if (data->GetTypeName() == "Directional")
	{
		auto* p = static_cast<DirectionalLightData*>(data->GetParam());
		return p;
	}

	assert(false);
	return nullptr;
}