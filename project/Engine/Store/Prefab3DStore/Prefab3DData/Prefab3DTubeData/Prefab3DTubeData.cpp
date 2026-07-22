#include "Prefab3DTubeData.h"
#include "Parameter/Prefab3DParameter/Prefab3DParameter.h"

#include "Store/LightStore/LightStore.h"
#include "Store/TextureStore/TextureStore.h"
#include "Store/Camera3DStore/Camera3DStore.h"
#include "Store/SkyboxStore/SkyboxStore.h"
#include "Store/PostEffectStore/PostEffectStore.h"

#include "Application/WorldTransform/WorldTransform3D/WorldTransform3D.h"

#include "PSO/PSOModel/BasePSOModel.h"

#include "Resource/VertexBufferResource/CubeVertexResource/CubeVertexResource.h"

/// @brief コンストラクタ
/// @param name 
/// @param numInstance 
/// @param hPrefab 
/// @param hTexture 
/// @param parameter 
Engine::Prefab3DTubeData::Prefab3DTubeData(const std::string& name, uint32_t numInstance, Prefab3DHandle hPrefab3D, TextureHandle hTexture, BasePSOModel* pso, BaseComputePSO* csPso, Prefab3DParameter* parameter)
	: hTexture_(hTexture), csTubePSO_(csPso), Prefab3DBaseData(name, numInstance, hPrefab3D, pso, parameter)
{
	// 種類
	type_ = Prefab3D::Type::Tube;

	// パラメータを生成する
	param_ = std::make_unique<Prefab3D::Tube::Base::Param>();
}

/// @brief 初期化
/// @param textureStore 
/// @param lightStore 
/// @param cameraStore 
/// @param heap 
/// @param device 
/// @param log 
void Engine::Prefab3DTubeData::Initialize(TextureStore* textureStore, LightStore* lightStore, Camera3DStore* cameraStore,
	DX12Heap* heap, ID3D12Device* device,ID3D12GraphicsCommandList* commandList, Log* log)
{
	// nullptrチェック
	assert(textureStore);
	assert(lightStore);
	assert(cameraStore);
	assert(heap);
	assert(device);

	// 引数を受け取る
	textureStore_ = textureStore;
	lightStore_ = lightStore;
	cameraStore_ = cameraStore;


	// 頂点リソースの生成と初期化
	vertexResource_ = std::make_unique<RWStructuredVertexBufferResource<VertexDataForGPU>>();
	vertexResource_->Initialize(device, commandList, heap, (kMaxSlices + 1) * 2, log);

	// インデックスリソースの生成と初期化
	indexResource_ = std::make_unique<RWStructuredVertexBufferResource<uint32_t>>();
	indexResource_->Initialize(device,commandList, heap, kMaxSlices * 6, log);

	// プリミティブリソースの生成と初期化
	primitiveResource_ = std::make_unique<StructuredBufferResource<Prefab::TubeDataForGPU>>();
	primitiveResource_->Initialize(device, heap, numInstance_, log);

	// 分割リソースの生成と初期化
	divisionResource_ = std::make_unique<ConstantBufferResource<PrimitiveDataForGPU::TubeDivisionDataForGPU>>();
	divisionResource_->Initialize(device, log);

	// ビュー変換用リソースの生成と初期化
	viewResource_ = std::make_unique<ConstantBufferResource<Matrix4x4>>();
	viewResource_->Initialize(device, log);

	// シャドウマップリソースの生成と初期化
	shadowMapTransformationResource_ = std::make_unique<StructuredBufferResource<Matrix4x4>>();
	shadowMapTransformationResource_->Initialize(device, heap, numInstance_, log);

	// モーションベクターリソースの生成と初期化
	motionVectorResource_ = std::make_unique<StructuredBufferResource<MotionVectorDataForGPU>>();
	motionVectorResource_->Initialize(device, heap, numInstance_, log);


	// ブレンドモード
	param_->blendMode = BlendMode::kNone;

	// トランスフォーム
	param_->transform.scale = Vector3(1.0f, 1.0f, 1.0f);
	param_->transform.rotate = Vector3(0.0f, 0.0f, 0.0f);
	param_->transform.translate = Vector3(0.0f, 0.0f, 0.0f);

	// マテリアル
	param_->material.color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	param_->material.uv.scale = Vector2(1.0f, 1.0f);
	param_->material.uv.radius = 0.0f;
	param_->material.uv.translate = Vector2(0.0f, 0.0f);
	param_->material.hTexture = hTexture_;
	param_->material.environment = 0.0f;
	param_->material.shininess = 10.0f;
	param_->material.enableLighting = true;
	param_->material.enableDiffuse = true;
	param_->material.enableHalfLambert = true;
	param_->material.enableSpecular = true;
	param_->material.enableBlinnPhong = true;
	param_->material.enableShadow = true;
	param_->material.drawShadowMap = true;

	// ブラー
	param_->blur.afterImageMask = 0.0f;
	param_->blur.motionBlurMask = 0.0f;

	// 分割とサイズ
	param_->division.slices = 24;
	param_->size.height = 1.0f;
	param_->size.radiusTop = 0.5f;
	param_->size.radiusBottom = 0.5f;

	// テクスチャファイルパス
	textureFilePath_ = textureStore_->GetFilePath(hTexture_);

	// パラメータの記録
	group_ = "Tube_" + name_;

	if (parameter_)
	{
		parameter_->SetValue(group_, "BlendMode", &param_->blendMode);
		parameter_->SetValue(group_, "Transform_Scale", &param_->transform.scale);
		parameter_->SetValue(group_, "Transform_Rotate", &param_->transform.rotate);
		parameter_->SetValue(group_, "Transform_Translate", &param_->transform.translate);
		parameter_->SetValue(group_, "Material_Color", &param_->material.color);
		parameter_->SetValue(group_, "Material_UV_Scale", &param_->material.uv.scale);
		parameter_->SetValue(group_, "Material_UV_Rotate", &param_->material.uv.radius);
		parameter_->SetValue(group_, "Material_UV_Translate", &param_->material.uv.translate);
		parameter_->SetValue(group_, "Material_Environment", &param_->material.environment);
		parameter_->SetValue(group_, "Material_Shininess", &param_->material.shininess);
		parameter_->SetValue(group_, "Material_Enable_Lighting", &param_->material.enableLighting);
		parameter_->SetValue(group_, "Material_Enable_Diffuse", &param_->material.enableDiffuse);
		parameter_->SetValue(group_, "Material_Enable_HalfLambert", &param_->material.enableHalfLambert);
		parameter_->SetValue(group_, "Material_Enable_Specular", &param_->material.enableSpecular);
		parameter_->SetValue(group_, "Material_Enable_BlinnPhong", &param_->material.enableBlinnPhong);
		parameter_->SetValue(group_, "Material_Enable_Shadow", &param_->material.enableShadow);
		parameter_->SetValue(group_, "Material_DrawShadowMap", &param_->material.drawShadowMap);
		parameter_->SetValue(group_, "Blur_AfterImageMask", &param_->blur.afterImageMask);
		parameter_->SetValue(group_, "Blur_MotionBlurMask", &param_->blur.motionBlurMask);
		parameter_->SetValue(group_, "Material_Texture", &textureFilePath_);
		parameter_->SetValue(group_, "Division_Slices", &param_->division.slices);
		parameter_->SetValue(group_, "Size_Height", &param_->size.height);
		parameter_->SetValue(group_, "Size_RadiusTop", &param_->size.radiusTop);
		parameter_->SetValue(group_, "Size_RadiusBottom", &param_->size.radiusBottom);

		// 値を反映させる
		parameter_->RegisterGroupDataReflection(group_);
		param_->material.hTexture = textureStore_->GetHandle(textureFilePath_);
	}
}

/// @brief 更新処理
void Engine::Prefab3DTubeData::Update()
{
	// 削除されたインスタンスをリストから除外する
	instanceTable_.remove_if([](std::unique_ptr<PrefabInstanceTube>& instance) {if (instance->isDelete_) { return true; }return false; });
}

/// @brief リセット
void Engine::Prefab3DTubeData::Reset()
{
	if (parameter_->IsFileFound(group_))
	{
		// 値を反映させる
		if (parameter_)parameter_->RegisterGroupDataReflection(group_);
		param_->material.hTexture = textureStore_->GetHandle(textureFilePath_);
	}
	else
	{
		// ブレンドモード
		param_->blendMode = BlendMode::kNone;

		// モデルトランスフォーム
		param_->transform.scale = Vector3(1.0f, 1.0f, 1.0f);
		param_->transform.rotate = Vector3(0.0f, 0.0f, 0.0f);
		param_->transform.translate = Vector3(0.0f, 0.0f, 0.0f);

		// マテリアル
		param_->material.color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		param_->material.uv.scale = Vector2(1.0f, 1.0f);
		param_->material.uv.radius = 0.0f;
		param_->material.uv.translate = Vector2(0.0f, 0.0f);
		param_->material.hTexture = hTexture_;
		param_->material.environment = 0.0f;
		param_->material.shininess = 10.0f;
		param_->material.enableLighting = true;
		param_->material.enableDiffuse = true;
		param_->material.enableHalfLambert = true;
		param_->material.enableSpecular = true;
		param_->material.enableBlinnPhong = true;
		param_->material.enableShadow = true;
		param_->material.drawShadowMap = true;

		// ブラー
		param_->blur.afterImageMask = 0.0f;
		param_->blur.motionBlurMask = 0.0f;

		// 分割とサイズ
		param_->division.slices = 16;
		param_->size.height = 1.0f;
		param_->size.radiusTop = 0.5f;
		param_->size.radiusBottom = 0.5f;
	}

	// 読み込まれたことにする
	isLoad_ = true;
}

/// @brief コマンドリストに登録する
/// @param commandList 
/// @param pso 
void Engine::Prefab3DTubeData::Register(SkyboxStore* skyboxStore, ID3D12GraphicsCommandList* commandList)
{
	// 読み込まれていないときは処理しない
	if (!isLoad_)return;

	// インスタンス描画命令を行っていないときは処理しない
	if (numUseInstance_ <= 0)
		return;


	/*---------
	    計算
	---------*/

	if (perSlices_ != param_->division.slices)
	{
		// PSOの設定
		csTubePSO_->Register(commandList);

		// スライス数をクランプする
		param_->division.slices = std::clamp(param_->division.slices, 3, static_cast<int32_t>(kMaxSlices));

		// 分割の設定
		divisionResource_->data_->slices = param_->division.slices;
		divisionResource_->RegisterCompute(commandList, 0);

		// 頂点の設定
		vertexResource_->RegisterCompute(commandList, 1);

		// インデックスの設定
		indexResource_->RegisterCompute(commandList, 2);

		// ディスパッチ
		commandList->Dispatch((param_->division.slices + 1 + 15) / 16, 1, 1);
	}


	/*---------
	    描画
	---------*/

	// バリアを張る
	vertexResource_->Barrier(commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	indexResource_->Barrier(commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_INDEX_BUFFER);

	// PSOの設定
	pso_->Register(commandList, param_->blendMode);

	// 頂点の設定
	D3D12_VERTEX_BUFFER_VIEW vbv = {};
	vbv.BufferLocation = vertexResource_->GetResource()->GetGPUVirtualAddress();
	vbv.SizeInBytes = sizeof(VertexDataForGPU) * (kMaxSlices + 1) * 2;
	vbv.StrideInBytes = sizeof(VertexDataForGPU);
	commandList->IASetVertexBuffers(0, 1, &vbv);

	// インデックスの設定
	D3D12_INDEX_BUFFER_VIEW ibv = {};
	ibv.BufferLocation = indexResource_->GetResource()->GetGPUVirtualAddress();
	ibv.SizeInBytes = sizeof(uint32_t) * kMaxSlices * 6;
	ibv.Format = DXGI_FORMAT_R32_UINT;
	commandList->IASetIndexBuffer(&ibv);

	// プリミティブの設定
	primitiveResource_->RegisterGraphics(commandList, 0);

	// テクスチャの設定
	commandList->SetGraphicsRootDescriptorTable(1, textureStore_->GetSrvGpuHandle(param_->material.hTexture));

	// シャドウマップテクスチャの設定
	lightStore_->GetShadowMapTextureResource()->Register(commandList, 2);

	// シャドウ用座標変換の設定
	lightStore_->GetShadowMapTransformationResource()->RegisterGraphics(commandList, 3);

	// カメラの設定
	cameraStore_->RegisterCameraResource(commandList, 4);

	// スカイボックスの設定
	skyboxStore->RegisterCubeMapTexture(commandList, 5);

	// ライトの設定
	lightStore_->LightRegister(commandList, 6, 7, 8, 9);

	// ビュー変換の設定
	*viewResource_->data_ = cameraStore_->GetCamera3D().GetCurrentVPMatrix();
	viewResource_->RegisterGraphics(commandList, 10);

	// 形状の設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ドローコール
	commandList->DrawIndexedInstanced(param_->division.slices * 6, numUseInstance_, 0, 0, 0);

	// バリアを張る
	indexResource_->Barrier(commandList, D3D12_RESOURCE_STATE_INDEX_BUFFER, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	vertexResource_->Barrier(commandList, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);


	// スライス数を記録する
	perSlices_ = param_->division.slices;
}

/// @brief シャドウマップを描画する
/// @param viewProjection 
/// @param commandList 
/// @param pso 
void Engine::Prefab3DTubeData::DrawShadowMap(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOShadowMap* pso)
{
	// シャドウマップ用インスタンス数が0のときは処理しない
	if (numShadowInstance_ <= 0)return;

	// 読み込まれていないときは処理しない
	if (!isLoad_)return;

	// シャドウマップを描画しない設定のときは処理しない
	if (!param_->material.drawShadowMap)return;

	// デバッグ指定のオブジェクトは処理しない
	if (isDebug_)return;


	UINT useInstance = 0;

	// インスタンスごとに処理
	for (useInstance = 0; useInstance < numShadowInstance_; ++useInstance)
	{
		// インスタンス数を越えたら処理しない
		if (useInstance >= numInstance_)break;

		// ワールド座標
		shadowMapTransformationResource_->data_[useInstance] = primitiveResource_->data_[useInstance].world * viewProjection;
	}


	// 使用インスタンスがなかったら処理しない
	if (useInstance <= 0)
		return;


	/*------------------------
		コマンドリストに登録
	------------------------*/

	// バリアを張る
	vertexResource_->Barrier(commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	indexResource_->Barrier(commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_INDEX_BUFFER);

	// PSOの設定
	pso->Register(commandList);

	// 頂点の設定
	D3D12_VERTEX_BUFFER_VIEW vbv = {};
	vbv.BufferLocation = vertexResource_->GetResource()->GetGPUVirtualAddress();
	vbv.SizeInBytes = sizeof(VertexDataForGPU) * (kMaxSlices + 1) * 2;
	vbv.StrideInBytes = sizeof(VertexDataForGPU);
	commandList->IASetVertexBuffers(0, 1, &vbv);

	// インデックスの設定
	D3D12_INDEX_BUFFER_VIEW ibv = {};
	ibv.BufferLocation = indexResource_->GetResource()->GetGPUVirtualAddress();
	ibv.SizeInBytes = sizeof(uint32_t) * kMaxSlices * 6;
	ibv.Format = DXGI_FORMAT_R32_UINT;
	commandList->IASetIndexBuffer(&ibv);

	// 座標変換の設定
	shadowMapTransformationResource_->RegisterGraphics(commandList, 0);

	// 形状の設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ドローコール
	commandList->DrawIndexedInstanced(perSlices_ * 6, useInstance, 0, 0, 0);


	// 記録したシャドウインスタンスをリセットする
	numShadowInstance_ = 0;

	// バリアを張る
	indexResource_->Barrier(commandList, D3D12_RESOURCE_STATE_INDEX_BUFFER, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	vertexResource_->Barrier(commandList, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
}

/// @brief モーションベクターを描画する
/// @param commandList 
/// @param pso 
void Engine::Prefab3DTubeData::RegisterMotionVector(ID3D12GraphicsCommandList* commandList, BasePSOMotionVector* pso)
{
	// 読み込まれていないときは処理しない
	if (!isLoad_)return;

	// インスタンス描画命令を行っていないときは処理しない
	if (numUseInstance_ <= 0)
		return;


	// バリアを張る
	vertexResource_->Barrier(commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	indexResource_->Barrier(commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_INDEX_BUFFER);

	// PSOの設定
	pso->Register(commandList);

	// 頂点の設定
	D3D12_VERTEX_BUFFER_VIEW vbv = {};
	vbv.BufferLocation = vertexResource_->GetResource()->GetGPUVirtualAddress();
	vbv.SizeInBytes = sizeof(VertexDataForGPU) * (kMaxSlices + 1) * 2;
	vbv.StrideInBytes = sizeof(VertexDataForGPU);
	commandList->IASetVertexBuffers(0, 1, &vbv);

	// インデックスの設定
	D3D12_INDEX_BUFFER_VIEW ibv = {};
	ibv.BufferLocation = indexResource_->GetResource()->GetGPUVirtualAddress();
	ibv.SizeInBytes = sizeof(uint32_t) * kMaxSlices * 6;
	ibv.Format = DXGI_FORMAT_R32_UINT;

	// モーションベクトルの設定
	motionVectorResource_->RegisterGraphics(commandList, 0);

	// 形状の設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ドローコール
	commandList->DrawIndexedInstanced(perSlices_ * 6, numUseInstance_, 0, 0, 0);

	// バリアを張る
	indexResource_->Barrier(commandList, D3D12_RESOURCE_STATE_INDEX_BUFFER, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	vertexResource_->Barrier(commandList, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
}

/// @brief インスタンスのドローコール
void Engine::Prefab3DTubeData::DrawCallInstance(const Engine::Prefab3D::Tube::Instance::Param* param)
{
	// 読み込まれていないときは処理しない
	if (!isLoad_)return;

	// 使用インスタンス数が最大インスタンス数以上のときは処理しない
	if (numUseInstance_ >= numInstance_)
		return;

	Quaternion modelQuaternion =
		ToQuaternion(param->transform.rotate.z, Vector3(0.0f, 0.0, 1.0f)).Normalize() *
		ToQuaternion(param->transform.rotate.y, Vector3(0.0f, 1.0, 0.0f)).Normalize() *
		ToQuaternion(param->transform.rotate.x, Vector3(1.0f, 0.0, 0.0f)).Normalize();

	Matrix4x4 worldMatrix = Make3DAffineMatrix4x4(param->transform.scale, modelQuaternion, param->transform.translate);

	// 親トランスフォームがあるときは親トランスフォームを掛ける
	if (param->parent)worldMatrix = worldMatrix * param->parent->GetWorldMatrix();

	// ビュープロジェクション行列を取得する
	Matrix4x4 viewProjection = cameraStore_->GetCamera3D().GetCurrentVPMatrix();
	Matrix4x4 prevVPUnJitter = cameraStore_->GetCamera3D().GetPrevVPUnJitterMatrix();
	Matrix4x4 currentVPUnJitter = cameraStore_->GetCamera3D().GetCurrentVPUnJitterMatrix();


	// 前フレームのWVP行列
	motionVectorResource_->data_[numUseInstance_].prevWVPMatrix =
		primitiveResource_->data_[numUseInstance_].world * prevVPUnJitter;

	// ワールド座標
	primitiveResource_->data_[numUseInstance_].world = worldMatrix;

	// 現フレームのWVP行列
	motionVectorResource_->data_[numUseInstance_].currentWVPMatrix =
		primitiveResource_->data_[numUseInstance_].world * currentVPUnJitter;

	// 逆転置ワールド行列
	primitiveResource_->data_[numUseInstance_].worldInverseTranspose =
		primitiveResource_->data_[numUseInstance_].world.Transpose().Inverse();



	// 色
	primitiveResource_->data_[numUseInstance_].color = param->material.color;

	// 環境
	primitiveResource_->data_[numUseInstance_].environment = param->material.environment;

	// 光沢度
	primitiveResource_->data_[numUseInstance_].shininess = param->material.shininess;

	// UV行列
	primitiveResource_->data_[numUseInstance_].uvTransform =
		Make3DScaleMatrix4x4(Vector3(param->material.uv.scale.x, param->material.uv.scale.y, 1.0f)) *
		Make3DRotateZMatrix4x4(param->material.uv.radius) *
		Make3DTranslateMatrix4x4(Vector3(param->material.uv.translate.x, param->material.uv.translate.y, 0.0f));

	// ライティング有効化
	primitiveResource_->data_[numUseInstance_].enableLighting = static_cast<int32_t>(param->material.enableLighting);

	// ディフューズ有効化
	primitiveResource_->data_[numUseInstance_].enableDiffuse = static_cast<int32_t>(param->material.enableDiffuse);

	// ハーフランバード有効化
	primitiveResource_->data_[numUseInstance_].enableHalfLambert = static_cast<int32_t>(param->material.enableHalfLambert);

	// スペキュラー有効化
	primitiveResource_->data_[numUseInstance_].enableSpecular = static_cast<int32_t>(param->material.enableSpecular);

	// ブリンフォン有効化
	primitiveResource_->data_[numUseInstance_].enableBlinnPhong = static_cast<int32_t>(param->material.enableBlinnPhong);

	// シャドウ有効化
	primitiveResource_->data_[numUseInstance_].enableShadow = static_cast<int32_t>(param->material.enableShadow);


	// サイズ
	primitiveResource_->data_[numUseInstance_].height = param->size.height;
	primitiveResource_->data_[numUseInstance_].topRadius = param->size.radiusTop;
	primitiveResource_->data_[numUseInstance_].bottomRadius = param->size.radiusBottom;


	// ブラー
	motionVectorResource_->data_[numUseInstance_].afterImageMask = param->blur.afterImageMask;
	motionVectorResource_->data_[numUseInstance_].motionBlurMask = param->blur.motionBlurMask;

	// 使用インスタンスをカウントする
	numUseInstance_++;
	numShadowInstance_++;
}



/// @brief インスタンスを生成する
/// @return 
void* Engine::Prefab3DTubeData::CreateInstance()
{
	// インスタンスを生成する
	std::unique_ptr<PrefabInstanceTube> instance =
		std::make_unique<PrefabInstanceTube>([this](const Prefab3D::Tube::Instance::Param* param) {DrawCallInstance(param); }, param_.get());

	// ポインタを保存する
	PrefabInstanceTube* pInstance = instance.get();

	// テーブルに追加する
	instanceTable_.push_back(std::move(instance));

	return pInstance;
}

/// @brief 全てのインスタンスを削除する
void Engine::Prefab3DTubeData::DestroyAllInstance()
{
	// デバッグ用は削除しない
	if (isDebug_)return;

	instanceTable_.clear();
}

/// @brief デバッグ用パラメータ
void Engine::Prefab3DTubeData::DebugParameter()
{
#ifdef DEVELOPMENT

	// 読み込まれていないときは処理しない
	if (!isLoad_)return;

	// モデル名
	if (ImGui::TreeNode(name_.c_str()))
	{
		// ブレンドモード
		const char* blendModeStr[] = { "None", "Normal", "Add", "Subtract", "Multiply", "Screen" };
		int32_t currentBlendMode = static_cast<int32_t>(param_->blendMode);
		if (ImGui::Combo("BlendMode", &currentBlendMode, blendModeStr, IM_ARRAYSIZE(blendModeStr)))
		{
			param_->blendMode = static_cast<BlendMode>(currentBlendMode);
		}

		// モデルトランスフォーム
		if (ImGui::TreeNode("Model_Transform"))
		{
			// 拡縮
			ImGui::DragFloat3("Scale", &param_->transform.scale.x, 0.01f, -100000.0f, 100000.0f);

			// 回転
			ImGui::DragFloat3("Rotate", &param_->transform.rotate.x, 0.01f, -100000.0f, 100000.0f);

			// 平行移動
			ImGui::DragFloat3("Translate", &param_->transform.translate.x, 0.01f, -100000.0f, 100000.0f);

			// 終了
			ImGui::TreePop();
		}


		// マテリアル
		if (ImGui::TreeNode("Material"))
		{
			// UV
			if (ImGui::TreeNode("UV"))
			{
				// 拡縮
				ImGui::DragFloat2("Scale", &param_->material.uv.scale.x, 0.01f, -100000.0f, 100000.0f);

				// 回転
				ImGui::DragFloat("Rotate", &param_->material.uv.radius, 0.01f, -100000.0f, 100000.0f);

				// 平行移動
				ImGui::DragFloat2("Translate", &param_->material.uv.translate.x, 0.01f, -100000.0f, 100000.0f);

				// 終了
				ImGui::TreePop();
			}

			// 色
			ImGui::ColorEdit4("Color", &param_->material.color.x);

			// テクスチャ
			ImGui::Text("Texture");

			// テクスチャボタン
			ImGui::ImageButton(textureStore_->GetFilePath(param_->material.hTexture).c_str(), textureStore_->GetSrvGpuHandle(param_->material.hTexture).ptr,
				ImVec2(32.0f, 32.0f), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0.2f, 0.2f, 0.2f, 1.0f), ImVec4(1, 1, 1, 1));

			// ドラッグ＆ドロップのターゲットを作成する
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TEXTURE_ID"))
				{
					int droppedIndex = *(const int*)payload->Data;

					// ドロップされたテクスチャのハンドルを取得する
					param_->material.hTexture = static_cast<uint32_t>(droppedIndex);
					textureFilePath_ = textureStore_->GetFilePath(param_->material.hTexture);
				}
				ImGui::EndDragDropTarget();
			}



			ImGui::SeparatorText("Lighting");

			// ライティング有効化
			ImGui::Checkbox("Lighting", &param_->material.enableLighting);

			// シャドウマップ描画有効化
			ImGui::Checkbox("DrawShadowMap", &param_->material.drawShadowMap);

			if (param_->material.enableLighting)
			{
				// ディフューズ有効化
				ImGui::Checkbox("Diffuse", &param_->material.enableDiffuse);

				if (param_->material.enableDiffuse)
				{
					// ハーフランバート有効化
					ImGui::Checkbox("HalfLambert", &param_->material.enableHalfLambert);
				}

				// スペキュラー有効化
				ImGui::Checkbox("Specular", &param_->material.enableSpecular);

				if (param_->material.enableSpecular)
				{
					// ブリンフォン有効化
					ImGui::Checkbox("BlinnPhong", &param_->material.enableBlinnPhong);

					// 光沢度
					ImGui::DragFloat("Shininess", &param_->material.shininess, 0.1f);
				}

				// シャドウ有効化
				ImGui::Checkbox("Shadow", &param_->material.enableShadow);

				// 環境
				ImGui::SliderFloat("Environment", &param_->material.environment, 0.0f, 1.0f);
			}

			// ブラー
			if (PostEffectStore::IsEnableMotionVector())
			{
				if (ImGui::TreeNode("Blur"))
				{
					// 残像
					if (PostEffectStore::IsLoadAfterImage())
						ImGui::DragFloat("AfterImageMask", &param_->blur.afterImageMask, 0.01f, 0.0f, 1.0f);

					// モーションブラー
					if (PostEffectStore::IsLoadMotionBlur())
						ImGui::DragFloat("MotionBlurMask", &param_->blur.motionBlurMask, 0.01f, 0.0f, 1.0f);

					// 終了
					ImGui::TreePop();
				}
			}

			// 終了
			ImGui::TreePop();
		}

		// スライス
		ImGui::DragInt("Slices", &param_->division.slices, 1.0f, 3, kMaxSlices);

		// サイズ
		if (ImGui::TreeNode("Size"))
		{
			ImGui::DragFloat("Height", &param_->size.height, 0.01f, 0.0f, 100000.0f);
			ImGui::DragFloat("RadiusTop", &param_->size.radiusTop, 0.01f, 0.0f, 100000.0f);
			ImGui::DragFloat("RadiusBottom", &param_->size.radiusBottom, 0.01f, 0.0f, 100000.0f);
			ImGui::TreePop();
		}

		// インスタンス量
		ImGui::Text("Instance \n");
		ImGui::ProgressBar(static_cast<float>(instanceTable_.size()) / static_cast<float>(numInstance_), ImVec2(200.0f, 20.0f),
			std::format("{} / {}", static_cast<uint32_t>(instanceTable_.size()), numInstance_).c_str());

		ImGui::Text("\n");

		// 保存ボタン
		if (ImGui::Button("Save"))
		{
			parameter_->SaveFile(group_);
			std::string message = std::format("{} : saved.", group_);
			MessageBoxA(nullptr, message.c_str(), "RecordSetting", 0);
		}

		ImGui::Text("\n");

		// ロードボタン
		if (ImGui::Button("Load"))
		{
			parameter_->RegisterGroupDataReflection(group_);
			std::string message = std::format("{} : loaded.", group_);
			MessageBoxA(nullptr, message.c_str(), "RecordSetting", 0);
		}

		ImGui::Text("\n");

		// 終了
		ImGui::TreePop();
	}

#endif
}