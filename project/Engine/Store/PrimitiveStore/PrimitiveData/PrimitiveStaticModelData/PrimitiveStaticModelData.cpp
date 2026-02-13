#include "PrimitiveStaticModelData.h"
#include "Store/ModelStore/ModelStore.h"
#include "Store/TextureStore/TextureStore.h"
#include <cassert>
#include "PSO/PSOModel/BasePSOModel.h"
#include "PSO/PSOShadowMap/BasePSOShadowMap.h"
#include "Store/LightStore/LightStore.h"

#include <numbers>

/// @brief 初期化
/// @param modelStore 
/// @param device 
void Engine::PrimitiveStaticModelData::Initialize(ModelStore* modelStore, TextureStore* textureStore, ID3D12Device* device, Log* log)
{
	// nullptrチェック
	assert(modelStore);
	assert(textureStore);
	assert(device);

	// 引数を受け取る
	modelStore_ = modelStore;
	textureStore_ = textureStore;



	// モデルトランスフォーム
	modelTransform_.scale = std::make_unique<Vector3>(1.0f, 1.0f, 1.0f);
	modelTransform_.rotation = std::make_unique<Vector3>(0.0f, 0.0f, 0.0f);
	modelTransform_.translate = std::make_unique<Vector3>(0.0f, 0.0f, 0.0f);



	// モデルデータを取得する
	const ModelData& modelData = modelStore_->GetModelData(hModel_);

	// 領域確保
	meshTransforms_.resize(static_cast<int32_t>(modelData.meshes.size()));
	meshTransformationResources_.resize(static_cast<int32_t>(modelData.meshes.size()));
	meshMaterials_.resize(static_cast<int32_t>(modelData.meshes.size()));
	meshMaterialResources_.resize(static_cast<int32_t>(modelData.meshes.size()));
	shadowMapTransformationResource_.resize(static_cast<int32_t>(modelData.meshes.size()));

	for (int32_t meshIndex = 0; meshIndex < modelData.meshes.size(); ++meshIndex)
	{
		// メッシュトランスフォーム
		meshTransforms_[meshIndex].scale = std::make_unique<Vector3>(1.0f, 1.0f, 1.0f);
		meshTransforms_[meshIndex].rotation = std::make_unique<Vector3>(0.0f, 0.0f, 0.0f);
		meshTransforms_[meshIndex].translate = std::make_unique<Vector3>(0.0f, 0.0f, 0.0f);

		// マテリアルトランスフォーム
		meshMaterials_[meshIndex].hTexture_ = std::make_unique<TextureHandle>(modelData.meshes[meshIndex].material.handle);
		meshMaterials_[meshIndex].color = std::make_unique<Vector4>(1.0f, 1.0f, 1.0f, 1.0f);
		meshMaterials_[meshIndex].uv.scale = std::make_unique<Vector2>(1.0f, 1.0f);
		meshMaterials_[meshIndex].uv.rotation = std::make_unique<float>(0.0f);
		meshMaterials_[meshIndex].uv.translate = std::make_unique<Vector2>(0.0f, 0.0f);

		// 座標変換リソース
		meshTransformationResources_[meshIndex] = std::make_unique<PrimitiveModelTransformationResource>();
		meshTransformationResources_[meshIndex]->Initialize(device, log);

		// マテリアルリソース
		meshMaterialResources_[meshIndex] = std::make_unique<PrimitiveModelMaterialResource>();
		meshMaterialResources_[meshIndex]->Initialize(device, log);

		// シャドウマップ用座標変換リソース
		shadowMapTransformationResource_[meshIndex] = std::make_unique<ShadowMapTransformationResource>();
		shadowMapTransformationResource_[meshIndex]->Initialize(device, log);
	}
}

/// @brief 更新処理
void Engine::PrimitiveStaticModelData::Update(const Matrix4x4& viewProjection)
{
	// モデルデータを取得する
	const ModelData& modelData = modelStore_->GetModelData(hModel_);

	Quaternion modelQuaternion =
		ToQuaternion(modelTransform_.rotation->z, Vector3(0.0f, 0.0, 1.0f)).Normalize()*
		ToQuaternion(modelTransform_.rotation->y, Vector3(0.0f, 1.0, 0.0f)).Normalize()*
		ToQuaternion(modelTransform_.rotation->x, Vector3(1.0f, 0.0, 0.0f)).Normalize();

	Matrix4x4 worldMatrix = Make3DAffineMatrix4x4(*modelTransform_.scale, modelQuaternion, *modelTransform_.translate);

	for (int meshIndex = 0; meshIndex < static_cast<int32_t>(modelStore_->GetModelData(hModel_).meshes.size()); meshIndex++)
	{
		// ノード行列
		Matrix4x4 nodeMatrix = MakeIdentityMatrix4x4();
		if (!modelData.nodes.empty())nodeMatrix = modelData.nodes[static_cast<int32_t>(modelStore_->GetModelData(hModel_).meshes.size()) - 1 - meshIndex].worldMatrix;

		Quaternion meshQuaternion =
			ToQuaternion(meshTransforms_[meshIndex].rotation->z, Vector3(0.0f, 0.0, 1.0f)).Normalize()*
			ToQuaternion(meshTransforms_[meshIndex].rotation->y, Vector3(0.0f, 1.0, 0.0f)).Normalize()*
			ToQuaternion(meshTransforms_[meshIndex].rotation->x, Vector3(1.0f, 0.0, 0.0f)).Normalize();

		Matrix4x4 localMatrix = Make3DAffineMatrix4x4(*meshTransforms_[meshIndex].scale, meshQuaternion, *meshTransforms_[meshIndex].translate);


		// ワールド座標
		meshTransformationResources_[meshIndex]->data_->worldMatrix =
			localMatrix * worldMatrix * nodeMatrix;

		// ワールドビュー正射影行列
		meshTransformationResources_[meshIndex]->data_->worldViewProjectionMatrix =
			meshTransformationResources_[meshIndex]->data_->worldMatrix * viewProjection;

		// 逆転置ワールド行列
		meshTransformationResources_[meshIndex]->data_->worldInverseTransposeMatrix =
			meshTransformationResources_[meshIndex]->data_->worldMatrix.Transpose().Inverse();



		// 色
		meshMaterialResources_[meshIndex]->data_->color = *meshMaterials_[meshIndex].color;

		// UV行列
		meshMaterialResources_[meshIndex]->data_->uvMatrix =
			Make3DScaleMatrix4x4(Vector3(meshMaterials_[meshIndex].uv.scale->x, meshMaterials_[meshIndex].uv.scale->y, 1.0f)) *
			Make3DRotateZMatrix4x4(*meshMaterials_[meshIndex].uv.rotation) *
			Make3DTranslateMatrix4x4(Vector3(meshMaterials_[meshIndex].uv.translate->x, meshMaterials_[meshIndex].uv.translate->y, 0.0f));
	}
}

/// @brief シャドウマップ用更新処理
/// @param viewProjection 
void Engine::PrimitiveStaticModelData::ShadowMapUpdate(const Matrix4x4& viewProjection)
{
	// モデルデータを取得する
	const ModelData& modelData = modelStore_->GetModelData(hModel_);

	Quaternion modelQuaternion =
		ToQuaternion(modelTransform_.rotation->z, Vector3(0.0f, 0.0, 1.0f)).Normalize() *
		ToQuaternion(modelTransform_.rotation->y, Vector3(0.0f, 1.0, 0.0f)).Normalize() *
		ToQuaternion(modelTransform_.rotation->x, Vector3(1.0f, 0.0, 0.0f)).Normalize();

	Matrix4x4 worldMatrix = Make3DAffineMatrix4x4(*modelTransform_.scale, modelQuaternion, *modelTransform_.translate);

	for (int meshIndex = 0; meshIndex < static_cast<int32_t>(modelStore_->GetModelData(hModel_).meshes.size()); meshIndex++)
	{
		// ノード行列
		Matrix4x4 nodeMatrix = MakeIdentityMatrix4x4();
		if (!modelData.nodes.empty())nodeMatrix = modelData.nodes[static_cast<int32_t>(modelStore_->GetModelData(hModel_).meshes.size()) - 1 - meshIndex].worldMatrix;

		Quaternion meshQuaternion =
			ToQuaternion(meshTransforms_[meshIndex].rotation->z, Vector3(0.0f, 0.0, 1.0f)).Normalize() *
			ToQuaternion(meshTransforms_[meshIndex].rotation->y, Vector3(0.0f, 1.0, 0.0f)).Normalize() *
			ToQuaternion(meshTransforms_[meshIndex].rotation->x, Vector3(1.0f, 0.0, 0.0f)).Normalize();

		Matrix4x4 localMatrix = Make3DAffineMatrix4x4(*meshTransforms_[meshIndex].scale, meshQuaternion, *meshTransforms_[meshIndex].translate);


		// ワールド座標
		*shadowMapTransformationResource_[meshIndex]->data_ = localMatrix * worldMatrix *  nodeMatrix * viewProjection;
	}
}

/// @brief コマンドリストに登録する
/// @param commandList 
/// @param pso 
/// @param textureStore 
void Engine::PrimitiveStaticModelData::Register(ID3D12GraphicsCommandList* commandList, BasePSOModel* pso, LightStore* lightStore)
{
	// PSOの設定
	pso->Register(commandList);

	for (int32_t meshIndex = 0; meshIndex < static_cast<int32_t>(modelStore_->GetModelData(hModel_).meshes.size()); meshIndex++)
	{
		// 頂点の設定
		modelStore_->Register(commandList, hModel_, meshIndex);

		// 座標変換の設定
		meshTransformationResources_[meshIndex]->Register(commandList, 0);

		// マテリアルの設定
		meshMaterialResources_[meshIndex]->Register(commandList, 1);

		// テクスチャの設定
		commandList->SetGraphicsRootDescriptorTable(2, textureStore_->GetSrvGpuHandle(*meshMaterials_[meshIndex].hTexture_));

		// シャドウマップテクスチャの設定
		lightStore->GetShadowMapTextureResource()->Register(commandList, 3);

		// シャドウ用座標変換の設定
		lightStore->GetShadowMapTransformationResource()->Register(commandList, 4);

		// 形状の設定
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// ドローコール
		commandList->DrawIndexedInstanced(static_cast<UINT>(modelStore_->GetModelData(hModel_).meshes[meshIndex].indices.size()), 1, 0, 0, 0);
	}
}

/// @brief コマンドリスト
/// @param commandList 
/// @param pso 
void Engine::PrimitiveStaticModelData::Register(ID3D12GraphicsCommandList* commandList, BasePSOShadowMap* pso)
{
	// PSOの設定
	pso->Register(commandList);

	for (int32_t meshIndex = 0; meshIndex < static_cast<int32_t>(modelStore_->GetModelData(hModel_).meshes.size()); meshIndex++)
	{
		// 頂点の設定
		modelStore_->Register(commandList, hModel_, meshIndex);

		// 座標変換の設定
		shadowMapTransformationResource_[meshIndex]->Register(commandList, 0);

		// 形状の設定
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// ドローコール
		commandList->DrawIndexedInstanced(static_cast<UINT>(modelStore_->GetModelData(hModel_).meshes[meshIndex].indices.size()), 1, 0, 0, 0);
	}
}