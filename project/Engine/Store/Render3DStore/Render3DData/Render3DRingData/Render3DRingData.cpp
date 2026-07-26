#include "Render3DRingData.h"
#include <cassert>
#include <algorithm>
#include <numbers>

#include "GrowthEngine.h"

/// @brief 初期化
/// @param textureStore 
/// @param lightStore 
/// @param device 
/// @param commandList 
/// @param log 
void Engine::Render3DRingData::Initialize(TextureStore* textureStore, LightStore* lightStore,
	ID3D12Device* device, Log* log)
{
	// nullptrチェック
	assert(textureStore);
	assert(lightStore);
	assert(device);

	// 引数を受け取る
	textureStore_ = textureStore;
	lightStore_ = lightStore;


	// パラメータの生成
	param_ = std::make_unique<Render3D::Ring::Param>();

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

	// サイズ
	param_->size.startInRadius = 0.5f;
	param_->size.startOutRadius = 1.0f;
	param_->size.startAngle = 0.0f;
	param_->size.endAngle = std::numbers::pi_v<float> *2.0f;
	param_->size.endInRadius = 0.5f;
	param_->size.endOutRadius = 1.0f;

	// ブラー
	param_->blur.afterImageMask = 0.0f;
	param_->blur.motionBlurMask = 0.0f;

	// アウトライン
	param_->outline.enableOutline = false;
	param_->outline.color = Vector4(0.0f, 0.0f, 0.0f, 1.0f);

	// テクスチャ
	param_->material.hTexture = hTexture_;
	textureFilePath_ = textureStore_->GetFilePath(param_->material.hTexture);


	// 頂点リソースの生成
	vertexResource_ = std::make_unique<VertexBufferResource<VertexDataForGPU>>();
	vertexResource_->Initialize(device, kMaxSlices * 4, log);

	// インデックスリソースの生成
	indexResource_ = std::make_unique<IndexBufferResource>();
	indexResource_->Initialize(device, kMaxSlices * 6, log);

	// 頂点の計算
	VertexCalculate();


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
}

/// @brief 更新処理
void Engine::Render3DRingData::Update()
{
	// 描画を記録する
	isPreDrew_ = isDrew_;
	isDrew_ = false;
}

/// @brief リセット
void Engine::Render3DRingData::Reset()
{
	// 頂点の計算
	VertexCalculate();

	// 読み込み
	isLoad_ = true;
}

/// @brief コマンドリストに登録する
/// @param commandList 
/// @param pso 
/// @param textureStore 
void Engine::Render3DRingData::Register(Camera3DStore* cameraStore, SkyboxStore* skyboxStore, ID3D12GraphicsCommandList* commandList, BasePSOModel* pso)
{
	// 読み込まれていないときは処理しない
	if (!isLoad_)return;

	// 頂点の再計算
	if (preSlices_ != param_->division.slices || preStartInRadius_ != param_->size.startInRadius ||
		preStartOutRadius_ != param_->size.startOutRadius || preStartAngle_ != param_->size.startAngle ||
		preEndAngle_ != param_->size.endAngle || preEndInRadius_ != param_->size.endInRadius ||
		preEndOutRadius_ != param_->size.endOutRadius)
		VertexCalculate();


	Quaternion modelQuaternion =
		ToQuaternion(param_->transform.rotate.z, Vector3(0.0f, 0.0, 1.0f)).Normalize() *
		ToQuaternion(param_->transform.rotate.y, Vector3(0.0f, 1.0, 0.0f)).Normalize() *
		ToQuaternion(param_->transform.rotate.x, Vector3(1.0f, 0.0, 0.0f)).Normalize();

	Matrix4x4 worldMatrix = Make3DAffineMatrix4x4(param_->transform.scale, modelQuaternion, param_->transform.translate);
	if (parent_)
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

	// インデックスの設定
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
	commandList->DrawIndexedInstanced(param_->division.slices * 6, 1, 0, 0, 0);


	// 描画した
	isDrew_ = true;
}

/// @brief コマンドリスト
/// @param commandList 
/// @param pso 
void Engine::Render3DRingData::Register(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOShadowMap* pso)
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

	// インデックスの設定
	indexResource_->Register(commandList);

	// 座標変換の設定
	shadowMapTransformationResource_->RegisterGraphics(commandList, 0);

	// 形状の設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ドローコール
	commandList->DrawIndexedInstanced(preSlices_ * 6, 1, 0, 0, 0);
}

/// @brief コマンドリストに登録
/// @param commandList 
/// @param pso 
void Engine::Render3DRingData::RegisterMotionVector(ID3D12GraphicsCommandList* commandList, BasePSOMotionVector* pso)
{
	// 読み込まれていないときは処理しない
	if (!isLoad_)return;

	// 今フレーム描画していないと処理しない
	if (!isDrew_)return;

	// PSOの設定
	pso->Register(commandList);

	// 頂点の設定
	vertexResource_->Register(commandList);

	// インデックスの設定
	indexResource_->Register(commandList);

	// モーションベクトルの設定
	motionVectorResource_->RegisterGraphics(commandList, 0);

	// 形状の設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ドローコール
	commandList->DrawIndexedInstanced(preSlices_ * 6, 1, 0, 0, 0);
}


/// @brief アウトライン用のコマンドリストに登録
/// @param commandList 
/// @param cameraStore 
/// @param pso 
void Engine::Render3DRingData::RegisterOutline(ID3D12GraphicsCommandList* commandList, BasePSOOutline* pso)
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
	commandList->DrawIndexedInstanced(preSlices_ * 6, 1, 0, 0, 0);
}


/// @brief 頂点計算
void Engine::Render3DRingData::VertexCalculate()
{
	// スライスごとの角度の計算
	const float kRadianPerDivide = (param_->size.endAngle - param_->size.startAngle) / static_cast<float>(param_->division.slices);

	for (int i = 0; i < param_->division.slices; ++i)
	{
		/*---------------------
		    インデックス計算
		---------------------*/

		int startIndex = i * 6;
		int vertexOffset = i * 4;

		indexResource_->data_[startIndex + 0] = vertexOffset + 0;
		indexResource_->data_[startIndex + 1] = vertexOffset + 1;
		indexResource_->data_[startIndex + 2] = vertexOffset + 2;
		indexResource_->data_[startIndex + 3] = vertexOffset + 2;
		indexResource_->data_[startIndex + 4] = vertexOffset + 1;
		indexResource_->data_[startIndex + 5] = vertexOffset + 3;



		/*--------------
		    頂点計算
		--------------*/

		float currentAngle = param_->size.startAngle + i * kRadianPerDivide;
		float nextAngle = param_->size.startAngle + (i + 1) * kRadianPerDivide;

		// 最後のスライスで、全体の角度が2πに近い場合は、誤差を考慮して次の角度をスタート角度にする
		if (i == param_->division.slices - 1)
		{
			float totalAngle = param_->size.endAngle - param_->size.startAngle;
			// 誤差を考慮して 2π と比較
			if (std::abs(totalAngle - (std::numbers::pi_v<float> *2.0f)) < 0.0001f)
			{
				nextAngle = param_->size.startAngle;
			}
		}

		// 角度からサインとコサインを計算
		float sin = std::sin(currentAngle);
		float cos = std::cos(currentAngle);
		float sinNext = std::sin(nextAngle);
		float cosNext = std::cos(nextAngle);

		// UV座標の計算
		float u = float(i) / static_cast<float>(param_->division.slices);
		float uNext = float(i + 1) / static_cast<float>(param_->division.slices);

		// 内半径と外半径の線形補間
		float t = float(i) / static_cast<float>(param_->division.slices);
		float tNext = float(i + 1) / static_cast<float>(param_->division.slices);

		// 内半径と外半径の線形補間
		float currentInRadius = (1.0f - t) * param_->size.startInRadius + t * param_->size.endInRadius;
		float currentOutRadius = (1.0f - t) * param_->size.startOutRadius + t * param_->size.endOutRadius;

		// 次のスライスの内半径と外半径の線形補間
		float nextInRadius = (1.0f - tNext) * param_->size.startInRadius + tNext * param_->size.endInRadius;
		float nextOutRadius = (1.0f - tNext) * param_->size.startOutRadius + tNext * param_->size.endOutRadius;

		vertexResource_->data_[vertexOffset].position = Vector4(-sin * currentOutRadius, cos * currentOutRadius, 0.0f, 1.0f);
		vertexResource_->data_[vertexOffset].texcoord = Vector2(u, 0.0f);
		vertexResource_->data_[vertexOffset].normal = Vector3(0.0f, 0.0f, 1.0f);

		vertexResource_->data_[vertexOffset + 1].position = Vector4(-sinNext * nextOutRadius, cosNext * nextOutRadius, 0.0f, 1.0f);
		vertexResource_->data_[vertexOffset + 1].texcoord = Vector2(uNext, 0.0f);
		vertexResource_->data_[vertexOffset + 1].normal = Vector3(0.0f, 0.0f, 1.0f);

		vertexResource_->data_[vertexOffset + 2].position = Vector4(-sin * currentInRadius, cos * currentInRadius, 0.0f, 1.0f);
		vertexResource_->data_[vertexOffset + 2].texcoord = Vector2(u, 1.0f);
		vertexResource_->data_[vertexOffset + 2].normal = Vector3(0.0f, 0.0f, 1.0f);

		vertexResource_->data_[vertexOffset + 3].position = Vector4(-sinNext * nextInRadius, cosNext * nextInRadius, 0.0f, 1.0f);
		vertexResource_->data_[vertexOffset + 3].texcoord = Vector2(uNext, 1.0f);
		vertexResource_->data_[vertexOffset + 3].normal = Vector3(0.0f, 0.0f, 1.0f);
	}

	// 分割数を記録する
	preSlices_ = param_->division.slices;
	preStartInRadius_ = param_->size.startInRadius;
	preStartOutRadius_ = param_->size.startOutRadius;
	preStartAngle_ = param_->size.startAngle;
	preEndAngle_ = param_->size.endAngle;
	preEndInRadius_ = param_->size.endInRadius;
	preEndOutRadius_ = param_->size.endOutRadius;
}
