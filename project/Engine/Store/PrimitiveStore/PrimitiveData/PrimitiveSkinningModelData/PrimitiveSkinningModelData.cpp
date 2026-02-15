#include "PrimitiveSkinningModelData.h"
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

#include <numbers>

/// @brief 初期化
/// @param modelStore 
/// @param device 
void Engine::PrimitiveSkinningModelData::Initialize(ModelStore* modelStore, TextureStore* textureStore, AnimationStore* animationStore, SkeletonStore* skeletonStore,
	DX12Heap* heap, ID3D12Device* device, ID3D12GraphicsCommandList* commandList, Log* log)
{
	// nullptrチェック
	assert(modelStore);
	assert(textureStore);
	assert(animationStore);
	assert(skeletonStore);
	assert(device);

	// 引数を受け取る
	modelStore_ = modelStore;
	textureStore_ = textureStore;
	animationStore_ = animationStore;
	skeletonStore_ = skeletonStore;


	// パラメータの生成
	param_ = std::make_unique<SkinningModel::Param>();


	// モデルトランスフォーム
	param_->modelTransform.scale = Vector3(1.0f, 1.0f, 1.0f);
	param_->modelTransform.rotate = Vector3(0.0f, 0.0f, 0.0f);
	param_->modelTransform.translate = Vector3(0.0f, 0.0f, 0.0f);

	// アニメーション
	param_->animation.timer = 0.0f;

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
}

/// @brief 更新処理
void Engine::PrimitiveSkinningModelData::Update()
{
	Animation animation = animationStore_->GetAnimation(hAnimation_);

	ApplyBoneAnimation(skeleton_, animation, param_->animation.timer);

	// スケルトンの更新
	UpdateSkeleton(skeleton_);

	// モデルデータを取得する
	const ModelData& modelData = modelStore_->GetModelData(hModel_);

	for (int32_t meshIndex = 0; meshIndex < static_cast<int32_t>(modelData.meshes.size()); meshIndex++)
	{
		skinClusters_[meshIndex]->Update(skeleton_);
	}
}

/// @brief スキニングを行う
/// @param commandList 
/// @param pso 
void Engine::PrimitiveSkinningModelData::Skinning(ID3D12GraphicsCommandList* commandList, ComputePSOSkinning* pso)
{
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
void Engine::PrimitiveSkinningModelData::Register(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOModel* pso, LightStore* lightStore)
{
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

		// UV行列
		meshMaterialResources_[meshIndex]->data_->uvMatrix =
			Make3DScaleMatrix4x4(Vector3(param_->meshMaterial[meshIndex].uv.scale.x, param_->meshMaterial[meshIndex].uv.scale.y, 1.0f)) *
			Make3DRotateZMatrix4x4(param_->meshMaterial[meshIndex].uv.radius) *
			Make3DTranslateMatrix4x4(Vector3(param_->meshMaterial[meshIndex].uv.translate.x, param_->meshMaterial[meshIndex].uv.translate.y, 0.0f));


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
		lightStore->GetShadowMapTextureResource()->Register(commandList, 3);

		// シャドウ用座標変換の設定
		lightStore->GetShadowMapTransformationResource()->RegisterGraphics(commandList, 4);

		// 形状の設定
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// ドローコール
		commandList->DrawIndexedInstanced(static_cast<UINT>(modelStore_->GetModelData(hModel_).meshes[meshIndex].indices.size()), 1, 0, 0, 0);

		outputVertexResource_[meshIndex]->Barrier(commandList, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	}
}

/// @brief コマンドリスト
/// @param commandList 
/// @param pso 
void Engine::PrimitiveSkinningModelData::Register(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOShadowMap* pso)
{
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