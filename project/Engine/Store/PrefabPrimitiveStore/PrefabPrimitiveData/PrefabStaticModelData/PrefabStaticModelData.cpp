#include "PrefabStaticModelData.h"
#include "Store/ModelStore/ModelStore.h"

/// @brief コンストラクタ
/// @param name 
/// @param numInstance 
/// @param hPrefab 
/// @param hTexture 
Engine::PrefabStaticModelData::PrefabStaticModelData(const std::string& name, uint32_t numInstance, PrefabPrimitiveHandle hPrefab, ModelHandle hModel)
	: hModel_(hModel), PrefabPrimitiveBaseData(name, numInstance, hPrefab)
{
	// パラメータを生成する
	param_ = std::make_unique<Prefab::StaticModel::Param>();

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


}