#include "Render3DSkinningModelData.h"
#include "Store/ModelStore/ModelStore.h"
#include "Store/TextureStore/TextureStore.h"
#include <cassert>
#include "PSO/PSOModel/BasePSOModel.h"
#include "PSO/PSOShadowMap/BasePSOShadowMap.h"
#include "Store/LightStore/LightStore.h"
#include "Store/AnimationStore/AnimationStore.h"
#include "Store/SkeletonStore/SkeletonStore.h"
#include "Func/ModelFunc/ModelFunc.h"
#include "PSO/ComputePSO/ComputePSOSkinning/ComputePSOSkinning.h"
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
void Engine::Render3DSkinningModelData::Initialize(ModelStore* modelStore, TextureStore* textureStore, AnimationStore* animationStore, SkeletonStore* skeletonStore, LightStore* lightStore,
	DX12Heap* heap, ID3D12Device* device, ID3D12GraphicsCommandList* commandList, Log* log)
{
	// nullptrチェック
	assert(modelStore);
	assert(textureStore);
	assert(animationStore);
	assert(skeletonStore);
	assert(lightStore);
	assert(device);

	// 引数を受け取る
	modelStore_ = modelStore;
	textureStore_ = textureStore;
	animationStore_ = animationStore;
	skeletonStore_ = skeletonStore;
	lightStore_ = lightStore;


	// パラメータの生成
	param_ = std::make_unique<Render3D::SkinningModel::Param>();


	// ブレンドモード
	param_->blendMode = BlendMode::kNone;

	// モデルトランスフォーム
	param_->modelTransform.scale = Vector3(1.0f, 1.0f, 1.0f);
	param_->modelTransform.rotate = Vector3(0.0f, 0.0f, 0.0f);
	param_->modelTransform.translate = Vector3(0.0f, 0.0f, 0.0f);

	// アニメーション
	param_->animation.timer = 0.0f;
	param_->animation.hAnimation = hAnimation_;
	param_->animation.hSkeleton = hSkeleton_;

	// パラメータの記録
	group_ = "SkinningModel_" + name_;
	if (parameter_)
	{
		parameter_->SetValue(group_, "BlendMode", &param_->blendMode);
		parameter_->SetValue(group_, "Model_Transform_Scale", &param_->modelTransform.scale);
		parameter_->SetValue(group_, "Model_Transform_Rotate", &param_->modelTransform.rotate);
		parameter_->SetValue(group_, "Model_Transform_Translate", &param_->modelTransform.translate);
		parameter_->SetValue(group_, "Animation_Timer", &param_->animation.timer);
	}


	// モデルデータを取得する
	const ModelData& modelData = modelStore_->GetModelData(hModel_);

	// スケルトンデータを取得する
	const ModelBoneData& modelBoneData = skeletonStore_->GetBoneData(hSkeleton_);

	// プリミティブ専用スケルトンを用意する
	skeleton_ = skeletonStore_->GetSkeleton(hSkeleton_);

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
	inputVertexResource_.resize(static_cast<int32_t>(modelData.meshes.size()));
	outputVertexResource_.resize(static_cast<int32_t>(modelData.meshes.size()));
	vertexNumResource_.resize(static_cast<int32_t>(modelData.meshes.size()));
	skinClusters_.resize(static_cast<int32_t>(modelData.meshes.size()));

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
			parameter_->SetValue(group_, modelData.meshNames[meshIndex] + "_Material_Draw_ShadowMap", &param_->meshMaterial[meshIndex].drawShadowMap);
			parameter_->SetValue(group_, modelData.meshNames[meshIndex] + "_Material_Enable_Shadow", &param_->meshMaterial[meshIndex].enableShadow);

			parameter_->SetValue(group_, modelData.meshNames[meshIndex] + "_Blur_AfterImageMask", &param_->meshBlur[meshIndex].afterImageMask);
			parameter_->SetValue(group_, modelData.meshNames[meshIndex] + "_Blur_MotionBlurMask", &param_->meshBlur[meshIndex].motionBlurMask);

			parameter_->SetValue(group_, modelData.meshNames[meshIndex] + "_Outline_Enable", &param_->meshOutline[meshIndex].enableOutline);
			parameter_->SetValue(group_, modelData.meshNames[meshIndex] + "_Outline_Color", &param_->meshOutline[meshIndex].color);

			parameter_->SetValue(group_, modelData.meshNames[meshIndex] + "_Material_Texture", &textureFilePathTable_[meshIndex]);
		}


		// 座標変換リソース
		meshTransformationResources_[meshIndex] = std::make_unique<ConstantBufferResource<PrimitiveModelTransformationDataForGPU>>();
		meshTransformationResources_[meshIndex]->Initialize(device, log);
		meshTransformationResources_[meshIndex]->data_->worldMatrix = MakeIdentityMatrix4x4();

		// マテリアルリソース
		meshMaterialResources_[meshIndex] = std::make_unique<ConstantBufferResource<PrimitiveModelMaterialDataForGPU>>();
		meshMaterialResources_[meshIndex]->Initialize(device, log);

		// 入力頂点リソース
		inputVertexResource_[meshIndex] = std::make_unique<StructuredBufferResource<VertexDataForGPU>>();
		inputVertexResource_[meshIndex]->Initialize(device, heap, UINT(modelData.meshes[meshIndex].vertices.size()), log);

		for (int32_t i = 0; i < int32_t(modelData.meshes[meshIndex].vertices.size()); ++i)
		{
			inputVertexResource_[meshIndex]->data_[i].position = modelData.meshes[meshIndex].vertices[i].position;
			inputVertexResource_[meshIndex]->data_[i].texcoord = modelData.meshes[meshIndex].vertices[i].texcoord;
			inputVertexResource_[meshIndex]->data_[i].normal = modelData.meshes[meshIndex].vertices[i].normal;
		}

		// 出力頂点リソース
		outputVertexResource_[meshIndex] = std::make_unique<RWStructuredVertexBufferResource<VertexDataForGPU>>();
		outputVertexResource_[meshIndex]->Initialize(device,commandList, heap, UINT(modelData.meshes[meshIndex].vertices.size()), log);

		// 頂点数リソース
		vertexNumResource_[meshIndex] = std::make_unique<ConstantBufferResource<uint32_t>>();
		vertexNumResource_[meshIndex]->Initialize(device, log);
		*vertexNumResource_[meshIndex]->data_ = static_cast<uint32_t>(modelData.meshes[meshIndex].vertices.size());

		// スキンクラスター
		skinClusters_[meshIndex] = std::make_unique<SkinCluster>();
		skinClusters_[meshIndex]->Initialize(heap, device, modelData.meshes[meshIndex], modelBoneData.meshes[meshIndex], skeleton_, log);

		// シャドウマップ用座標変換リソース
		shadowMapTransformationResource_[meshIndex] = std::make_unique<ConstantBufferResource<Matrix4x4>>();
		shadowMapTransformationResource_[meshIndex]->Initialize(device, log);

		// モーションベクター用リソース
		motionVectorResources_[meshIndex] = std::make_unique<ConstantBufferResource<MotionVectorDataForGPU>>();
		motionVectorResources_[meshIndex]->Initialize(device, log);

		// アウトライン用座標変換リソース
		outlineTransformationResources_[meshIndex] = std::make_unique<ConstantBufferResource<Matrix4x4>>();
		outlineTransformationResources_[meshIndex]->Initialize(device, log);

		// アウトライン用色リソース
		outlineColorResources_[meshIndex] = std::make_unique<ConstantBufferResource<Vector4>>();
		outlineColorResources_[meshIndex]->Initialize(device, log);
	}

	// 値を反映させる
	if (parameter_)parameter_->RegisterGroupDataReflection(group_);
	for (int32_t meshIndex = 0; meshIndex < modelData.meshes.size(); ++meshIndex)
		param_->meshMaterial[meshIndex].hTexture = textureStore_->GetHandle(textureFilePathTable_[meshIndex]);
}

/// @brief 更新処理
void Engine::Render3DSkinningModelData::Update()
{
	// 描画を記録する
	isPreDrew_ = isDrew_;
	isDrew_ = false;

	// 読み込まれていないときは処理しない
	if (!isLoad_)return;

	// 更新するかどうか
	if (!param_->isUpdate)return;

	Animation animation = animationStore_->GetAnimation(param_->animation.hAnimation);

	// 現在の時間に合わせたスケルトンを取得する
	Skeleton endSkeleton = ApplyBoneAnimation(skeleton_, animation, param_->animation.timer);

	// スケルトンを補間
	LerpSkeleton(skeleton_, endSkeleton, 0.3f);

	// スケルトンの更新
	ForwardKinematics(skeleton_);

	// モデルデータを取得する
	const ModelData& modelData = modelStore_->GetModelData(hModel_);

	for (int32_t meshIndex = 0; meshIndex < static_cast<int32_t>(modelData.meshes.size()); meshIndex++)
	{
		skinClusters_[meshIndex]->Update(skeleton_);
	}
}

/// @brief リセット
void Engine::Render3DSkinningModelData::Reset()
{
	// モデルデータを取得する
	const ModelData& modelData = modelStore_->GetModelData(hModel_);

	// jsonファイルがあるかどうか
	if (parameter_->IsFileFound(group_))
	{
		// ファイルがあるとき

		// 値を反映させる
		if (parameter_)parameter_->RegisterGroupDataReflection(group_);
		for (int32_t meshIndex = 0; meshIndex < modelData.meshes.size(); ++meshIndex)
			param_->meshMaterial[meshIndex].hTexture = textureStore_->GetHandle(textureFilePathTable_[meshIndex]);
	} 
	else
	{
		// ファイルがないとき

		// ブレンドモード
		param_->blendMode = BlendMode::kNone;

		// モデルトランスフォーム
		param_->modelTransform.scale = Vector3(1.0f, 1.0f, 1.0f);
		param_->modelTransform.rotate = Vector3(0.0f, 0.0f, 0.0f);
		param_->modelTransform.translate = Vector3(0.0f, 0.0f, 0.0f);

		// アニメーション
		param_->animation.timer = 0.0f;
		param_->animation.hAnimation = hAnimation_;
		param_->animation.hSkeleton = hSkeleton_;

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
		}
	}

	// 読み込む
	isLoad_ = true;
}

/// @brief スキニングを行う
/// @param commandList 
/// @param pso 
void Engine::Render3DSkinningModelData::Skinning(ID3D12GraphicsCommandList* commandList, ComputePSOSkinning* pso)
{
	// 読み込まれていないときは処理しない
	if (!isLoad_)return;

	// 更新するかどうか
	if (!param_->isUpdate)return;

	// モデルデータを取得する
	const ModelData& modelData = modelStore_->GetModelData(hModel_);

	// PSOの設定
	pso->Register(commandList);

	for (int32_t meshIndex = 0; meshIndex < static_cast<int32_t>(modelData.meshes.size()); meshIndex++)
	{
		// 入力頂点
		inputVertexResource_[meshIndex]->RegisterCompute(commandList, 1);

		// スキンクラスター
		skinClusters_[meshIndex]->Register(commandList, 0, 2);

		// 出力頂点リソース
		outputVertexResource_[meshIndex]->RegisterCompute(commandList, 3);

		// 頂点数
		vertexNumResource_[meshIndex]->RegisterCompute(commandList, 4);

		commandList->Dispatch(UINT(modelData.meshes[meshIndex].vertices.size() + 1023) / 1024, 1, 1);
	}
}

/// @brief コマンドリストに登録する
/// @param commandList 
/// @param pso 
/// @param textureStore 
void Engine::Render3DSkinningModelData::Register(Camera3DStore* cameraStore, SkyboxStore* skyboxStore, ID3D12GraphicsCommandList* commandList, BasePSOModel* pso)
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
			localMatrix * worldMatrix;

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

		outputVertexResource_[meshIndex]->Barrier(commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

		// 頂点の設定
		modelStore_->Register(commandList, hModel_, meshIndex);

		D3D12_VERTEX_BUFFER_VIEW vbv = {};
		vbv.BufferLocation = outputVertexResource_[meshIndex]->GetResource()->GetGPUVirtualAddress();
		vbv.SizeInBytes = UINT(modelData.meshes[meshIndex].vertices.size()) * sizeof(VertexDataForGPU);
		vbv.StrideInBytes = sizeof(VertexDataForGPU);

		commandList->IASetVertexBuffers(0, 1, &vbv);


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

		outputVertexResource_[meshIndex]->Barrier(commandList, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);


#ifdef DEVELOPMENT
		// デバッグ用の線を描画する
		skeletonStore_->DrawDebugSkeleton(skeleton_,
			Vector3(meshTransformationResources_[meshIndex]->data_->worldMatrix.m[3][0],
				meshTransformationResources_[meshIndex]->data_->worldMatrix.m[3][1],
				meshTransformationResources_[meshIndex]->data_->worldMatrix.m[3][2]),
			Vector4(1.0f, 0.0f, 0.0f, 1.0f));
#endif
	}

	// 描画した
	isDrew_ = true;
}

/// @brief コマンドリスト
/// @param commandList 
/// @param pso 
void Engine::Render3DSkinningModelData::Register(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOShadowMap* pso)
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

		Quaternion meshQuaternion =
			ToQuaternion(param_->meshTransforms[meshIndex].rotate.z, Vector3(0.0f, 0.0, 1.0f)).Normalize() *
			ToQuaternion(param_->meshTransforms[meshIndex].rotate.y, Vector3(0.0f, 1.0, 0.0f)).Normalize() *
			ToQuaternion(param_->meshTransforms[meshIndex].rotate.x, Vector3(1.0f, 0.0, 0.0f)).Normalize();

		Matrix4x4 localMatrix = Make3DAffineMatrix4x4(param_->meshTransforms[meshIndex].scale, meshQuaternion, param_->meshTransforms[meshIndex].translate);


		// ワールド座標
		*shadowMapTransformationResource_[meshIndex]->data_ = (localMatrix * worldMatrix) * viewProjection;


		/*------------------------
		    コマンドリストに登録
		------------------------*/

		outputVertexResource_[meshIndex]->Barrier(commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

		// 頂点の設定
		modelStore_->Register(commandList, hModel_, meshIndex);

		D3D12_VERTEX_BUFFER_VIEW vbv = {};
		vbv.BufferLocation = outputVertexResource_[meshIndex]->GetResource()->GetGPUVirtualAddress();
		vbv.SizeInBytes = UINT(modelData.meshes[meshIndex].vertices.size()) * sizeof(VertexDataForGPU);
		vbv.StrideInBytes = sizeof(VertexDataForGPU);

		commandList->IASetVertexBuffers(0, 1, &vbv);

		// 座標変換の設定
		shadowMapTransformationResource_[meshIndex]->RegisterGraphics(commandList, 0);

		// 形状の設定
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// ドローコール
		commandList->DrawIndexedInstanced(static_cast<UINT>(modelStore_->GetModelData(hModel_).meshes[meshIndex].indices.size()), 1, 0, 0, 0);

		outputVertexResource_[meshIndex]->Barrier(commandList, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	}
}

/// @brief コマンドリストに登録
/// @param commandList 
/// @param pso 
void Engine::Render3DSkinningModelData::RegisterMotionVector(ID3D12GraphicsCommandList* commandList, BasePSOMotionVector* pso)
{
	// 読み込まれていないときは処理しない
	if (!isLoad_)return;

	// 今フレーム描画していないと処理しない
	if (!isDrew_)return;

	// PSOの設定
	pso->Register(commandList);

	// モデルデータを取得する
	const ModelData& modelData = modelStore_->GetModelData(hModel_);

	for (int32_t meshIndex = 0; meshIndex < static_cast<int32_t>(modelStore_->GetModelData(hModel_).meshes.size()); meshIndex++)
	{
		// スキニング出力頂点を描画用に遷移
		outputVertexResource_[meshIndex]->Barrier(commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

		// 頂点の設定
		modelStore_->Register(commandList, hModel_, meshIndex);

		D3D12_VERTEX_BUFFER_VIEW vbv = {};
		vbv.BufferLocation = outputVertexResource_[meshIndex]->GetResource()->GetGPUVirtualAddress();
		vbv.SizeInBytes = UINT(modelData.meshes[meshIndex].vertices.size()) * sizeof(VertexDataForGPU);
		vbv.StrideInBytes = sizeof(VertexDataForGPU);

		commandList->IASetVertexBuffers(0, 1, &vbv);

		// モーションベクター用座標変換の設定
		motionVectorResources_[meshIndex]->RegisterGraphics(commandList, 0);

		// 形状の設定
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// ドローコール
		commandList->DrawIndexedInstanced(static_cast<UINT>(modelData.meshes[meshIndex].indices.size()), 1, 0, 0, 0);

		// 次フレームのスキニング用に戻す
		outputVertexResource_[meshIndex]->Barrier(commandList, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	}
}

/// @brief アウトライン用のコマンドリストに登録
/// @param commandList 
/// @param cameraStore 
/// @param pso 
void Engine::Render3DSkinningModelData::RegisterOutline(ID3D12GraphicsCommandList* commandList, BasePSOOutline* pso)
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

/// @brief ボーンのワールド行列を取得する
/// @param name 
/// @return 
Matrix4x4 Engine::Render3DSkinningModelData::GetBoneWorldMatrix(const std::string& name)
{
	Quaternion modelQuaternion =
		ToQuaternion(param_->modelTransform.rotate.z, Vector3(0.0f, 0.0, 1.0f)).Normalize() *
		ToQuaternion(param_->modelTransform.rotate.y, Vector3(0.0f, 1.0, 0.0f)).Normalize() *
		ToQuaternion(param_->modelTransform.rotate.x, Vector3(1.0f, 0.0, 0.0f)).Normalize();

	Matrix4x4 worldMatrix = Make3DAffineMatrix4x4(param_->modelTransform.scale, modelQuaternion, param_->modelTransform.translate);
	if (parent_)worldMatrix = worldMatrix * parent_->GetWorldMatrix();

	// ボーンのローカル行列を取得する
	Matrix4x4 boneMatrix = skeleton_.joints[skeleton_.jointMap[name]].worldMatrix;
	Vector3 xVector = Vector3(boneMatrix.m[0][0], boneMatrix.m[0][1], boneMatrix.m[0][2]);
	Vector3 yVector = Vector3(boneMatrix.m[1][0], boneMatrix.m[1][1], boneMatrix.m[1][2]);
	Vector3 zVector = Vector3(boneMatrix.m[2][0], boneMatrix.m[2][1], boneMatrix.m[2][2]);

	Vector3 xRotate = xVector.Normalize();
	Vector3 yRotate = yVector.Normalize();
	Vector3 zRotate = zVector.Normalize();

	// ボーンのローカル行列から回転行列を作成する
	boneMatrix.m[0][0] = xRotate.x; boneMatrix.m[0][1] = xRotate.y; boneMatrix.m[0][2] = xRotate.z;
	boneMatrix.m[1][0] = yRotate.x; boneMatrix.m[1][1] = yRotate.y; boneMatrix.m[1][2] = yRotate.z;
	boneMatrix.m[2][0] = zRotate.x; boneMatrix.m[2][1] = zRotate.y; boneMatrix.m[2][2] = zRotate.z;

	// ボーンのワールド行列を返す
	return boneMatrix * worldMatrix;
}

/// @brief デバッグ用パラメータ
void Engine::Render3DSkinningModelData::DebugParameter()
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

		// アニメーション
		if (ImGui::TreeNode("Animation"))
		{
			// タイマー
			ImGui::DragFloat("Timer", &param_->animation.timer, 1.0f / 60.0f, 0.0f, 100000.0f);

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

						// テクスチャのボタンを作成する
						ImGui::ImageButton(textureStore_->GetFilePath(param_->meshMaterial[meshIndex].hTexture).c_str(), textureStore_->GetSrvGpuHandle(param_->meshMaterial[meshIndex].hTexture).ptr,
							ImVec2(32.0f, 32.0f), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0.2f, 0.2f, 0.2f, 1.0f), ImVec4(1, 1, 1, 1));

						// ドラッグアンドドロップのターゲットを作成する
						if (ImGui::BeginDragDropTarget())
						{
							if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TEXTURE_ID"))
							{
								int droppedIndex = *(const int*)payload->Data;

								// ドロップされたテクスチャを設定する
								param_->meshMaterial[meshIndex].hTexture = static_cast<uint32_t>(droppedIndex);
								textureFilePathTable_[meshIndex] = textureStore_->GetFilePath(param_->meshMaterial[meshIndex].hTexture);
							}
							ImGui::EndDragDropTarget();
						}




						ImGui::SeparatorText("Lighting");

						// ライティング有効化
						ImGui::Checkbox("Lighting", &param_->meshMaterial[meshIndex].enableLighting);

						// シャドウマップ描画
						ImGui::Checkbox("DrawShadowMap", &param_->meshMaterial[meshIndex].drawShadowMap);

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

					// アウトライン
					if (PostEffectStore::IsLoadOutline())
					{
						if (ImGui::TreeNode("Outline"))
						{
							// アウトライン有効化
							ImGui::Checkbox("EnableOutline", &param_->meshOutline[meshIndex].enableOutline);

							// アウトライン色
							ImGui::ColorEdit4("Color", &param_->meshOutline[meshIndex].color.x);

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