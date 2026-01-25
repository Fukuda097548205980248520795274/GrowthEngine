#include "PrimitiveStaticModelData.h"
#include "Store/ModelStore/ModelStore.h"
#include <cassert>

/// @brief 初期化
/// @param modelStore 
/// @param device 
void Engine::PrimitiveStaticModelData::Initialize(ModelStore* modelStore, ID3D12Device* device, Log* log)
{
	// nullptrチェック
	assert(modelStore);
	assert(device);

	// 引数を受け取る
	modelStore_ = modelStore;



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

	for (int32_t meshIndex = 0; meshIndex < modelData.meshes.size(); ++meshIndex)
	{
		// メッシュトランスフォーム
		meshTransforms_[meshIndex].scale = std::make_unique<Vector3>(1.0f, 1.0f, 1.0f);
		meshTransforms_[meshIndex].rotation = std::make_unique<Vector3>(0.0f, 0.0f, 0.0f);
		meshTransforms_[meshIndex].translate = std::make_unique<Vector3>(0.0f, 0.0f, 0.0f);

		// マテリアルトランスフォーム
		meshMaterials_[meshIndex].color = std::make_unique<Vector4>(1.0f, 1.0f, 1.0f, 1.0f);

		// 座標変換リソース
		meshTransformationResources_[meshIndex] = std::make_unique<PrimitiveModelTransformationResource>();
		meshTransformationResources_[meshIndex]->Initialize(device, log);

		// マテリアルリソース
		meshMaterialResources_[meshIndex] = std::make_unique<PrimitiveModelMaterialResource>();
		meshMaterialResources_[meshIndex]->Initialize(device, log);
	}
}

/// @brief 更新処理
void Engine::PrimitiveStaticModelData::Update()
{

}

/// @brief コマンドリストに登録する
/// @param commandList 
/// @param meshIndex 
void Engine::PrimitiveStaticModelData::Register(ID3D12GraphicsCommandList* commandList, int32_t meshIndex)
{
	Quaternion modelQuaternion = 
		ToQuaternion(modelTransform_.rotation->x, Vector3(1.0f, 0.0f, 0.0f)) * 
		ToQuaternion(modelTransform_.rotation->y, Vector3(0.0f, 1.0f, 0.0f)) * 
		ToQuaternion(modelTransform_.rotation->z, Vector3(0.0f, 0.0f, 1.0f));

	Matrix4x4 worldMatrix = MakeAffineMatrix4x4(*modelTransform_.scale, modelQuaternion, *modelTransform_.translate);


	Quaternion meshQuaternion =
		ToQuaternion(meshTransforms_[meshIndex].rotation->x, Vector3(1.0f, 0.0f, 0.0f)) *
		ToQuaternion(meshTransforms_[meshIndex].rotation->y, Vector3(0.0f, 1.0f, 0.0f)) *
		ToQuaternion(meshTransforms_[meshIndex].rotation->z, Vector3(0.0f, 0.0f, 1.0f));

	Matrix4x4 localMatrix = MakeAffineMatrix4x4(*meshTransforms_[meshIndex].scale, meshQuaternion, *meshTransforms_[meshIndex].translate);





	/*-----------------------
	    コマンドリスト登録
	-----------------------*/

	// 頂点の設定
	modelStore_->Register(commandList, hModel_, meshIndex);

	// 座標変換の設定
	meshTransformationResources_[meshIndex]->data_->worldMatrix = worldMatrix * localMatrix;
	meshTransformationResources_[meshIndex]->data_->worldViewProjectionMatrix = meshTransformationResources_[meshIndex]->data_->worldMatrix;
	meshTransformationResources_[meshIndex]->data_->worldInverseTransposeMatrix = meshTransformationResources_[meshIndex]->data_->worldMatrix.Transpose().Inverse();
	meshTransformationResources_[meshIndex]->Register(commandList, 0);

	// マテリアルの設定
	meshMaterialResources_[meshIndex]->data_->color = *meshMaterials_[meshIndex].color;
	meshMaterialResources_[meshIndex]->Register(commandList, 1);
}