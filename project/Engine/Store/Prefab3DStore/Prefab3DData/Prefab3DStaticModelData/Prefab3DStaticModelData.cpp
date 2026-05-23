#include "Prefab3DStaticModelData.h"
#include "Store/ModelStore/ModelStore.h"
#include "PSO/PSOModel/BasePSOModel.h"
#include "PSO/PSOShadowMap/BasePSOShadowMap.h"

#include "Store/LightStore/LightStore.h"
#include "Store/TextureStore/TextureStore.h"
#include "Store/Camera3DStore/Camera3DStore.h"
#include "Store/SkyboxStore/SkyboxStore.h"
#include "Store/PostEffectStore/PostEffectStore.h"

#include "Parameter/Prefab3DParameter/Prefab3DParameter.h"

#include "Application/WorldTransform/WorldTransform3D/WorldTransform3D.h"

#include "RenderContext/ImGuiRender/ImGuiRender.h"

/// @brief コンストラクタ
/// @param name 
/// @param numInstance 
/// @param hPrefab 
/// @param hTexture 
Engine::Prefab3DStaticModelData::Prefab3DStaticModelData(const std::string& name, uint32_t numInstance, Prefab3DHandle hPrefab3D, ModelHandle hModel,BasePSOModel* pso, Prefab3DParameter* parameter)
	: hModel_(hModel), Prefab3DBaseData(name, numInstance, hPrefab3D, pso, parameter)
{
	// 種類
	type_ = Prefab3D::Type::StaticModel;

	// パラメータを生成する
	param_ = std::make_unique<Prefab3D::StaticModel::Base::Param>();
}

/// @brief 初期化
/// @param modelStore 
/// @param textureStore 
/// @param lightStore 
/// @param cameraStore 
/// @param heap 
/// @param device 
/// @param log 
void Engine::Prefab3DStaticModelData::Initialize(ModelStore* modelStore, TextureStore* textureStore, LightStore* lightStore, Camera3DStore* cameraStore,
	DX12Heap* heap, ID3D12Device* device, Log* log)
{
	// nullptrチェック
	assert(modelStore);
	assert(textureStore);
	assert(lightStore);
	assert(cameraStore);
	assert(heap);
	assert(device);

	// 引数を受け取る
	modelStore_ = modelStore;
	textureStore_ = textureStore;
	lightStore_ = lightStore;
	cameraStore_ = cameraStore;

	// モデルデータを取得する
	ModelData modelData = modelStore_->GetModelData(hModel_);

	// ブレンドモード
	param_->blendMode = BlendMode::kNone;

	// モデルトランスフォーム
	param_->modelTransform.scale = Vector3(1.0f, 1.0f, 1.0f);
	param_->modelTransform.rotate = Vector3(0.0f, 0.0f, 0.0f);
	param_->modelTransform.translate = Vector3(0.0f, 0.0f, 0.0f);

	// パラメータの記録
	group_ = "StaticModel_" + name_;
	if (parameter_)
	{
		parameter_->SetValue(group_, "BlendMode", &param_->blendMode);
		parameter_->SetValue(group_, "Model_Transform_Scale", &param_->modelTransform.scale);
		parameter_->SetValue(group_, "Model_Transform_Rotate", &param_->modelTransform.rotate);
		parameter_->SetValue(group_, "Model_Transform_Translate", &param_->modelTransform.translate);
	}

	// 領域確保
	param_->meshMaterial.resize(static_cast<int32_t>(modelData.meshes.size()));
	param_->meshTransforms.resize(static_cast<int32_t>(modelData.meshes.size()));
	param_->meshBlur.resize(static_cast<int32_t>(modelData.meshes.size()));
	primitiveResource_.resize(static_cast<int32_t>(modelData.meshes.size()));
	shadowMapTransformationResource_.resize(static_cast<int32_t>(modelData.meshes.size()));
	motionVectorResources_.resize(static_cast<int32_t>(modelData.meshes.size()));

	// ファイルパス
	textureFilePathTable_.resize(static_cast<int32_t>(modelData.meshes.size()));

	// メッシュごとにデータ生成
	for (int32_t meshIndex = 0; meshIndex < static_cast<int32_t>(modelData.meshes.size()); ++meshIndex)
	{
		// トランスフォーム
		param_->meshTransforms[meshIndex].scale = Vector3(1.0f, 1.0f, 1.0f);
		param_->meshTransforms[meshIndex].rotate = Vector3(0.0f, 0.0f, 0.0f);
		param_->meshTransforms[meshIndex].translate = Vector3(0.0f, 0.0f, 0.0f);

		// マテリアル
		param_->meshMaterial[meshIndex].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		param_->meshMaterial[meshIndex].uv.scale = Vector2(1.0f, 1.0f);
		param_->meshMaterial[meshIndex].uv.radius = 0.0f;
		param_->meshMaterial[meshIndex].uv.translate = Vector2(0.0f, 0.0f);
		param_->meshMaterial[meshIndex].hTexture = modelData.meshes[meshIndex].material.handle;
		param_->meshMaterial[meshIndex].environment = 0.0f;
		param_->meshMaterial[meshIndex].shininess = 10.0f;
		param_->meshMaterial[meshIndex].enableLighting = true;
		param_->meshMaterial[meshIndex].enableDiffuse = true;
		param_->meshMaterial[meshIndex].enableHalfLambert = true;
		param_->meshMaterial[meshIndex].enableSpecular = true;
		param_->meshMaterial[meshIndex].enableBlinnPhong = true;
		param_->meshMaterial[meshIndex].enableShadow = true;

		// ブラー
		param_->meshBlur[meshIndex].afterImageMask = 0.0f;
		param_->meshBlur[meshIndex].motionBlurMask = 0.0f;

		// テクスチャファイルパス
		textureFilePathTable_[meshIndex] = textureStore_->GetFilePath(param_->meshMaterial[meshIndex].hTexture);

		// パラメータの記録
		if (parameter_)
		{
			parameter_->SetValue(group_, modelData.meshNames[meshIndex] + "_Mesh_Transform_Scale", &param_->meshTransforms[meshIndex].scale);
			parameter_->SetValue(group_, modelData.meshNames[meshIndex] + "_Mesh_Transform_Rotate", &param_->meshTransforms[meshIndex].rotate);
			parameter_->SetValue(group_, modelData.meshNames[meshIndex] + "_Mesh_Transform_Translate", &param_->meshTransforms[meshIndex].translate);
			parameter_->SetValue(group_, modelData.meshNames[meshIndex] + "_Material_Color", &param_->meshMaterial[meshIndex].color);
			parameter_->SetValue(group_, modelData.meshNames[meshIndex] + "_Material_UV_Scale", &param_->meshMaterial[meshIndex].uv.scale);
			parameter_->SetValue(group_, modelData.meshNames[meshIndex] + "_Material_UV_Rotate", &param_->meshMaterial[meshIndex].uv.radius);
			parameter_->SetValue(group_, modelData.meshNames[meshIndex] + "_Material_UV_Translate", &param_->meshMaterial[meshIndex].uv.translate);
			parameter_->SetValue(group_, modelData.meshNames[meshIndex] + "_Material_Environment", &param_->meshMaterial[meshIndex].environment);
			parameter_->SetValue(group_, modelData.meshNames[meshIndex] + "_Material_Shininess", &param_->meshMaterial[meshIndex].shininess);
			parameter_->SetValue(group_, modelData.meshNames[meshIndex] + "_Material_Enable_Lighting", &param_->meshMaterial[meshIndex].enableLighting);
			parameter_->SetValue(group_, modelData.meshNames[meshIndex] + "_Material_Enable_Diffuse", &param_->meshMaterial[meshIndex].enableDiffuse);
			parameter_->SetValue(group_, modelData.meshNames[meshIndex] + "_Material_Enable_HalfLambert", &param_->meshMaterial[meshIndex].enableHalfLambert);
			parameter_->SetValue(group_, modelData.meshNames[meshIndex] + "_Material_Enable_Specular", &param_->meshMaterial[meshIndex].enableSpecular);
			parameter_->SetValue(group_, modelData.meshNames[meshIndex] + "_Material_Enable_BlinnPhong", &param_->meshMaterial[meshIndex].enableBlinnPhong);
			parameter_->SetValue(group_, modelData.meshNames[meshIndex] + "_Material_Enable_Shadow", &param_->meshMaterial[meshIndex].enableShadow);
			parameter_->SetValue(group_, modelData.meshNames[meshIndex] + "_Mesh_Blur_AfterImageMask", &param_->meshBlur[meshIndex].afterImageMask);
			parameter_->SetValue(group_, modelData.meshNames[meshIndex] + "_Mesh_Blur_MotionBlurMask", &param_->meshBlur[meshIndex].motionBlurMask);
			parameter_->SetValue(group_, modelData.meshNames[meshIndex] + "_Material_Texture", &textureFilePathTable_[meshIndex]);
		}

		// プリミティブ
		primitiveResource_[meshIndex] = std::make_unique<StructuredBufferResource<Prefab::PrimitiveDataForGPU>>();
		primitiveResource_[meshIndex]->Initialize(device, heap, numInstance_, log);

		// シャドウマップ
		shadowMapTransformationResource_[meshIndex] = std::make_unique<StructuredBufferResource<Matrix4x4>>();
		shadowMapTransformationResource_[meshIndex]->Initialize(device, heap, numInstance_, log);

		// モーションベクター
		motionVectorResources_[meshIndex] = std::make_unique<StructuredBufferResource<MotionVectorDataForGPU>>();
		motionVectorResources_[meshIndex]->Initialize(device, heap, numInstance_, log);
	}

	// 値を反映させる
	if (parameter_)parameter_->RegisterGroupDataReflection(group_);
	for (int32_t meshIndex = 0; meshIndex < modelData.meshes.size(); ++meshIndex)
		param_->meshMaterial[meshIndex].hTexture = textureStore_->GetHandle(textureFilePathTable_[meshIndex]);
}

/// @brief 更新処理
void Engine::Prefab3DStaticModelData::Update()
{
	// 削除されたインスタンスをリストから除外する
	instanceTable_.remove_if([](std::unique_ptr<PrefabInstanceStaticModel>& instance) {if (instance->isDelete_) { return true; }return false; });
}

/// @brief リセット
void Engine::Prefab3DStaticModelData::Reset()
{
	// モデルデータを取得する
	ModelData modelData = modelStore_->GetModelData(hModel_);

	if (parameter_->IsFileFound(group_))
	{
		// 値を反映させる
		if (parameter_)parameter_->RegisterGroupDataReflection(group_);
		for (int32_t meshIndex = 0; meshIndex < modelData.meshes.size(); ++meshIndex)
			param_->meshMaterial[meshIndex].hTexture = textureStore_->GetHandle(textureFilePathTable_[meshIndex]);
	}
	else
	{
		// ブレンドモード
		param_->blendMode = BlendMode::kNone;

		// モデルトランスフォーム
		param_->modelTransform.scale = Vector3(1.0f, 1.0f, 1.0f);
		param_->modelTransform.rotate = Vector3(0.0f, 0.0f, 0.0f);
		param_->modelTransform.translate = Vector3(0.0f, 0.0f, 0.0f);

		// メッシュごとにデータ生成
		for (int32_t meshIndex = 0; meshIndex < static_cast<int32_t>(modelData.meshes.size()); ++meshIndex)
		{
			// トランスフォーム
			param_->meshTransforms[meshIndex].scale = Vector3(1.0f, 1.0f, 1.0f);
			param_->meshTransforms[meshIndex].rotate = Vector3(0.0f, 0.0f, 0.0f);
			param_->meshTransforms[meshIndex].translate = Vector3(0.0f, 0.0f, 0.0f);

			// マテリアル
			param_->meshMaterial[meshIndex].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
			param_->meshMaterial[meshIndex].uv.scale = Vector2(1.0f, 1.0f);
			param_->meshMaterial[meshIndex].uv.radius = 0.0f;
			param_->meshMaterial[meshIndex].uv.translate = Vector2(0.0f, 0.0f);
			param_->meshMaterial[meshIndex].hTexture = modelData.meshes[meshIndex].material.handle;
			param_->meshMaterial[meshIndex].environment = 0.0f;
			param_->meshMaterial[meshIndex].shininess = 10.0f;
			param_->meshMaterial[meshIndex].enableLighting = true;
			param_->meshMaterial[meshIndex].enableDiffuse = true;
			param_->meshMaterial[meshIndex].enableHalfLambert = true;
			param_->meshMaterial[meshIndex].enableSpecular = true;
			param_->meshMaterial[meshIndex].enableBlinnPhong = true;
			param_->meshMaterial[meshIndex].enableShadow = true;

			// ブラー
			param_->meshBlur[meshIndex].afterImageMask = 0.0f;
			param_->meshBlur[meshIndex].motionBlurMask = 0.0f;

			// テクスチャファイルパス
			textureFilePathTable_[meshIndex] = textureStore_->GetFilePath(param_->meshMaterial[meshIndex].hTexture);
		}
	}

	// 読み込まれたことにする
	isLoad_ = true;
}

/// @brief コマンドリストに登録する
/// @param commandList 
/// @param pso 
void Engine::Prefab3DStaticModelData::Register(SkyboxStore* skyboxStore, ID3D12GraphicsCommandList* commandList)
{
	// 読み込まれていないときは処理しない
	if (!isLoad_)return;

	// インスタンス描画命令を行っていないときは処理しない
	if (numUseInstance_ <= 0)
		return;


	// モデルデータを取得する
	ModelData modelData = modelStore_->GetModelData(hModel_);


	// PSOの設定
	pso_->Register(commandList, param_->blendMode);

	// カメラの設定
	cameraStore_->RegisterCameraResource(commandList, 4);

	// スカイボックスの設定
	skyboxStore->RegisterCubeMapTexture(commandList, 5);

	// ライトの設定
	lightStore_->LightRegister(commandList, 6, 7, 8, 9);

	// メッシュごとに処理
	for (int32_t meshIndex = 0; meshIndex < static_cast<int32_t>(modelData.meshes.size()); meshIndex++)
	{
		// 頂点の設定
		modelStore_->Register(commandList, hModel_, meshIndex);

		// プリミティブの設定
		primitiveResource_[meshIndex]->RegisterGraphics(commandList, 0);

		// テクスチャの設定
		commandList->SetGraphicsRootDescriptorTable(1, textureStore_->GetSrvGpuHandle(param_->meshMaterial[meshIndex].hTexture));

		// シャドウマップテクスチャの設定
		lightStore_->GetShadowMapTextureResource()->Register(commandList, 2);

		// シャドウ用座標変換の設定
		lightStore_->GetShadowMapTransformationResource()->RegisterGraphics(commandList, 3);

		// 形状の設定
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// ドローコール
		commandList->DrawIndexedInstanced(static_cast<UINT>(modelStore_->GetModelData(hModel_).meshes[meshIndex].indices.size()), numUseInstance_, 0, 0, 0);
	}
}

/// @brief シャドウマップを描画する
/// @param viewProjection 
/// @param commandList 
/// @param pso 
void Engine::Prefab3DStaticModelData::DrawShadowMap(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOShadowMap* pso)
{
	// 読み込まれていないときは処理しない
	if (!isLoad_)return;

	// シャドウインスタンスがないときは処理しない
	if (numShadowInstance_ <= 0)return;


	// モデルデータを取得する
	ModelData modelData = modelStore_->GetModelData(hModel_);

	// PSOの設定
	pso->Register(commandList);

	// メッシュごとに処理
	for (int32_t meshIndex = 0; meshIndex < static_cast<int32_t>(modelData.meshes.size()); meshIndex++)
	{
		UINT useInstance = 0;

		for (useInstance = 0; useInstance < numShadowInstance_; ++useInstance)
		{
			// インスタンス数を越えたら処理しない
			if (useInstance >= numInstance_)break;

			// シャドウマップ用座標変換の計算
			shadowMapTransformationResource_[meshIndex]->data_[useInstance] = 
				primitiveResource_[meshIndex]->data_[useInstance].world * viewProjection;
		}


		// 使用インスタンスがなかったら処理しない
		if (useInstance <= 0)
			continue;


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
		commandList->DrawIndexedInstanced(static_cast<UINT>(modelStore_->GetModelData(hModel_).meshes[meshIndex].indices.size()), useInstance, 0, 0, 0);
	}

	// シャドウインスタンスをリセットする
	numShadowInstance_ = 0;
}

/// @brief モーションベクターを描画する
/// @param commandList 
/// @param pso 
void Engine::Prefab3DStaticModelData::RegisterMotionVector(ID3D12GraphicsCommandList* commandList, BasePSOMotionVector* pso)
{
	// 読み込まれていないときは処理しない
	if (!isLoad_)return;

	// インスタンス描画命令を行っていないときは処理しない
	if (numUseInstance_ <= 0)
		return;


	// モデルデータを取得する
	ModelData modelData = modelStore_->GetModelData(hModel_);

	// PSOの設定
	pso->Register(commandList);

	// メッシュごとに処理
	for (int32_t meshIndex = 0; meshIndex < static_cast<int32_t>(modelData.meshes.size()); meshIndex++)
	{
		// 頂点の設定
		modelStore_->Register(commandList, hModel_, meshIndex);

		// モーションベクトルの設定
		motionVectorResources_[meshIndex]->RegisterGraphics(commandList, 0);

		// 形状の設定
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// ドローコール
		commandList->DrawIndexedInstanced(static_cast<UINT>(modelStore_->GetModelData(hModel_).meshes[meshIndex].indices.size()), numUseInstance_, 0, 0, 0);
	}
}

/// @brief インスタンスを生成する
/// @return 
void* Engine::Prefab3DStaticModelData::CreateInstance()
{
	// インスタンスを生成する
	std::unique_ptr<PrefabInstanceStaticModel> instance =
		std::make_unique<PrefabInstanceStaticModel>([this](const Prefab3D::StaticModel::Instance::Param* param) {DrawCallInstance(param); }, param_.get());

	// ポインタを保存する
	PrefabInstanceStaticModel* pInstance = instance.get();

	// テーブルに追加する
	instanceTable_.push_back(std::move(instance));

	return pInstance;
}

/// @brief 全てのインスタンスを削除する
void Engine::Prefab3DStaticModelData::DestroyAllInstance()
{
	instanceTable_.clear();
}

/// @brief インスタンスのドローコール
/// @param param 
void Engine::Prefab3DStaticModelData::DrawCallInstance(const Engine::Prefab3D::StaticModel::Instance::Param* param)
{
	// 読み込まれていないときは処理しない
	if (!isLoad_)return;

	// インスタンス数を越えたら処理しない
	if (numUseInstance_ >= numInstance_)
		return;

	// モデルデータを取得する
	const ModelData& modelData = modelStore_->GetModelData(hModel_);

	Quaternion modelQuaternion =
		ToQuaternion(param->modelTransform.rotate.z, Vector3(0.0f, 0.0, 1.0f)).Normalize() *
		ToQuaternion(param->modelTransform.rotate.y, Vector3(0.0f, 1.0, 0.0f)).Normalize() *
		ToQuaternion(param->modelTransform.rotate.x, Vector3(1.0f, 0.0, 0.0f)).Normalize();

	Matrix4x4 worldMatrix = Make3DAffineMatrix4x4(param->modelTransform.scale, modelQuaternion, param->modelTransform.translate);

	// 親子関係のあるモデルのときは、親のワールド行列を掛ける
	if (param->parent)worldMatrix = worldMatrix * param->parent->GetWorldMatrix();


	// ビュープロジェクション行列を取得する
	Matrix4x4 viewProjection = cameraStore_->GetCamera3D().GetCurrentVPMatrix();
	Matrix4x4 prevVPUnJitter = cameraStore_->GetCamera3D().GetPrevVPUnJitterMatrix();
	Matrix4x4 currentVPUnJitter = cameraStore_->GetCamera3D().GetCurrentVPUnJitterMatrix();


	for (int meshIndex = 0; meshIndex < static_cast<int32_t>(modelData.meshes.size()); meshIndex++)
	{
		// ノード行列
		Matrix4x4 nodeMatrix = MakeIdentityMatrix4x4();
		if (!modelData.nodes.empty())nodeMatrix = modelData.nodes[static_cast<int32_t>(modelStore_->GetModelData(hModel_).meshes.size()) - 1 - meshIndex].worldMatrix;

		Quaternion meshQuaternion =
			ToQuaternion(param->meshTransforms[meshIndex].rotate.z, Vector3(0.0f, 0.0, 1.0f)).Normalize() *
			ToQuaternion(param->meshTransforms[meshIndex].rotate.y, Vector3(0.0f, 1.0, 0.0f)).Normalize() *
			ToQuaternion(param->meshTransforms[meshIndex].rotate.x, Vector3(1.0f, 0.0, 0.0f)).Normalize();

		Matrix4x4 localMatrix = Make3DAffineMatrix4x4(param->meshTransforms[meshIndex].scale, meshQuaternion, param->meshTransforms[meshIndex].translate);


		// 前フレームのWVP行列
		motionVectorResources_[meshIndex]->data_[numUseInstance_].prevWVPMatrix =
			primitiveResource_[meshIndex]->data_[numUseInstance_].world * prevVPUnJitter;

		// ワールド座標
		primitiveResource_[meshIndex]->data_[numUseInstance_].world =
			localMatrix * nodeMatrix * worldMatrix;

		// 現フレームのWVP行列
		motionVectorResources_[meshIndex]->data_[numUseInstance_].currentWVPMatrix =
			primitiveResource_[meshIndex]->data_[numUseInstance_].world * currentVPUnJitter;

		// ワールドビュー正射影行列
		primitiveResource_[meshIndex]->data_[numUseInstance_].worldViewProjection =
			primitiveResource_[meshIndex]->data_[numUseInstance_].world * viewProjection;

		// 逆転置ワールド行列
		primitiveResource_[meshIndex]->data_[numUseInstance_].worldInverseTranspose =
			primitiveResource_[meshIndex]->data_[numUseInstance_].world.Transpose().Inverse();



		// 色
		primitiveResource_[meshIndex]->data_[numUseInstance_].color = param->meshMaterial[meshIndex].color;

		// 環境
		primitiveResource_[meshIndex]->data_[numUseInstance_].environment = param->meshMaterial[meshIndex].environment;

		// 光沢度
		primitiveResource_[meshIndex]->data_[numUseInstance_].shininess = param->meshMaterial[meshIndex].shininess;

		// UV行列
		primitiveResource_[meshIndex]->data_[numUseInstance_].uvTransform =
			Make3DScaleMatrix4x4(Vector3(param->meshMaterial[meshIndex].uv.scale.x, param->meshMaterial[meshIndex].uv.scale.y, 1.0f)) *
			Make3DRotateZMatrix4x4(param->meshMaterial[meshIndex].uv.radius) *
			Make3DTranslateMatrix4x4(Vector3(param->meshMaterial[meshIndex].uv.translate.x, param->meshMaterial[meshIndex].uv.translate.y, 0.0f));

		// ライティング有効化
		primitiveResource_[meshIndex]->data_[numUseInstance_].enableLighting = static_cast<int32_t>(param->meshMaterial[meshIndex].enableLighting);

		// ディフューズ有効化
		primitiveResource_[meshIndex]->data_[numUseInstance_].enableDiffuse = static_cast<int32_t>(param->meshMaterial[meshIndex].enableDiffuse);

		// ハーフランバード有効化
		primitiveResource_[meshIndex]->data_[numUseInstance_].enableHalfLambert = static_cast<int32_t>(param->meshMaterial[meshIndex].enableHalfLambert);

		// スペキュラー有効化
		primitiveResource_[meshIndex]->data_[numUseInstance_].enableSpecular = static_cast<int32_t>(param->meshMaterial[meshIndex].enableSpecular);

		// ブリンフォン有効化
		primitiveResource_[meshIndex]->data_[numUseInstance_].enableBlinnPhong = static_cast<int32_t>(param->meshMaterial[meshIndex].enableBlinnPhong);

		// シャドウ有効化
		primitiveResource_[meshIndex]->data_[numUseInstance_].enableShadow = static_cast<int32_t>(param->meshMaterial[meshIndex].enableShadow);


		// ブラー
		motionVectorResources_[meshIndex]->data_[numUseInstance_].afterImageMask = param->meshBlur[meshIndex].afterImageMask;
		motionVectorResources_[meshIndex]->data_[numUseInstance_].motionBlurMask = param->meshBlur[meshIndex].motionBlurMask;
	}

	// 使用インスタンスをカウントする
	numUseInstance_++;
	numShadowInstance_++;
}

/// @brief デバッグ用パラメータ
void Engine::Prefab3DStaticModelData::DebugParameter()
{
#ifdef _DEVELOPMENT

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
			ImGui::DragFloat3("Scale", &param_->modelTransform.scale.x, 0.01f, -100000.0f, 100000.0f);

			// 回転
			ImGui::DragFloat3("Rotate", &param_->modelTransform.rotate.x, 0.01f, -100000.0f, 100000.0f);

			// 平行移動
			ImGui::DragFloat3("Translate", &param_->modelTransform.translate.x, 0.01f, -100000.0f, 100000.0f);

			// 終了
			ImGui::TreePop();
		}


		// モデルデータを取得する
		const ModelData& modelData = modelStore_->GetModelData(hModel_);

		// メッシュ
		if (ImGui::TreeNode("Mesh"))
		{
			for (int32_t meshIndex = 0; meshIndex < static_cast<int32_t>(modelData.meshes.size()); ++meshIndex)
			{
				// メッシュ
				if (ImGui::TreeNode(modelData.meshNames[meshIndex].c_str()))
				{
					// トランスフォーム
					if (ImGui::TreeNode("Transform"))
					{
						// 拡縮
						ImGui::DragFloat3("Scale", &param_->meshTransforms[meshIndex].scale.x, 0.01f, -100000.0f, 100000.0f);

						// 回転
						ImGui::DragFloat3("Rotate", &param_->meshTransforms[meshIndex].rotate.x, 0.01f, -100000.0f, 100000.0f);

						// 平行移動
						ImGui::DragFloat3("Translate", &param_->meshTransforms[meshIndex].translate.x, 0.01f, -100000.0f, 100000.0f);

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
							ImGui::DragFloat2("Scale", &param_->meshMaterial[meshIndex].uv.scale.x, 0.01f, -100000.0f, 100000.0f);

							// 回転
							ImGui::DragFloat("Rotate", &param_->meshMaterial[meshIndex].uv.radius, 0.01f, -100000.0f, 100000.0f);

							// 平行移動
							ImGui::DragFloat2("Translate", &param_->meshMaterial[meshIndex].uv.translate.x, 0.01f, -100000.0f, 100000.0f);

							// 終了
							ImGui::TreePop();
						}

						// 色
						ImGui::ColorEdit4("Color", &param_->meshMaterial[meshIndex].color.x);

						// テクスチャ
						ImGui::Text("Texture");

						ImGui::ImageButton(
							textureStore_->GetFilePath(param_->meshMaterial[meshIndex].hTexture).c_str(),
							textureStore_->GetSrvGpuHandle(param_->meshMaterial[meshIndex].hTexture).ptr,
							ImVec2(32.0f, 32.0f),
							ImVec2(0, 0),
							ImVec2(1, 1),
							ImVec4(0.2f, 0.2f, 0.2f, 1.0f),
							ImVec4(1, 1, 1, 1)
						);

						// --- ドロップ処理 ---
						if (ImGui::BeginDragDropTarget())
						{
							if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TEXTURE_ID"))
							{
								int droppedIndex = *(const int*)payload->Data;

								// droppedIndex が dataTable_ の index
								// ここでマテリアルなどに設定する
								param_->meshMaterial[meshIndex].hTexture = static_cast<uint32_t>(droppedIndex);
								textureFilePathTable_[meshIndex] = textureStore_->GetFilePath(param_->meshMaterial[meshIndex].hTexture);
							}
							ImGui::EndDragDropTarget();
						}



						ImGui::SeparatorText("Lighting");

						// ライティング有効化
						ImGui::Checkbox("Lighting", &param_->meshMaterial[meshIndex].enableLighting);

						if (param_->meshMaterial[meshIndex].enableLighting)
						{
							// ディフューズ有効化
							ImGui::Checkbox("Diffuse", &param_->meshMaterial[meshIndex].enableDiffuse);

							if (param_->meshMaterial[meshIndex].enableDiffuse)
							{
								// ハーフランバート有効化
								ImGui::Checkbox("HalfLambert", &param_->meshMaterial[meshIndex].enableHalfLambert);
							}

							// スペキュラー有効化
							ImGui::Checkbox("Specular", &param_->meshMaterial[meshIndex].enableSpecular);

							if (param_->meshMaterial[meshIndex].enableSpecular)
							{
								// ブリンフォン有効化
								ImGui::Checkbox("BlinnPhong", &param_->meshMaterial[meshIndex].enableBlinnPhong);

								// 光沢度
								ImGui::DragFloat("Shininess", &param_->meshMaterial[meshIndex].shininess, 0.1f);
							}

							// シャドウ有効化
							ImGui::Checkbox("Shadow", &param_->meshMaterial[meshIndex].enableShadow);

							// 環境
							ImGui::SliderFloat("Environment", &param_->meshMaterial[meshIndex].environment, 0.0f, 1.0f);
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
								ImGui::DragFloat("AfterImageMask", &param_->meshBlur[meshIndex].afterImageMask, 0.01f, 0.0f, 1.0f);

							// モーションブラー
							if (PostEffectStore::IsLoadMotionBlur())
								ImGui::DragFloat("MotionBlurMask", &param_->meshBlur[meshIndex].motionBlurMask, 0.01f, 0.0f, 1.0f);

							// 終了
							ImGui::TreePop();
						}
					}

					// 終了
					ImGui::TreePop();
				}
			}

			// 終了
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