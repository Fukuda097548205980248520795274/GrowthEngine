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

	// シャドウマップ用テクスチャリソースの生成と初期化
	shadowMapTextureResource_ = std::make_unique<ShadowMapTextureResource>();
	shadowMapTextureResource_->Initialize(heap, device, GrowthEngine::GetInstance()->GetScreenWidth(), GrowthEngine::GetInstance()->GetScreenHeight(), log);


	// ライト数リソースの生成と初期化
	numLightResource_ = std::make_unique<ConstantBufferResource<LightNumDataForGPU>>();
	numLightResource_->Initialize(device, log);

	// 平行光源リソースの作成と初期化
	directionalLightResource_ = std::make_unique<StructuredBufferResource<DirectionalLightDataForGPU>>();
	directionalLightResource_->Initialize(device, heap, kNumMaxLight, log);

	// 平行光源リソースの作成と初期化
	pointLightResource_ = std::make_unique<StructuredBufferResource<PointLightDataForGPU>>();
	pointLightResource_->Initialize(device, heap, kNumMaxLight, log);

	// 平行光源リソースの作成と初期化
	spotLightResource_ = std::make_unique<StructuredBufferResource<SpotLightDataForGPU>>();
	spotLightResource_->Initialize(device, heap, kNumMaxLight, log);
}

/// @brief リセット
void Engine::LightStore::Reset()
{
	// 数をリセット
	numLightResource_->data_->directionalLight = 0;
	numLightResource_->data_->pointLight = 0;
	numLightResource_->data_->spotLight = 0;
}

/// @brief ライト読み込み
/// @param name 
/// @param type 
/// @return 
LightHandle Engine::LightStore::Load(const std::string& name, Light::Type type)
{
	// 同じライトデータがあるかどうか
	for (auto& data : dataTable_)
	{
		if (data->GetName() == name && data->GetType() == type)
			return data->GetHandle();
	}

	// ハンドル
	LightHandle handle = static_cast<LightHandle>(dataTable_.size());

	// 平行光源
	if (type == Light::Type::Directional)
	{
		std::unique_ptr<DirectionalLightData> data = std::make_unique<DirectionalLightData>(name, handle);
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

		if (light->GetType() != Light::Type::Directional)
			continue;

		auto directionalLightData = static_cast<DirectionalLightData*>(light.get());

		// 深度をクリアする
		shadowMapTextureResource_->SetRenderTarget(commandList);
		shadowMapTextureResource_->ClearDepthStencil(commandList);

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

/// @brief ライトのコマンドリスト登録
/// @param commandList 
/// @param numLightRootParameterIndex 
/// @param directionalLightRootParameterIndex 
/// @param pointLightRootParameterIndex 
/// @param spotLightRootParameterIndex 
void Engine::LightStore::LightRegister(ID3D12GraphicsCommandList* commandList, UINT numLightRootParameterIndex,
	UINT directionalLightRootParameterIndex, UINT pointLightRootParameterIndex, UINT spotLightRootParameterIndex)
{
	numLightResource_->RegisterGraphics(commandList, numLightRootParameterIndex);
	directionalLightResource_->RegisterGraphics(commandList, directionalLightRootParameterIndex);
	pointLightResource_->RegisterGraphics(commandList, pointLightRootParameterIndex);
	spotLightResource_->RegisterGraphics(commandList, spotLightRootParameterIndex);
}

/// @brief セットする
/// @param hLight 
/// @param type 
void Engine::LightStore::Set(LightHandle hLight, Light::Type type)
{
	switch (type)
	{
	case Light::Type::Directional:
		SetDirection(dataTable_[hLight].get());
		break;

	case Light::Type::Point:

		break;

	case Light::Type::Spot:

		break;
	}
}

/// @brief 平行光源を設置する
/// @param lightData 
void Engine::LightStore::SetDirection(BaseLightData* lightData)
{
	// 最大数を超えないようにする
	if (numLightResource_->data_->directionalLight >= kNumMaxLight)
		return;

	// 型変換
	DirectionalLightData* data = static_cast<DirectionalLightData*>(lightData);
	Engine::Light::DirectionalLightParam* param = static_cast<Engine::Light::DirectionalLightParam*>(data->GetParam());

	// 値を渡す
	directionalLightResource_->data_[numLightResource_->data_->directionalLight].color = Vector4(param->color.x, param->color.y, param->color.z, 0.0f);
	directionalLightResource_->data_[numLightResource_->data_->directionalLight].direction = param->direction.Normalize();
	directionalLightResource_->data_[numLightResource_->data_->directionalLight].intensity = param->intensity;

	// 個数を加算
	numLightResource_->data_->directionalLight++;
}