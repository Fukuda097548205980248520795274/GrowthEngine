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

	// リソース領域確保
	meshTransformationResources_.resize(static_cast<int32_t>(modelData.meshes.size()));
	meshMaterialResources_.resize(static_cast<int32_t>(modelData.meshes.size()));
	shadowMapTransformationResource_.resize(static_cast<int32_t>(modelData.meshes.size()));
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

			parameter_->SetValue(group_, modelData.meshNames[meshIndex] + "_Material_Texture", &textureFilePathTable_[meshIndex]);
		}


		// 座標変換リソース
		meshTransformationResources_[meshIndex] = std::make_unique<ConstantBufferResource<PrimitiveModelTransformationDataForGPU>>();
		meshTransformationResources_[meshIndex]->Initialize(device, log);

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
		outputVertexResource_[meshIndex] = std::make_unique<RWStructuredBufferResource<VertexDataForGPU>>();
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

	Animation animation = animationStore_->GetAnimation(param_->animation.hAnimation);

	// 現在の時間に合わせたスケルトンを取得する
	Skeleton endSkeleton = ApplyBoneAnimation(skeleton_, animation, param_->animation.timer);

	// スケルトンを補間
	LerpSkeleton(skeleton_, endSkeleton, 0.3f);

	// スケルトンの更新
	UpdateSkeleton(skeleton_);

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

	// ビュープロジェクション行列を取得する
	Matrix4x4 viewProjection = cameraStore->GetCamera3D().GetViewProjectionMatrix();



	// PSOの設定
	pso->Register(commandList);

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


		// ワールド座標
		meshTransformationResources_[meshIndex]->data_->worldMatrix =
			localMatrix * worldMatrix;

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


	// PSOの設定
	pso->Register(commandList);

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

/// @brief デバッグ用パラメータ
void Engine::Render3DSkinningModelData::DebugParameter()
{
#ifdef _DEVELOPMENT

	// 読み込んでいないと処理しない
	if (!isLoad_)return;

	// モデル名
	if (ImGui::TreeNode(name_.c_str()))
	{
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

							// 環境
							ImGui::SliderFloat("Environment", &param_->meshMaterial[meshIndex].environment, 0.0f, 1.0f);
						}


						// 終了
						ImGui::TreePop();
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

/// @brief デバッグ用レイピッキング
/// @param ray 
/// @param pickList 
void Engine::Render3DSkinningModelData::DebugRayPicker(const Collision3D::Ray& ray, std::vector<std::pair<float, DebugData::DebugGuizmoData*>>& pickList)
{
	// 選択初期化
	if (guizmoData_.isSelect)
	{
		guizmoData_.isSelect = false;
		return;
	}

	// 読み込んでいないと処理しない
	if (!isLoad_)return;

	Collision3D::AABB aabb;
	aabb.center = param_->modelTransform.translate;
	aabb.radius = Vector3(1.0f, 1.0f, 1.0f);

	if (CollisionCheckFunc(aabb, ray))
	{
		std::pair<float, DebugData::DebugGuizmoData*> pick;
		pick.first = Vector3(aabb.center - ray.start).Length();
		pick.second = &guizmoData_;
		pickList.push_back(pick);
	}
}

/// @brief Guizmo操作
/// @param cameraStore 
void Engine::Render3DSkinningModelData::DebugGuizmo(Camera3DStore* cameraStore)
{
	// 読み込んでいないと処理しない
	if (!isLoad_)return;

	// 選択していないときは処理しない
	if (!guizmoData_.isSelect)
		return;

	// Tキー -> 移動
	if (engine_->GetKeyTrigger(DIK_T))guizmoData_.mode = DebugData::GuizmoMode::Translate;

	// Rキー -> 回転
	if (engine_->GetKeyTrigger(DIK_R))guizmoData_.mode = DebugData::GuizmoMode::Rotate;

	// Sキー -> 拡縮
	if (engine_->GetKeyTrigger(DIK_S))guizmoData_.mode = DebugData::GuizmoMode::Scale;

	Matrix4x4 viewMatrix = cameraStore->GetCamera3D().GetViewMatrix();        // worldMatrix_.Inverse()
	Matrix4x4 projMatrix = cameraStore->GetCamera3D().GetProjectionMatrix();  // いつものやつ

   // 現在のSRTからワールド行列を一度だけ生成する
	Quaternion rotateQ =
		ToQuaternion(param_->modelTransform.rotate.z, Vector3(0.0f, 0.0, 1.0f)).Normalize() *
		ToQuaternion(param_->modelTransform.rotate.y, Vector3(0.0f, 1.0, 0.0f)).Normalize() *
		ToQuaternion(param_->modelTransform.rotate.x, Vector3(1.0f, 0.0, 0.0f)).Normalize();

	// ワールド行列を生成する
	Matrix4x4 worldMatrix =
		Make3DScaleMatrix4x4(param_->modelTransform.scale) *
		Make3DRotateMatrix4x4(rotateQ) *
		Make3DTranslateMatrix4x4(param_->modelTransform.translate);

	// Guizmoの操作モードを切り替える
	ImGuizmo::OPERATION operation = ImGuizmo::TRANSLATE;
	switch (guizmoData_.mode)
	{
	case DebugData::GuizmoMode::Translate:
		operation = ImGuizmo::TRANSLATE;
		break;
	case DebugData::GuizmoMode::Rotate:
		operation = ImGuizmo::ROTATE;
		break;
	case DebugData::GuizmoMode::Scale:
		operation = ImGuizmo::SCALE;
		break;
	}

	// 操作モードだけを切り替えて同じワールド行列を編集する
	ImGuizmo::Manipulate(&viewMatrix.m[0][0], &projMatrix.m[0][0], operation, ImGuizmo::LOCAL, &worldMatrix.m[0][0]);

	if (ImGuizmo::IsUsing())
	{
		float translation[3];
		float rotation[3];
		float scale[3];

		// 変更されたワールド行列からSRTを分解して取得する
		ImGuizmo::DecomposeMatrixToComponents(&worldMatrix.m[0][0], translation, rotation, scale);

		// 度数法(Degrees)から弧度法(Radians)へ変換するための係数
		constexpr float DEG2RAD = std::numbers::pi_v<float> / 180.0f;

		switch (guizmoData_.mode)
		{
		case DebugData::GuizmoMode::Translate:
			// 移動成分抽出
			param_->modelTransform.translate = Vector3(translation[0], translation[1], translation[2]);
			break;
		case DebugData::GuizmoMode::Rotate:
			// 回転成分抽出
			param_->modelTransform.rotate.x = rotation[0] * DEG2RAD;
			param_->modelTransform.rotate.y = rotation[1] * DEG2RAD;
			param_->modelTransform.rotate.z = rotation[2] * DEG2RAD;
			break;
		case DebugData::GuizmoMode::Scale:
			// 拡縮成分抽出
			param_->modelTransform.scale = Vector3(scale[0], scale[1], scale[2]);
			break;
		}
	}
}