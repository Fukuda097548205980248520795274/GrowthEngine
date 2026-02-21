#include "PrefabStaticModelData.h"
#include "Store/ModelStore/ModelStore.h"
#include "PSO/PSOModel/BasePSOModel.h"
#include "PSO/PSOShadowMap/BasePSOShadowMap.h"

#include "Store/LightStore/LightStore.h"
#include "Store/TextureStore/TextureStore.h"
#include "Store/Camera3DStore/Camera3DStore.h"

/// @brief コンストラクタ
/// @param name 
/// @param numInstance 
/// @param hPrefab 
/// @param hTexture 
Engine::PrefabStaticModelData::PrefabStaticModelData(const std::string& name, uint32_t numInstance, PrefabPrimitiveHandle hPrefab, ModelHandle hModel)
	: hModel_(hModel), PrefabPrimitiveBaseData(name, numInstance, hPrefab)
{
	// 種類
	type_ = Prefab::Type::StaticModel;

	// パラメータを生成する
	param_ = std::make_unique<Prefab::StaticModel::Base::Param>();

	// モデルトランスフォームの初期化
	param_->modelTransform.scale = Vector3(1.0f, 1.0f, 1.0f);
	param_->modelTransform.rotate = Vector3(0.0f, 0.0f, 0.0f);
	param_->modelTransform.translate = Vector3(0.0f, 0.0f, 0.0f);
}

/// @brief 初期化
/// @param modelStore 
/// @param textureStore 
/// @param lightStore 
/// @param cameraStore 
/// @param heap 
/// @param device 
/// @param log 
void Engine::PrefabStaticModelData::Initialize(ModelStore* modelStore, TextureStore* textureStore, LightStore* lightStore, Camera3DStore* cameraStore,
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

	// 領域確保
	param_->meshMaterial.resize(static_cast<int32_t>(modelData.meshes.size()));
	param_->meshTransforms.resize(static_cast<int32_t>(modelData.meshes.size()));
	primitiveResource_.resize(static_cast<int32_t>(modelData.meshes.size()));
	shadowMapTransformationResource_.resize(static_cast<int32_t>(modelData.meshes.size()));

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

		// プリミティブ
		primitiveResource_[meshIndex] = std::make_unique<StructuredBufferResource<Prefab::PrimitiveDataForGPU>>();
		primitiveResource_[meshIndex]->Initialize(device, heap, numInstance_, log);

		// シャドウマップ
		shadowMapTransformationResource_[meshIndex] = std::make_unique<StructuredBufferResource<Matrix4x4>>();
		shadowMapTransformationResource_[meshIndex]->Initialize(device, heap, numInstance_, log);
	}
}

/// @brief 更新処理
void Engine::PrefabStaticModelData::Update()
{
	// 削除されたインスタンスをリストから除外する
	instanceTable_.remove_if([](std::unique_ptr<PrefabInstanceStaticModel>& instance) {if (instance->IsDelete()) { return true; }return false; });
}

/// @brief コマンドリストに登録する
/// @param commandList 
/// @param pso 
void Engine::PrefabStaticModelData::Register(ID3D12GraphicsCommandList* commandList, BasePSOModel* pso)
{
	// モデルデータを取得する
	ModelData modelData = modelStore_->GetModelData(hModel_);


	// PSOの設定
	pso->Register(commandList);

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
void Engine::PrefabStaticModelData::DrawShadowMap(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOShadowMap* pso)
{
	// モデルデータを取得する
	ModelData modelData = modelStore_->GetModelData(hModel_);

	// PSOの設定
	pso->Register(commandList);

	// メッシュごとに処理
	for (int32_t meshIndex = 0; meshIndex < static_cast<int32_t>(modelData.meshes.size()); meshIndex++)
	{
		UINT useInstance = 0;

		// インスタンスごとに処理
		for (auto& instance : instanceTable_)
		{
			// インスタンス数を越えたら処理しない
			if (useInstance >= numInstance_)
				break;

			Quaternion modelQuaternion =
				ToQuaternion(instance->param_.modelTransform.rotate.z, Vector3(0.0f, 0.0, 1.0f)).Normalize() *
				ToQuaternion(instance->param_.modelTransform.rotate.y, Vector3(0.0f, 1.0, 0.0f)).Normalize() *
				ToQuaternion(instance->param_.modelTransform.rotate.x, Vector3(1.0f, 0.0, 0.0f)).Normalize();

			Matrix4x4 worldMatrix = Make3DAffineMatrix4x4(instance->param_.modelTransform.scale, modelQuaternion, instance->param_.modelTransform.translate);

			// ノード行列
			Matrix4x4 nodeMatrix = MakeIdentityMatrix4x4();
			if (!modelData.nodes.empty())nodeMatrix = modelData.nodes[static_cast<int32_t>(modelStore_->GetModelData(hModel_).meshes.size()) - 1 - meshIndex].worldMatrix;

			Quaternion meshQuaternion =
				ToQuaternion(instance->param_.meshTransforms[meshIndex].rotate.z, Vector3(0.0f, 0.0, 1.0f)).Normalize() *
				ToQuaternion(instance->param_.meshTransforms[meshIndex].rotate.y, Vector3(0.0f, 1.0, 0.0f)).Normalize() *
				ToQuaternion(instance->param_.meshTransforms[meshIndex].rotate.x, Vector3(1.0f, 0.0, 0.0f)).Normalize();

			Matrix4x4 localMatrix = Make3DAffineMatrix4x4(instance->param_.meshTransforms[meshIndex].scale, meshQuaternion, instance->param_.meshTransforms[meshIndex].translate);


			// ワールド座標
			shadowMapTransformationResource_[meshIndex]->data_[useInstance] = localMatrix * nodeMatrix * worldMatrix * viewProjection;

			// 使用インスタンスをカウントする
			useInstance++;
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
}

/// @brief インスタンスを生成する
/// @return 
void* Engine::PrefabStaticModelData::CreateInstance()
{
	// インスタンスを生成する
	std::unique_ptr<PrefabInstanceStaticModel> instance =
		std::make_unique<PrefabInstanceStaticModel>([this](const Prefab::StaticModel::Instance::Param* param) {DrawCallInstance(param); }, param_.get());

	// ポインタを保存する
	PrefabInstanceStaticModel* pInstance = instance.get();

	// テーブルに追加する
	instanceTable_.push_back(std::move(instance));

	return pInstance;
}

/// @brief 全てのインスタンスを削除する
void Engine::PrefabStaticModelData::DestroyAllInstance()
{
	instanceTable_.clear();
}

/// @brief インスタンスのドローコール
void Engine::PrefabStaticModelData::DrawCallInstance(const Engine::Prefab::StaticModel::Instance::Param* param)
{
	if (numUseInstance_ >= numInstance_)
		return;

	// モデルデータを取得する
	const ModelData& modelData = modelStore_->GetModelData(hModel_);

	Quaternion modelQuaternion =
		ToQuaternion(param->modelTransform.rotate.z, Vector3(0.0f, 0.0, 1.0f)).Normalize() *
		ToQuaternion(param->modelTransform.rotate.y, Vector3(0.0f, 1.0, 0.0f)).Normalize() *
		ToQuaternion(param->modelTransform.rotate.x, Vector3(1.0f, 0.0, 0.0f)).Normalize();

	Matrix4x4 worldMatrix = Make3DAffineMatrix4x4(param->modelTransform.scale, modelQuaternion, param->modelTransform.translate);

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


		// ワールド座標
		primitiveResource_[meshIndex]->data_[numUseInstance_].world =
			localMatrix * nodeMatrix * worldMatrix;

		// ワールドビュー正射影行列
		primitiveResource_[meshIndex]->data_[numUseInstance_].worldViewProjection =
			primitiveResource_[meshIndex]->data_[numUseInstance_].world * cameraStore_->GetCamera3D().GetViewProjectionMatrix();

		// 逆転置ワールド行列
		primitiveResource_[meshIndex]->data_[numUseInstance_].worldInverseTranspose =
			primitiveResource_[meshIndex]->data_[numUseInstance_].world.Transpose().Inverse();



		// 色
		primitiveResource_[meshIndex]->data_[numUseInstance_].color = param_->meshMaterial[meshIndex].color;

		// UV行列
		primitiveResource_[meshIndex]->data_[numUseInstance_].uvTransform =
			Make3DScaleMatrix4x4(Vector3(param_->meshMaterial[meshIndex].uv.scale.x, param_->meshMaterial[meshIndex].uv.scale.y, 1.0f)) *
			Make3DRotateZMatrix4x4(param_->meshMaterial[meshIndex].uv.radius) *
			Make3DTranslateMatrix4x4(Vector3(param_->meshMaterial[meshIndex].uv.translate.x, param_->meshMaterial[meshIndex].uv.translate.y, 0.0f));
	}

	numUseInstance_++;
}