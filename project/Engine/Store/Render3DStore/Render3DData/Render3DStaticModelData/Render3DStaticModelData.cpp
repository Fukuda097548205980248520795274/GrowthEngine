#include "Render3DStaticModelData.h"
#include "Store/ModelStore/ModelStore.h"
#include "Store/TextureStore/TextureStore.h"
#include <cassert>
#include "PSO/PSOModel/BasePSOModel.h"
#include "PSO/PSOShadowMap/BasePSOShadowMap.h"
#include "Store/LightStore/LightStore.h"
#include "Store/Camera3DStore/Camera3DStore.h"
#include "Store/SkyboxStore/SkyboxStore.h"

#include <numbers>

#include "RenderContext/ImGuiRender/ImGuiRender.h"

#include "Parameter/Render3DParameter/Render3DParameter.h"

#include "Func/CollisionFunc/CollisionFunc.h"

#include "GrowthEngine.h"

/// @brief 初期化
/// @param modelStore 
/// @param device 
void Engine::Render3DStaticModelData::Initialize(ModelStore* modelStore, TextureStore* textureStore, LightStore* lightStore, ID3D12Device* device, Log* log)
{
	// nullptrチェック
	assert(modelStore);
	assert(textureStore);
	assert(lightStore);
	assert(device);

	// 引数を受け取る
	modelStore_ = modelStore;
	textureStore_ = textureStore;
	lightStore_ = lightStore;


	// パラメータの生成
	param_ = std::make_unique<Render3D::StaticModel::Param>();


	// ブレンドモード
	param_->blendMode = BlendMode::kNone;

	// モデルトランスフォーム
	param_->modelTransform.scale = Vector3(1.0f, 1.0f, 1.0f);
	param_->modelTransform.rotate = Vector3(0.0f, 0.0f, 0.0f);
	param_->modelTransform.translate = Vector3(0.0f, 0.0f, 0.0f);



	// モデルデータを取得する
	const ModelData& modelData = modelStore_->GetModelData(hModel_);

	// パラメータ領域確保
	param_->meshTransforms.resize(static_cast<int32_t>(modelData.meshes.size()));
	param_->meshMaterial.resize(static_cast<int32_t>(modelData.meshes.size()));
	param_->meshBlur.resize(static_cast<int32_t>(modelData.meshes.size()));
	param_->meshOutline.resize(static_cast<int32_t>(modelData.meshes.size()));

	// リソース領域確保
	meshTransformationResources_.resize(static_cast<int32_t>(modelData.meshes.size()));
	meshMaterialResources_.resize(static_cast<int32_t>(modelData.meshes.size()));
	shadowMapTransformationResource_.resize(static_cast<int32_t>(modelData.meshes.size()));
	motionVectorResources_.resize(static_cast<int32_t>(modelData.meshes.size()));
	outlineColorResources_.resize(static_cast<int32_t>(modelData.meshes.size()));
	outlineTransformationResources_.resize(static_cast<int32_t>(modelData.meshes.size()));
	
	// ファイルパス
	textureFilePathTable_.resize(static_cast<int32_t>(modelData.meshes.size()));

	for (int32_t meshIndex = 0; meshIndex < modelData.meshes.size(); ++meshIndex)
	{
		// メッシュトランスフォーム
		param_->meshTransforms[meshIndex].scale = Vector3(1.0f, 1.0f, 1.0f);
		param_->meshTransforms[meshIndex].rotate = Vector3(0.0f, 0.0f, 0.0f);
		param_->meshTransforms[meshIndex].translate = Vector3(0.0f, 0.0f, 0.0f);

		// マテリアルトランスフォーム
		param_->meshMaterial[meshIndex].hTexture = modelData.meshes[meshIndex].material.handle;
		param_->meshMaterial[meshIndex].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		param_->meshMaterial[meshIndex].uv.scale = Vector2(1.0f, 1.0f);
		param_->meshMaterial[meshIndex].uv.radius = 0.0f;
		param_->meshMaterial[meshIndex].uv.translate = Vector2(0.0f, 0.0f);
		param_->meshMaterial[meshIndex].environment = 0.0f;
		param_->meshMaterial[meshIndex].shininess = 10.0f;
		param_->meshMaterial[meshIndex].enableLighting = true;
		param_->meshMaterial[meshIndex].enableDiffuse = true;
		param_->meshMaterial[meshIndex].enableHalfLambert = true;
		param_->meshMaterial[meshIndex].enableSpecular = true;
		param_->meshMaterial[meshIndex].enableBlinnPhong = true;
		param_->meshMaterial[meshIndex].drawShadowMap = true;
		param_->meshMaterial[meshIndex].enableShadow = true;

		// ブラー
		param_->meshBlur[meshIndex].afterImageMask = 0.0f;
		param_->meshBlur[meshIndex].motionBlurMask = 0.0f;

		// アウトライン
		param_->meshOutline[meshIndex].enableOutline = false;
		param_->meshOutline[meshIndex].color = Vector4(0.0f, 0.0f, 0.0f, 1.0f);

		// テクスチャファイルパス
		textureFilePathTable_[meshIndex] = textureStore_->GetFilePath(param_->meshMaterial[meshIndex].hTexture);


		// 座標変換リソース
		meshTransformationResources_[meshIndex] = std::make_unique<ConstantBufferResource<PrimitiveModelTransformationDataForGPU>>();
		meshTransformationResources_[meshIndex]->Initialize(device, log);
		meshTransformationResources_[meshIndex]->data_->worldMatrix = MakeIdentityMatrix4x4();

		// マテリアルリソース
		meshMaterialResources_[meshIndex] = std::make_unique<ConstantBufferResource<PrimitiveModelMaterialDataForGPU>>();
		meshMaterialResources_[meshIndex]->Initialize(device, log);

		// シャドウマップ用座標変換リソース
		shadowMapTransformationResource_[meshIndex] = std::make_unique<ConstantBufferResource<Matrix4x4>>();
		shadowMapTransformationResource_[meshIndex]->Initialize(device, log);

		// モーションベクター用座標変換リソース
		motionVectorResources_[meshIndex] = std::make_unique<ConstantBufferResource<MotionVectorDataForGPU>>();
		motionVectorResources_[meshIndex]->Initialize(device, log);

		// アウトライン用座標変換リソース
		outlineTransformationResources_[meshIndex] = std::make_unique<ConstantBufferResource<Matrix4x4>>();
		outlineTransformationResources_[meshIndex]->Initialize(device, log);

		// アウトライン用色リソース
		outlineColorResources_[meshIndex] = std::make_unique<ConstantBufferResource<Vector4>>();
		outlineColorResources_[meshIndex]->Initialize(device, log);
	}
}

/// @brief 更新処理
void Engine::Render3DStaticModelData::Update()
{
	// 描画を記録する
	isPreDrew_ = isDrew_;
	isDrew_ = false;
}

/// @brief リセット
void Engine::Render3DStaticModelData::Reset()
{
	// 読み込み
	isLoad_ = true;
}

/// @brief コマンドリストに登録する
/// @param commandList 
/// @param pso 
/// @param textureStore 
void Engine::Render3DStaticModelData::Register(Camera3DStore* cameraStore, SkyboxStore* skyboxStore, ID3D12GraphicsCommandList* commandList, BasePSOModel* pso)
{
	// 読み込まれていないときは処理しない
	if (!isLoad_)return;

	// モデルデータを取得する
	const ModelData& modelData = modelStore_->GetModelData(hModel_);

	Quaternion modelQuaternion =
		ToQuaternion(param_->modelTransform.rotate.z, Vector3(0.0f, 0.0, 1.0f)).Normalize() *
		ToQuaternion(param_->modelTransform.rotate.y, Vector3(0.0f, 1.0, 0.0f)).Normalize() *
		ToQuaternion(param_->modelTransform.rotate.x, Vector3(1.0f, 0.0, 0.0f)).Normalize();

	Matrix4x4 worldMatrix = Make3DAffineMatrix4x4(param_->modelTransform.scale, modelQuaternion, param_->modelTransform.translate);
	if (parent_)worldMatrix = worldMatrix * parent_->GetWorldMatrix();

	// ビュープロジェクション行列を取得する
	Matrix4x4 viewProjection = cameraStore->GetCamera3D().GetCurrentVPMatrix();
	Matrix4x4 prevVPUnJitter = cameraStore->GetCamera3D().GetPrevVPUnJitterMatrix();
	Matrix4x4 currentVPUnJitter = cameraStore->GetCamera3D().GetCurrentVPUnJitterMatrix();


	// PSOの設定
	pso->Register(commandList, param_->blendMode);

	// カメラリソースの設定
	cameraStore->RegisterCameraResource(commandList, 5);

	// スカイボックスの設定
	skyboxStore->RegisterCubeMapTexture(commandList, 6);

	// ライトの設定
	lightStore_->LightRegister(commandList, 7, 8, 9, 10);

	for (int32_t meshIndex = 0; meshIndex < static_cast<int32_t>(modelStore_->GetModelData(hModel_).meshes.size()); meshIndex++)
	{
		/*-----------------
		    データを渡す
		-----------------*/

		// ノード行列
		Matrix4x4 nodeMatrix = MakeIdentityMatrix4x4();
		if (!modelData.nodes.empty())nodeMatrix = modelData.nodes[static_cast<int32_t>(modelStore_->GetModelData(hModel_).meshes.size()) - 1 - meshIndex].worldMatrix;

		Quaternion meshQuaternion =
			ToQuaternion(param_->meshTransforms[meshIndex].rotate.z, Vector3(0.0f, 0.0, 1.0f)).Normalize() *
			ToQuaternion(param_->meshTransforms[meshIndex].rotate.y, Vector3(0.0f, 1.0, 0.0f)).Normalize() *
			ToQuaternion(param_->meshTransforms[meshIndex].rotate.x, Vector3(1.0f, 0.0, 0.0f)).Normalize();

		Matrix4x4 localMatrix = Make3DAffineMatrix4x4(param_->meshTransforms[meshIndex].scale, meshQuaternion, param_->meshTransforms[meshIndex].translate);


		// 前フレームのWVP行列
		motionVectorResources_[meshIndex]->data_->prevWVPMatrix =
			meshTransformationResources_[meshIndex]->data_->worldMatrix * prevVPUnJitter;

		// ワールド座標
		meshTransformationResources_[meshIndex]->data_->worldMatrix =
			localMatrix * nodeMatrix * worldMatrix;

		// 現フレームのWVP行列
		motionVectorResources_[meshIndex]->data_->currentWVPMatrix =
			meshTransformationResources_[meshIndex]->data_->worldMatrix * currentVPUnJitter;

		// ワールドビュー正射影行列
		meshTransformationResources_[meshIndex]->data_->worldViewProjectionMatrix =
			meshTransformationResources_[meshIndex]->data_->worldMatrix * viewProjection;

		// 逆転置ワールド行列
		meshTransformationResources_[meshIndex]->data_->worldInverseTransposeMatrix =
			meshTransformationResources_[meshIndex]->data_->worldMatrix.Transpose().Inverse();



		// 色
		meshMaterialResources_[meshIndex]->data_->color = param_->meshMaterial[meshIndex].color;

		// 環境
		meshMaterialResources_[meshIndex]->data_->environment = param_->meshMaterial[meshIndex].environment;

		// 光沢度
		meshMaterialResources_[meshIndex]->data_->shininess = param_->meshMaterial[meshIndex].shininess;

		// UV行列
		meshMaterialResources_[meshIndex]->data_->uvMatrix =
			Make3DScaleMatrix4x4(Vector3(param_->meshMaterial[meshIndex].uv.scale.x, param_->meshMaterial[meshIndex].uv.scale.y, 1.0f)) *
			Make3DRotateZMatrix4x4(param_->meshMaterial[meshIndex].uv.radius) *
			Make3DTranslateMatrix4x4(Vector3(param_->meshMaterial[meshIndex].uv.translate.x, param_->meshMaterial[meshIndex].uv.translate.y, 0.0f));

		// ライティング有効化
		meshMaterialResources_[meshIndex]->data_->enableLighting = static_cast<int32_t>(param_->meshMaterial[meshIndex].enableLighting);

		// ディフューズ有効化
		meshMaterialResources_[meshIndex]->data_->enableDiffuse = static_cast<int32_t>(param_->meshMaterial[meshIndex].enableDiffuse);

		// ハーフランバード有効化
		meshMaterialResources_[meshIndex]->data_->enableHalfLambert = static_cast<int32_t>(param_->meshMaterial[meshIndex].enableHalfLambert);

		// スペキュラー有効化
		meshMaterialResources_[meshIndex]->data_->enableSpecular = static_cast<int32_t>(param_->meshMaterial[meshIndex].enableSpecular);

		// ブリンフォン有効化
		meshMaterialResources_[meshIndex]->data_->enableBlinnPhong = static_cast<int32_t>(param_->meshMaterial[meshIndex].enableBlinnPhong);

		// シャドウ有効化
		meshMaterialResources_[meshIndex]->data_->enableShadow = static_cast<int32_t>(param_->meshMaterial[meshIndex].enableShadow);


		// ブラー
		motionVectorResources_[meshIndex]->data_->afterImageMask = param_->meshBlur[meshIndex].afterImageMask;
		motionVectorResources_[meshIndex]->data_->motionBlurMask = param_->meshBlur[meshIndex].motionBlurMask;


		/*------------------------
		    コマンドリストに登録
		------------------------*/

		// 頂点の設定
		modelStore_->Register(commandList, hModel_, meshIndex);

		// 座標変換の設定
		meshTransformationResources_[meshIndex]->RegisterGraphics(commandList, 0);

		// マテリアルの設定
		meshMaterialResources_[meshIndex]->RegisterGraphics(commandList, 1);

		// テクスチャの設定
		commandList->SetGraphicsRootDescriptorTable(2, textureStore_->GetSrvGpuHandle(param_->meshMaterial[meshIndex].hTexture));

		// シャドウマップテクスチャの設定
		lightStore_->GetShadowMapTextureResource()->Register(commandList, 3);

		// シャドウ用座標変換の設定
		lightStore_->GetShadowMapTransformationResource()->RegisterGraphics(commandList, 4);

		// 形状の設定
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// ドローコール
		commandList->DrawIndexedInstanced(static_cast<UINT>(modelStore_->GetModelData(hModel_).meshes[meshIndex].indices.size()), 1, 0, 0, 0);
	}

	// 描画した
	isDrew_ = true;
}

/// @brief コマンドリスト
/// @param commandList 
/// @param pso 
void Engine::Render3DStaticModelData::Register(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOShadowMap* pso)
{
	// 読み込まれていないときは処理しない
	if (!isLoad_)return;

	// 直前で描画されているときのみ
	if (!IsDrew())return;

	// モデルデータを取得する
	const ModelData& modelData = modelStore_->GetModelData(hModel_);

	Quaternion modelQuaternion =
		ToQuaternion(param_->modelTransform.rotate.z, Vector3(0.0f, 0.0, 1.0f)).Normalize() *
		ToQuaternion(param_->modelTransform.rotate.y, Vector3(0.0f, 1.0, 0.0f)).Normalize() *
		ToQuaternion(param_->modelTransform.rotate.x, Vector3(1.0f, 0.0, 0.0f)).Normalize();

	Matrix4x4 worldMatrix = Make3DAffineMatrix4x4(param_->modelTransform.scale, modelQuaternion, param_->modelTransform.translate);
	if (parent_)worldMatrix = worldMatrix * parent_->GetWorldMatrix();



	// PSOの設定
	pso->Register(commandList);

	for (int32_t meshIndex = 0; meshIndex < static_cast<int32_t>(modelStore_->GetModelData(hModel_).meshes.size()); meshIndex++)
	{
		// シャドウマップを描画しないときは処理しない
		if (!param_->meshMaterial[meshIndex].drawShadowMap)
			continue;

		/*-----------------
		    データを渡す
		-----------------*/

		// ノード行列
		Matrix4x4 nodeMatrix = MakeIdentityMatrix4x4();
		if (!modelData.nodes.empty())nodeMatrix = modelData.nodes[static_cast<int32_t>(modelStore_->GetModelData(hModel_).meshes.size()) - 1 - meshIndex].worldMatrix;

		Quaternion meshQuaternion =
			ToQuaternion(param_->meshTransforms[meshIndex].rotate.z, Vector3(0.0f, 0.0, 1.0f)).Normalize() *
			ToQuaternion(param_->meshTransforms[meshIndex].rotate.y, Vector3(0.0f, 1.0, 0.0f)).Normalize() *
			ToQuaternion(param_->meshTransforms[meshIndex].rotate.x, Vector3(1.0f, 0.0, 0.0f)).Normalize();

		Matrix4x4 localMatrix = Make3DAffineMatrix4x4(param_->meshTransforms[meshIndex].scale, meshQuaternion, param_->meshTransforms[meshIndex].translate);


		// ワールド座標
		*shadowMapTransformationResource_[meshIndex]->data_ = localMatrix * nodeMatrix * worldMatrix * viewProjection;


		/*------------------------
		    コマンドリストに登録
		------------------------*/

		// 頂点の設定
		modelStore_->Register(commandList, hModel_, meshIndex);

		// 座標変換の設定
		shadowMapTransformationResource_[meshIndex]->RegisterGraphics(commandList, 0);

		// 形状の設定
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// ドローコール
		commandList->DrawIndexedInstanced(static_cast<UINT>(modelStore_->GetModelData(hModel_).meshes[meshIndex].indices.size()), 1, 0, 0, 0);
	}
}

/// @brief モーションベクトルをコマンドリストに登録する
/// @param commandList 
/// @param pso 
void Engine::Render3DStaticModelData::RegisterMotionVector(ID3D12GraphicsCommandList* commandList, BasePSOMotionVector* pso)
{
	// 読み込まれていないときは処理しない
	if (!isLoad_)return;

	// 直前で描画されているときのみ
	if (!isDrew_)return;

	// PSOの設定
	pso->Register(commandList);

	for (int32_t meshIndex = 0; meshIndex < static_cast<int32_t>(modelStore_->GetModelData(hModel_).meshes.size()); meshIndex++)
	{
		// 頂点の設定
		modelStore_->Register(commandList, hModel_, meshIndex);

		// モーションベクター用座標変換の設定
		motionVectorResources_[meshIndex]->RegisterGraphics(commandList, 0);

		// 形状の設定
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// ドローコール
		commandList->DrawIndexedInstanced(static_cast<UINT>(modelStore_->GetModelData(hModel_).meshes[meshIndex].indices.size()), 1, 0, 0, 0);
	}
}

/// @brief アウトライン用のコマンドリストに登録
/// @param commandList 
/// @param cameraStore 
/// @param pso 
void Engine::Render3DStaticModelData::RegisterOutline(ID3D12GraphicsCommandList* commandList, BasePSOOutline* pso)
{
	// 読み込まれていないときは処理しない
	if (!isLoad_)return;

	// 直前で描画されているときのみ
	if (!isDrew_)return;

	// PSOの設定
	pso->Register(commandList);

	for (int32_t meshIndex = 0; meshIndex < static_cast<int32_t>(modelStore_->GetModelData(hModel_).meshes.size()); meshIndex++)
	{
		// アウトラインを描画しないときは処理しない
		if (!param_->meshOutline[meshIndex].enableOutline)continue;

		// データを渡す
		*outlineTransformationResources_[meshIndex]->data_ = meshTransformationResources_[meshIndex]->data_->worldViewProjectionMatrix;
		*outlineColorResources_[meshIndex]->data_ = param_->meshOutline[meshIndex].color;

		// 頂点の設定
		modelStore_->Register(commandList, hModel_, meshIndex);

		// アウトライン座標変換の設定
		outlineTransformationResources_[meshIndex]->RegisterGraphics(commandList, 0);

		// アウトライン色の設定
		outlineColorResources_[meshIndex]->RegisterGraphics(commandList, 1);

		// 形状の設定
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// ドローコール
		commandList->DrawIndexedInstanced(static_cast<UINT>(modelStore_->GetModelData(hModel_).meshes[meshIndex].indices.size()), 1, 0, 0, 0);
	}
}
