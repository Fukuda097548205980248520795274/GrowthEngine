#include "Render3DUVSphereData.h"
#include <cassert>
#include <algorithm>
#include <numbers>

#include "GrowthEngine.h"

/// @brief 初期化
/// @param modelStore 
/// @param device 
void Engine::Render3DUVSphereData::Initialize(TextureStore* textureStore, LightStore* lightStore, ID3D12Device* device, Log* log)
{
	// nullptrチェック
	assert(textureStore);
	assert(lightStore);
	assert(device);

	// 引数を受け取る
	textureStore_ = textureStore;
	lightStore_ = lightStore;


	// パラメータの生成
	param_ = std::make_unique<Render3D::UVSphere::Param>();

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
	param_->material.environment = 0.0f;
	param_->material.shininess = 10.0f;
	param_->material.enableLighting = true;
	param_->material.enableDiffuse = true;
	param_->material.enableHalfLambert = true;
	param_->material.enableSpecular = true;
	param_->material.enableBlinnPhong = true;
	param_->material.drawShadowMap = true;
	param_->material.enableShadow = true;

	// 分割
	param_->division.slices = 32;
	param_->division.rings = 16;

	// ブラー
	param_->blur.afterImageMask = 0.0f;
	param_->blur.motionBlurMask = 0.0f;

	// アウトライン
	param_->outline.enableOutline = false;
	param_->outline.color = Vector4(0.0f, 0.0f, 0.0f, 1.0f);

	// テクスチャ
	param_->material.hTexture = engine_->LoadTexture("./Assets/Textures/white2x2.png");

	// 頂点リソースの生成
	vertexResource_ = std::make_unique<VertexBufferResource<VertexDataForGPU>>();
	vertexResource_->Initialize(device, kMaxSlices * kMaxRings * 4, log);

	// インデックスリソースの生成
	indexResource_ = std::make_unique<IndexBufferResource>();
	indexResource_->Initialize(device, kMaxSlices * kMaxRings * 6, log);

	// 分割リソースの生成
	divisionResource_ = std::make_unique<ConstantBufferResource<PrimitiveDataForGPU::UVSphereDivisionDataForGPU>>();
	divisionResource_->Initialize(device, log);

	// 座標変換リソースの生成
	transformationResources_ = std::make_unique<ConstantBufferResource<PrimitiveModelTransformationDataForGPU>>();
	transformationResources_->Initialize(device, log);
	transformationResources_->data_->worldMatrix = MakeIdentityMatrix4x4();

	// マテリアルリソースの生成
	materialResources_ = std::make_unique<ConstantBufferResource<PrimitiveModelMaterialDataForGPU>>();
	materialResources_->Initialize(device, log);

	// シャドウマップ用座標変換リソースの生成
	shadowMapTransformationResource_ = std::make_unique<ConstantBufferResource<Matrix4x4>>();
	shadowMapTransformationResource_->Initialize(device, log);

	// モーションベクトルリソースの生成
	motionVectorResource_ = std::make_unique<ConstantBufferResource<MotionVectorDataForGPU>>();
	motionVectorResource_->Initialize(device, log);

	// アウトライン用座標変換リソースの生成
	outlineTransformationResource_ = std::make_unique<ConstantBufferResource<Matrix4x4>>();
	outlineTransformationResource_->Initialize(device, log);

	// アウトライン用色リソースの生成
	outlineColorResource_ = std::make_unique<ConstantBufferResource<Vector4>>();
	outlineColorResource_->Initialize(device, log);


	// 頂点計算
	VertexCalculation();
}

/// @brief 更新処理
void Engine::Render3DUVSphereData::Update()
{
	// 描画を記録する
	isPreDrew_ = isDrew_;
	isDrew_ = false;
}

/// @brief リセット
void Engine::Render3DUVSphereData::Reset()
{
	// 読み込み
	isLoad_ = true;

	// 頂点計算
	VertexCalculation();
}

/// @brief コマンドリストに登録する
/// @param commandList 
/// @param pso 
/// @param textureStore 
void Engine::Render3DUVSphereData::Register(Camera3DStore* cameraStore, SkyboxStore* skyboxStore, ID3D12GraphicsCommandList* commandList, BasePSOModel* pso)
{
	// 読み込まれていないときは処理しない
	if (!isLoad_)return;

	// 頂点計算
	if (preSlices_ != param_->division.slices || preRings_ != param_->division.rings)
		VertexCalculation();


	Quaternion modelQuaternion =
		ToQuaternion(param_->transform.rotate.z, Vector3(0.0f, 0.0, 1.0f)).Normalize() *
		ToQuaternion(param_->transform.rotate.y, Vector3(0.0f, 1.0, 0.0f)).Normalize() *
		ToQuaternion(param_->transform.rotate.x, Vector3(1.0f, 0.0, 0.0f)).Normalize();

	Matrix4x4 worldMatrix = Make3DAffineMatrix4x4(param_->transform.scale, modelQuaternion, param_->transform.translate);
	if(parent_)
		worldMatrix = worldMatrix * parent_->GetWorldMatrix();

	// ビュープロジェクション行列を取得する
	Matrix4x4 viewProjection = cameraStore->GetCamera3D().GetCurrentVPMatrix();
	Matrix4x4 prevVPUnJitter = cameraStore->GetCamera3D().GetPrevVPUnJitterMatrix();
	Matrix4x4 currentVPUnJitter = cameraStore->GetCamera3D().GetCurrentVPUnJitterMatrix();


	/*------------------------
	   コマンドリストに登録する
	------------------------*/

	// PSOの設定
	pso->Register(commandList, param_->blendMode);

	// カメラリソースの設定
	cameraStore->RegisterCameraResource(commandList, 5);

	// スカイボックスの設定
	skyboxStore->RegisterCubeMapTexture(commandList, 6);

	// ライトの設定
	lightStore_->LightRegister(commandList, 7, 8, 9, 10);

	// 前フレームのWVP行列
	motionVectorResource_->data_->prevWVPMatrix = transformationResources_->data_->worldMatrix * prevVPUnJitter;

	// ワールド座標
	transformationResources_->data_->worldMatrix = worldMatrix;

	// 現フレームのWVP行列
	motionVectorResource_->data_->currentWVPMatrix = transformationResources_->data_->worldMatrix * currentVPUnJitter;

	// ワールドビュー正射影行列
	transformationResources_->data_->worldViewProjectionMatrix =
		transformationResources_->data_->worldMatrix * viewProjection;
	// 逆転置ワールド行列
	transformationResources_->data_->worldInverseTransposeMatrix =
		transformationResources_->data_->worldMatrix.Transpose().Inverse();



	// 色
	materialResources_->data_->color = param_->material.color;

	// 環境
	materialResources_->data_->environment = param_->material.environment;

	// 光沢度
	materialResources_->data_->shininess = param_->material.shininess;

	// UV行列
	materialResources_->data_->uvMatrix =
		Make3DScaleMatrix4x4(Vector3(param_->material.uv.scale.x, param_->material.uv.scale.y, 1.0f)) *
		Make3DRotateZMatrix4x4(param_->material.uv.radius) *
		Make3DTranslateMatrix4x4(Vector3(param_->material.uv.translate.x, param_->material.uv.translate.y, 0.0f));

	// ライティング有効化
	materialResources_->data_->enableLighting = static_cast<int32_t>(param_->material.enableLighting);

	// ディフューズ有効化
	materialResources_->data_->enableDiffuse = static_cast<int32_t>(param_->material.enableDiffuse);

	// ハーフランバード有効化
	materialResources_->data_->enableHalfLambert = static_cast<int32_t>(param_->material.enableHalfLambert);

	// スペキュラー有効化
	materialResources_->data_->enableSpecular = static_cast<int32_t>(param_->material.enableSpecular);

	// ブリンフォン有効化
	materialResources_->data_->enableBlinnPhong = static_cast<int32_t>(param_->material.enableBlinnPhong);

	// シャドウ有効化
	materialResources_->data_->enableShadow = static_cast<int32_t>(param_->material.enableShadow);


	// ブラー
	motionVectorResource_->data_->afterImageMask = param_->blur.afterImageMask;
	motionVectorResource_->data_->motionBlurMask = param_->blur.motionBlurMask;

	/*------------------------
		コマンドリストに登録
	------------------------*/

	// 頂点の設定
	vertexResource_->Register(commandList);
	indexResource_->Register(commandList);

	// 座標変換の設定
	transformationResources_->RegisterGraphics(commandList, 0);

	// マテリアルの設定
	materialResources_->RegisterGraphics(commandList, 1);

	// テクスチャの設定
	commandList->SetGraphicsRootDescriptorTable(2, textureStore_->GetSrvGpuHandle(param_->material.hTexture));

	// シャドウマップテクスチャの設定
	lightStore_->GetShadowMapTextureResource()->Register(commandList, 3);

	// シャドウ用座標変換の設定
	lightStore_->GetShadowMapTransformationResource()->RegisterGraphics(commandList, 4);

	// 形状の設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ドローコール
	commandList->DrawIndexedInstanced(param_->division.slices * param_->division.rings * 6, 1, 0, 0, 0);


	// 描画した
	isDrew_ = true;
}

/// @brief コマンドリスト
/// @param commandList 
/// @param pso 
void Engine::Render3DUVSphereData::Register(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOShadowMap* pso)
{
	// 読み込まれていないときは処理しない
	if (!isLoad_)return;

	// シャドウマップを描画しないときは処理しない
	if (!param_->material.drawShadowMap)return;

	// 直前で描画されているときのみ
	if (!IsDrew())return;

	Quaternion modelQuaternion =
		ToQuaternion(param_->transform.rotate.z, Vector3(0.0f, 0.0, 1.0f)).Normalize() *
		ToQuaternion(param_->transform.rotate.y, Vector3(0.0f, 1.0, 0.0f)).Normalize() *
		ToQuaternion(param_->transform.rotate.x, Vector3(1.0f, 0.0, 0.0f)).Normalize();

	Matrix4x4 worldMatrix = Make3DAffineMatrix4x4(param_->transform.scale, modelQuaternion, param_->transform.translate);

	if (parent_)
		worldMatrix = worldMatrix * parent_->GetWorldMatrix();



	// PSOの設定
	pso->Register(commandList);

	/*-----------------
		データを渡す
	-----------------*/


	// ワールド座標
	*shadowMapTransformationResource_->data_ = worldMatrix * viewProjection;


	/*------------------------
		コマンドリストに登録
	------------------------*/

	// 頂点の設定
	vertexResource_->Register(commandList);
	indexResource_->Register(commandList);

	// 座標変換の設定
	shadowMapTransformationResource_->RegisterGraphics(commandList, 0);

	// 形状の設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ドローコール
	commandList->DrawIndexedInstanced(preSlices_ * preRings_ * 6, 1, 0, 0, 0);
}

/// @brief コマンドリストに登録
/// @param commandList 
/// @param pso 
void Engine::Render3DUVSphereData::RegisterMotionVector(ID3D12GraphicsCommandList* commandList, BasePSOMotionVector* pso)
{
	// 読み込まれていないときは処理しない
	if (!isLoad_)return;

	// 今フレーム描画していないと処理しない
	if (!isDrew_)return;

	// PSOの設定
	pso->Register(commandList);

	// 頂点の設定
	vertexResource_->Register(commandList);
	indexResource_->Register(commandList);

	// モーションベクトルの設定
	motionVectorResource_->RegisterGraphics(commandList, 0);

	// 形状の設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ドローコール
	commandList->DrawIndexedInstanced(preSlices_ * preRings_ * 6, 1, 0, 0, 0);
}


/// @brief アウトライン用のコマンドリストに登録
/// @param commandList 
/// @param cameraStore 
/// @param pso 
void Engine::Render3DUVSphereData::RegisterOutline(ID3D12GraphicsCommandList* commandList, BasePSOOutline* pso)
{
	// 読み込まれていないときは処理しない
	if (!isLoad_)return;

	// アウトラインを描画しないときは処理しない
	if (!param_->outline.enableOutline)return;

	// 今フレーム描画していないと処理しない
	if (!isDrew_)return;

	// データを渡す
	*outlineTransformationResource_->data_ = transformationResources_->data_->worldViewProjectionMatrix;
	*outlineColorResource_->data_ = param_->outline.color;

	/*-----------------------------
		コマンドリストに登録する
	-----------------------------*/

	// PSOの設定
	pso->Register(commandList);

	// 頂点の設定
	vertexResource_->Register(commandList);
	indexResource_->Register(commandList);

	// 座標変換の設定
	outlineTransformationResource_->RegisterGraphics(commandList, 0);

	// 色の設定
	outlineColorResource_->RegisterGraphics(commandList, 1);

	// 形状の設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ドローコール
	commandList->DrawIndexedInstanced(preSlices_ * preRings_ * 6, 1, 0, 0, 0);
}


/// @brief 頂点計算
void Engine::Render3DUVSphereData::VertexCalculation()
{
	// 経度と緯度の1区切りの角度を計算
	float kLonEvery = std::numbers::pi_v<float> *2.0f / float(param_->division.slices);
	float kLatEvery = std::numbers::pi_v<float> / float(param_->division.rings);

	// インデックス計算
	for (int32_t latIndex = 0; latIndex < param_->division.rings; ++latIndex)
	{
		// 緯度を計算
		float lat = -std::numbers::pi_v<float> / 2.0f + kLatEvery * latIndex;

		for (int32_t lonIndex = 0; lonIndex < param_->division.slices; ++lonIndex)
		{
			/*--------------------
			    インデックス計算
			---------------------*/

			int startIndex = (latIndex * param_->division.slices + lonIndex) * 6;
			int index = (latIndex * param_->division.slices + lonIndex) * 4;

			indexResource_->data_[startIndex] = index;
			indexResource_->data_[startIndex + 1] = index + 1;
			indexResource_->data_[startIndex + 2] = index + 2;
			indexResource_->data_[startIndex + 3] = index + 1;
			indexResource_->data_[startIndex + 4] = index + 3;
			indexResource_->data_[startIndex + 5] = index + 2;


			/*--------------
			    頂点計算
			--------------*/

			// 経度を計算
			float lon = kLonEvery * lonIndex;

			vertexResource_->data_[index].position.x = std::cos(lat) * std::cos(lon);
			vertexResource_->data_[index].position.y = std::sin(lat);
			vertexResource_->data_[index].position.z = std::cos(lat) * std::sin(lon);
			vertexResource_->data_[index].position.w = 1.0f;
			vertexResource_->data_[index].texcoord.x = float(lonIndex) / float(param_->division.slices);
			vertexResource_->data_[index].texcoord.y = 1.0f - (float(latIndex) / float(param_->division.rings));
			vertexResource_->data_[index].normal.x = vertexResource_->data_[index].position.x;
			vertexResource_->data_[index].normal.y = vertexResource_->data_[index].position.y;
			vertexResource_->data_[index].normal.z = vertexResource_->data_[index].position.z;

			vertexResource_->data_[index + 1].position.x = std::cos(lat + kLatEvery) * std::cos(lon);
			vertexResource_->data_[index + 1].position.y = std::sin(lat + kLatEvery);
			vertexResource_->data_[index + 1].position.z = std::cos(lat + kLatEvery) * std::sin(lon);
			vertexResource_->data_[index + 1].position.w = 1.0f;
			vertexResource_->data_[index + 1].texcoord.x = float(lonIndex) / float(param_->division.slices);
			vertexResource_->data_[index + 1].texcoord.y = 1.0f - (float(latIndex + 1) / float(param_->division.rings));
			vertexResource_->data_[index + 1].normal.x = vertexResource_->data_[index + 1].position.x;
			vertexResource_->data_[index + 1].normal.y = vertexResource_->data_[index + 1].position.y;
			vertexResource_->data_[index + 1].normal.z = vertexResource_->data_[index + 1].position.z;

			vertexResource_->data_[index + 2].position.x = std::cos(lat) * std::cos(lon + kLonEvery);
			vertexResource_->data_[index + 2].position.y = std::sin(lat);
			vertexResource_->data_[index + 2].position.z = std::cos(lat) * std::sin(lon + kLonEvery);
			vertexResource_->data_[index + 2].position.w = 1.0f;
			vertexResource_->data_[index + 2].texcoord.x = float(lonIndex + 1) / float(param_->division.slices);
			vertexResource_->data_[index + 2].texcoord.y = 1.0f - (float(latIndex) / float(param_->division.rings));
			vertexResource_->data_[index + 2].normal.x = vertexResource_->data_[index + 2].position.x;
			vertexResource_->data_[index + 2].normal.y = vertexResource_->data_[index + 2].position.y;
			vertexResource_->data_[index + 2].normal.z = vertexResource_->data_[index + 2].position.z;

			vertexResource_->data_[index + 3].position.x = std::cos(lat + kLatEvery) * std::cos(lon + kLonEvery);
			vertexResource_->data_[index + 3].position.y = std::sin(lat + kLatEvery);
			vertexResource_->data_[index + 3].position.z = std::cos(lat + kLatEvery) * std::sin(lon + kLonEvery);
			vertexResource_->data_[index + 3].position.w = 1.0f;
			vertexResource_->data_[index + 3].texcoord.x = float(lonIndex + 1) / float(param_->division.slices);
			vertexResource_->data_[index + 3].texcoord.y = 1.0f - (float(latIndex + 1) / float(param_->division.rings));
			vertexResource_->data_[index + 3].normal.x = vertexResource_->data_[index + 3].position.x;
			vertexResource_->data_[index + 3].normal.y = vertexResource_->data_[index + 3].position.y;
			vertexResource_->data_[index + 3].normal.z = vertexResource_->data_[index + 3].position.z;
		}
	}

	// 分割数を記録する
	preRings_ = param_->division.rings;
	preSlices_ = param_->division.slices;
}
