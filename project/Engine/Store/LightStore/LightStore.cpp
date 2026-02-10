#include "LightStore.h"
#include "LightData/DirectionalLightData/DirectionalLightData.h"
#include <cassert>

/// @brief 初期化
/// @param device 
/// @param compiler 
/// @param log 
void Engine::LightStore::Initialize(ID3D12Device* device, ShaderCompiler* compiler, Log* log)
{
	// nullptrチェック
	assert(device);
	assert(compiler);

	// プリミティブ用シャドウマップPSOの生成と初期化
	psoShadowMapPrimitive_ = std::make_unique<PSOShadowMapPrimitive>();
	psoShadowMapPrimitive_->Initialize(device, compiler, log);
}

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

		// 平行光源のビュープロジェクション行列を取得する
		Matrix4x4 viewProjectionMatrix = directionalLightData->GetViewProjectionMatrix();

		break;
	}
}