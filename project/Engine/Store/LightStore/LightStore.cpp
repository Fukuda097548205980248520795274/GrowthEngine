#include "LightStore.h"
#include "LightData/DirectionalLightData/DirectionalLightData.h"
#include "RenderContext/DX12Model/DX12Model.h"
#include "RenderContext/DX12Prefab/DX12Prefab.h"
#include <cassert>
#include "GrowthEngine.h"

/// @brief 初期化
/// @param device 
/// @param compiler 
/// @param log 
void Engine::LightStore::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, DX12Heap* heap, ShaderCompiler* compiler, Log* log)
{
	// nullptrチェック
	assert(device);
	assert(commandList);
	assert(compiler);
	assert(heap);

	// シャドウマップPSOの生成と初期化
	psoShadowMap_ = std::make_unique<PSOShadowMap>();
	psoShadowMap_->Initialize(device, compiler, log);

	// プレハブ用シャドウマップPSOの生成と初期化
	psoShadowMapPrefab_ = std::make_unique<PSOShadowMapPrefab>();
	psoShadowMapPrefab_->Initialize(device, compiler, log);


	// 座標変換用シャドウマップリソース
	shadowMapTransformationResource_ = std::make_unique<ConstantBufferResource<Matrix4x4>>();
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
void Engine::LightStore::Update(ID3D12GraphicsCommandList* commandList, DX12Model* model, DX12Prefab* prefab, const Matrix4x4& projectionMatrix)
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

		// シャドウマップ用に描画
		model->ShadowMapDraw(viewProjectionMatrix, commandList, psoShadowMap_.get());
		prefab->ShadowMapDraw(viewProjectionMatrix, commandList, psoShadowMapPrefab_.get());

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