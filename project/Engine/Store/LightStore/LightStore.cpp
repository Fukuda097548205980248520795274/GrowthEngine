#include "LightStore.h"

#include "LightData/DirectionalLightData/DirectionalLightData.h"
#include "LightData/PointLightData/PointLightData.h"
#include "LightData/SpotLightData/SpotLightData.h"

#include "RenderContext/DX12Render/DX12Render.h"
#include "RenderContext/DX12Prefab/DX12Prefab.h"
#include <cassert>
#include "GrowthEngine.h"

#include "RenderContext/ImGuiRender/ImGuiRender.h"

/// @brief コンストラクタ
Engine::LightStore::LightStore()
{
	// パラメータの生成と初期化
	parameter_ = std::make_unique<LightParameter>("Light");
}

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

/// @brief 更新処理
void Engine::LightStore::Update()
{
	for (auto& data : dataTable_)
	{
		// 使用していないと処理しない
		if (!data->IsLoad())
			continue;

		// 輝度がないと処理しない
		if (data->GetIntensity() <= 0.0f)
			continue;

		// 更新処理
		data->Update();

		// ライトの種類ごとの処理
		switch (data->GetType())
		{
		case Light::Type::Directional:
			SetDirection(data.get());
			break;

		case Light::Type::Point:
			SetPoint(data.get());
			break;

		case Light::Type::Spot:
			SetSpot(data.get());
			break;
		}
	}
}

/// @brief リセット
void Engine::LightStore::FrameReset()
{
	// 数をリセット
	numLightResource_->data_->directionalLight = 0;
	numLightResource_->data_->pointLight = 0;
	numLightResource_->data_->spotLight = 0;
}

/// @brief シーン毎リセット
void Engine::LightStore::PerSceneReset()
{
	// シーン前処理
	for (auto& data : dataTable_)data->PerSceneReset();
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
		{
			// リセットしてハンドルを返す
			data->Reset();
			return data->GetHandle();
		}
	}

	// ハンドル
	LightHandle handle = static_cast<LightHandle>(dataTable_.size());

	// 名前テーブルに記録する
	nameTable_[name] = handle;

	// 平行光源
	if (type == Light::Type::Directional)
	{
		std::unique_ptr<DirectionalLightData> data = std::make_unique<DirectionalLightData>(name, handle);
		data->Initialize(parameter_.get());
		dataTable_.push_back(std::move(data));

		return handle;
	}

	// ポイントライト
	if (type == Light::Type::Point)
	{
		std::unique_ptr<PointLightData> data = std::make_unique<PointLightData>(name, handle);
		data->Initialize(parameter_.get());
		dataTable_.push_back(std::move(data));

		return handle;
	}

	// スポットライト
	if (type == Light::Type::Spot)
	{
		std::unique_ptr<SpotLightData> data = std::make_unique<SpotLightData>(name, handle);
		data->Initialize(parameter_.get());
		dataTable_.push_back(std::move(data));

		return handle;
	}


	assert(false);
	return 0;
	
}


/// @brief シャドウマップ
/// @param commandList 
/// @param render 
/// @param prefab 
/// @param projectionMatrix 
void Engine::LightStore::ShadowMap(ID3D12GraphicsCommandList* commandList, DX12Render* render, DX12Prefab* prefab, const Matrix4x4& projectionMatrix)
{
	// 平行光源を探す
	for (auto& light : dataTable_)
	{
		// 読み込まれているかどうか
		if(!light->IsLoad())
			continue;

		// 平行光源であるかどうか
		if (light->GetType() != Light::Type::Directional)
			continue;

		auto directionalLightData = static_cast<DirectionalLightData*>(light.get());

		// 深度をクリアする
		shadowMapTextureResource_->SetRenderTarget(commandList);
		shadowMapTextureResource_->ClearDepthStencil(commandList);

		// 平行光源のビュープロジェクション行列を取得する
		Matrix4x4 viewProjectionMatrix = directionalLightData->GetViewProjectionMatrix();

		// シャドウマップ用に描画
		render->ShadowMapDraw(viewProjectionMatrix, commandList, psoShadowMap_.get());
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
	directionalLightResource_->data_[numLightResource_->data_->directionalLight].direction = param->direction;
	directionalLightResource_->data_[numLightResource_->data_->directionalLight].intensity = param->intensity;

	// 個数を加算
	numLightResource_->data_->directionalLight++;
}

/// @brief ポイントライトを設置する
/// @param lightData 
void Engine::LightStore::SetPoint(BaseLightData* lightData)
{
	// 最大数を超えないようにする
	if (numLightResource_->data_->pointLight >= kNumMaxLight)
		return;

	// 型変換
	PointLightData* data = static_cast<PointLightData*>(lightData);
	Engine::Light::PointLightParam* param = static_cast<Engine::Light::PointLightParam*>(data->GetParam());

	// 値を渡す
	pointLightResource_->data_[numLightResource_->data_->pointLight].color = Vector4(param->color.x, param->color.y, param->color.z, 0.0f);
	pointLightResource_->data_[numLightResource_->data_->pointLight].position = param->position;
	pointLightResource_->data_[numLightResource_->data_->pointLight].intensity = param->intensity;
	pointLightResource_->data_[numLightResource_->data_->pointLight].radius = param->radius;
	pointLightResource_->data_[numLightResource_->data_->pointLight].decay = param->decay;

	// 個数を加算
	numLightResource_->data_->pointLight++;
}

/// @brief スポットライトを設置する
/// @param lightData 
void Engine::LightStore::SetSpot(BaseLightData* lightData)
{
	// 最大数を超えないようにする
	if (numLightResource_->data_->spotLight >= kNumMaxLight)
		return;

	// 型変換
	SpotLightData* data = static_cast<SpotLightData*>(lightData);
	Engine::Light::SpotLightParam* param = static_cast<Engine::Light::SpotLightParam*>(data->GetParam());

	// 値を渡す
	spotLightResource_->data_[numLightResource_->data_->spotLight].color = Vector4(param->color.x, param->color.y, param->color.z, 0.0f);
	spotLightResource_->data_[numLightResource_->data_->spotLight].position = param->position;
	spotLightResource_->data_[numLightResource_->data_->spotLight].direction = param->direction;
	spotLightResource_->data_[numLightResource_->data_->spotLight].intensity = param->intensity;
	spotLightResource_->data_[numLightResource_->data_->spotLight].distance = param->distance;
	spotLightResource_->data_[numLightResource_->data_->spotLight].decay = param->decay;
	spotLightResource_->data_[numLightResource_->data_->spotLight].cosAngle = param->cosAngle;
	spotLightResource_->data_[numLightResource_->data_->spotLight].cosFalloffStart = param->cosFalloffStart;


	// 個数を加算
	numLightResource_->data_->spotLight++;
}

/// @brief デバッグ用パラメータ
void Engine::LightStore::DebugParameter()
{
#ifdef _DEVELOPMENT

	// メニューバーを使用する
	if (!ImGui::Begin("Light"))
	{
		ImGui::End();
		return;
	}

	for (auto& data : dataTable_)data->DebugParameter();

	// 終了
	ImGui::End();

#endif
}

/// @brief デバッグ用の線を描画
void Engine::LightStore::DebugDrawLine()
{
	for (auto& data : dataTable_)data->DebugDrawLine();
}

/// @brief デバッグ用レイピッキング
/// @param ray 
/// @param pickList 
void Engine::LightStore::DebugRayPicking(const Collision3D::Ray& ray, std::vector<std::pair<float, DebugData::DebugGuizmoData*>>& pickList)
{
	for (auto& data : dataTable_)data->DebugRayPicking(ray, pickList);
}

/// @brief デバッグ用Guizmo操作
/// @param viewMatrix 
/// @param projMatrix 
void Engine::LightStore::DebugGuizmo(const Matrix4x4& viewMatrix, const Matrix4x4& projMatrix)
{
	for (auto& data : dataTable_)data->DebugGuizmo(viewMatrix, projMatrix);
}