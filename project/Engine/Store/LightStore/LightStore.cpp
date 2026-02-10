#include "LightStore.h"
#include "LightData/DirectionalLightData/DirectionalLightData.h"
#include "RenderContext/DX12Primitive/DX12Primitive.h"
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

	// 座標変換用シャドウマップリソース
	shadowMapTransformationResource_ = std::make_unique<ShadowMapTransformationResource>();
	shadowMapTransformationResource_->Initialize(device, log);
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


/// @brief 更新処理
/// @param commandList 
void Engine::LightStore::Update(ID3D12GraphicsCommandList* commandList, DX12Primitive* primitive)
{
	// 平行光源を探す
	for (auto& light : dataTable_)
	{
		if (light->GetTypeName() != "Directional")
			continue;

		auto directionalLightData = static_cast<DirectionalLightData*>(light.get());

		// 深度をクリアする
		directionalLightData->SetRenderTarget(commandList);
		directionalLightData->ClearDepthStencil(commandList);

		// 平行光源のビュープロジェクション行列を取得する
		Matrix4x4 viewProjectionMatrix = directionalLightData->GetViewProjectionMatrix();

		// シャドウマップ用にプリミティブ更新
		primitive->ShadowMapUpdate(viewProjectionMatrix);

		// シャドウマップ用に描画
		primitive->ShadowMapDraw(commandList, psoShadowMapPrimitive_.get());

		// データを渡す
		*shadowMapTransformationResource_->data_ = viewProjectionMatrix;

		break;
	}
}

/// @brief シャドウマップテクスチャリソースを取得する
/// @return 
Engine::ShadowMapTextureResource* Engine::LightStore::GetShadowMapTextureResource()
{
	// 平行光源を探す
	for (auto& light : dataTable_)
	{
		if (light->GetTypeName() != "Directional")
			continue;

		auto directionalLightData = static_cast<DirectionalLightData*>(light.get());

		return directionalLightData->GetShadowMapTextureResource();
	}

	return nullptr;
}