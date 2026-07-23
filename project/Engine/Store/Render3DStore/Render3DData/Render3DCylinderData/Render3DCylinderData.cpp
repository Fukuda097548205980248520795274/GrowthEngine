#include "Render3DCylinderData.h"
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
void Engine::Render3DCylinderData::Initialize(TextureStore* textureStore, LightStore* lightStore,
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
	param_ = std::make_unique<Render3D::Cylinder::Param>();

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
	param_->size.topRadius = 1.0f;
	param_->size.bottomRadius = 1.0f;
	param_->size.height = 1.0f;

	// ブラー
	param_->blur.afterImageMask = 0.0f;
	param_->blur.motionBlurMask = 0.0f;

	// アウトライン
	param_->outline.enableOutline = false;
	param_->outline.color = Vector4(0.0f, 0.0f, 0.0f, 1.0f);

	// テクスチャ
	param_->material.hTexture = hTexture_;
	textureFilePath_ = textureStore_->GetFilePath(param_->material.hTexture);


	// パラメータの記録
	group_ = "Cylinder_" + name_;
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
		parameter_->SetValue(group_, "Material_Draw_ShadowMap", &param_->material.drawShadowMap);
		parameter_->SetValue(group_, "Material_Enable_Shadow", &param_->material.enableShadow);
		parameter_->SetValue(group_, "Material_Texture", &textureFilePath_);
		parameter_->SetValue(group_, "Division_Slices", &param_->division.slices);
		parameter_->SetValue(group_, "Size_TopRadius", &param_->size.topRadius);
		parameter_->SetValue(group_, "Size_BottomRadius", &param_->size.bottomRadius);
		parameter_->SetValue(group_, "Size_Height", &param_->size.height);
		parameter_->SetValue(group_, "Blur_AfterImageMask", &param_->blur.afterImageMask);
		parameter_->SetValue(group_, "Blur_MotionBlurMask", &param_->blur.motionBlurMask);
		parameter_->SetValue(group_, "Outline_Enable", &param_->outline.enableOutline);
		parameter_->SetValue(group_, "Outline_Color", &param_->outline.color);

		// 値を反映させる
		parameter_->RegisterGroupDataReflection(group_);
		param_->material.hTexture = textureStore_->GetHandle(textureFilePath_);
	}

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
void Engine::Render3DCylinderData::Update()
{
	// 描画を記録する
	isPreDrew_ = isDrew_;
	isDrew_ = false;
}

/// @brief リセット
void Engine::Render3DCylinderData::Reset()
{
	// jsonファイルがあるかどうか
	if (parameter_->IsFileFound(group_))
	{
		// ファイルがあるとき

		// 値を反映させる
		parameter_->RegisterGroupDataReflection(group_);
		param_->material.hTexture = textureStore_->GetHandle(textureFilePath_);
	}
	else
	{
		// ファイルがないとき

		// ブレンドモード
		param_->blendMode = BlendMode::kNone;

		// トランスフォーム
		param_->transform.scale = Vector3(1.0f, 1.0f, 1.0f);
		param_->transform.rotate = Vector3(0.0f, 0.0f, 0.0f);
		param_->transform.translate = Vector3(0.0f, 0.0f, 0.0f);

		// マテリアル
		param_->material.hTexture = hTexture_;
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
		param_->size.topRadius = 1.0f;
		param_->size.bottomRadius = 1.0f;
		param_->size.height = 1.0f;

		// ブラー
		param_->blur.afterImageMask = 0.0f;
		param_->blur.motionBlurMask = 0.0f;

		// アウトライン
		param_->outline.enableOutline = false;
		param_->outline.color = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
	}

	// 頂点の計算
	VertexCalculate();

	// 読み込み
	isLoad_ = true;
}

/// @brief コマンドリストに登録する
/// @param commandList 
/// @param pso 
/// @param textureStore 
void Engine::Render3DCylinderData::Register(Camera3DStore* cameraStore, SkyboxStore* skyboxStore, ID3D12GraphicsCommandList* commandList, BasePSOModel* pso)
{
	// 読み込まれていないときは処理しない
	if (!isLoad_)return;

	// 頂点の再計算
	if (preSlices_ != param_->division.slices || preTopRadius_ != param_->size.topRadius || 
		preBottomRadius_ != param_->size.bottomRadius || preHeight_ != param_->size.height)
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
void Engine::Render3DCylinderData::Register(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOShadowMap* pso)
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
void Engine::Render3DCylinderData::RegisterMotionVector(ID3D12GraphicsCommandList* commandList, BasePSOMotionVector* pso)
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
void Engine::Render3DCylinderData::RegisterOutline(ID3D12GraphicsCommandList* commandList, BasePSOOutline* pso)
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
void Engine::Render3DCylinderData::VertexCalculate()
{
	const float radianPerDivid = 2.0f * std::numbers::pi_v<float> / static_cast<float>(param_->division.slices);

	// インデックスの計算
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

		// 円周上の位置を計算する
		float sin = std::sin(i * radianPerDivid);
		float cos = std::cos(i * radianPerDivid);
		float sinNext = std::sin((i + 1) * radianPerDivid);
		float cosNext = std::cos((i + 1) * radianPerDivid);

		// UVの計算
		float u = float(i) / static_cast<float>(param_->division.slices);
		float uNext = float(i + 1) / static_cast<float>(param_->division.slices);

		vertexResource_->data_[vertexOffset].position = Vector4(-sin * param_->size.topRadius, param_->size.height, cos * param_->size.topRadius, 1.0f);
		vertexResource_->data_[vertexOffset].texcoord = Vector2(u, 0.0f);
		vertexResource_->data_[vertexOffset].normal = Vector3(-sin, 0.0f, cos);

		vertexResource_->data_[vertexOffset + 1].position = Vector4(-sinNext * param_->size.topRadius, param_->size.height, cosNext * param_->size.topRadius, 1.0f);
		vertexResource_->data_[vertexOffset + 1].texcoord = Vector2(uNext, 0.0f);
		vertexResource_->data_[vertexOffset + 1].normal = Vector3(-sinNext, 0.0f, cosNext);

		vertexResource_->data_[vertexOffset + 2].position = Vector4(-sin * param_->size.bottomRadius, 0.0f, cos * param_->size.bottomRadius, 1.0f);
		vertexResource_->data_[vertexOffset + 2].texcoord = Vector2(u, 1.0f);
		vertexResource_->data_[vertexOffset + 2].normal = Vector3(-sin, 0.0f, cos);

		vertexResource_->data_[vertexOffset + 3].position = Vector4(-sinNext * param_->size.bottomRadius, 0.0f, cosNext * param_->size.bottomRadius, 1.0f);
		vertexResource_->data_[vertexOffset + 3].texcoord = Vector2(uNext, 1.0f);
		vertexResource_->data_[vertexOffset + 3].normal = Vector3(-sinNext, 0.0f, cosNext);
	}

	// 分割数を記録する
	preSlices_ = param_->division.slices;
	preTopRadius_ = param_->size.topRadius;
	preBottomRadius_ = param_->size.bottomRadius;
	preHeight_ = param_->size.height;
}


/// @brief デバッグ用パラメータ
void Engine::Render3DCylinderData::DebugParameter()
{
#ifdef DEVELOPMENT

	// 読み込んでいないと処理しない
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
		if (ImGui::TreeNode("Transform"))
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

			// ドラッグ＆ドロップの受け入れ
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TEXTURE_ID"))
				{
					int droppedIndex = *(const int*)payload->Data;

					// ドロップされたテクスチャを設定する
					param_->material.hTexture = static_cast<uint32_t>(droppedIndex);
					textureFilePath_ = textureStore_->GetFilePath(param_->material.hTexture);
				}
				ImGui::EndDragDropTarget();
			}



			ImGui::SeparatorText("Lighting");

			// ライティング有効化
			ImGui::Checkbox("Lighting", &param_->material.enableLighting);

			// シャドウマップを描画するか
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

			// 終了
			ImGui::TreePop();
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

		// 分割数
		if (ImGui::TreeNode("Division"))
		{
			// スライス
			ImGui::DragInt("Slices", &param_->division.slices, 1.0f, 3, kMaxSlices);

			// 終了
			ImGui::TreePop();
		}

		// サイズ
		if (ImGui::TreeNode("Size"))
		{
			// 上半径
			ImGui::DragFloat("TopRadius", &param_->size.topRadius, 0.01f, 0.0f, 100000.0f);

			// 下半径
			ImGui::DragFloat("BottomRadius", &param_->size.bottomRadius, 0.01f, 0.0f, 100000.0f);

			// 高さ
			ImGui::DragFloat("Height", &param_->size.height, 0.01f, 0.0f, 100000.0f);

			// 終了
			ImGui::TreePop();
		}


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