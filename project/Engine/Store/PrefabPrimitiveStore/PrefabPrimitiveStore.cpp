#include "PrefabPrimitiveStore.h"
#include <cassert>

#include "PrefabPrimitiveData/PrefabStaticModelData/PrefabStaticModelData.h"

/// @brief 初期化
/// @param device 
/// @param compiler 
/// @param heap 
/// @param modelStore 
/// @param textureStore 
/// @param animationStore 
/// @param skeletonStore 
/// @param lightStore 
/// @param cameraStore 
/// @param log 
void Engine::PrefabPrimitiveStore::Initialize(ID3D12Device* device, ShaderCompiler* compiler, DX12Heap* heap,
	ModelStore* modelStore, TextureStore* textureStore, AnimationStore* animationStore, SkeletonStore* skeletonStore,
	LightStore* lightStore, Camera3DStore* cameraStore, Log* log)
{
	// nullptrチェック
	assert(device);
	assert(compiler);
	assert(heap);
	assert(modelStore);
	assert(textureStore);
	assert(animationStore);
	assert(skeletonStore);
	assert(lightStore);
	assert(cameraStore);

	// 引数を受け取る
	heap_ = heap;
	modelStore_ = modelStore;
	textureStore_ = textureStore;
	animationStore_ = animationStore;
	skeletonStore_ = skeletonStore;
	lightStore_ = lightStore;
	cameraStore_ = cameraStore;
}

/// @brief プレハブの読み込み
/// @param device 
/// @param commandList 
/// @param hModel 
/// @param hAnimation 
/// @param hSkeleton 
/// @param name 
/// @param numInstance 
/// @param type 
/// @param log 
PrefabPrimitiveHandle Engine::PrefabPrimitiveStore::Load(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, ModelHandle hModel, AnimationHandle hAnimation, SkeletonHandle hSkeleton,
	const std::string& name, uint32_t numInstance, Prefab::Type type, Log* log)
{
	// nullptrチェック
	assert(device);
	assert(commandList);

	// 同じデータがないか探す
	for (auto& data : dataTable_)
	{
		if (data->GetName() == name && data->GetType() == type)
			return data->GetHandle();
	}

	// ハンドルを取得する
	PrefabPrimitiveHandle handle = static_cast<PrefabPrimitiveHandle>(dataTable_.size());

	// 静的モデルプレハブデータ
	if (type == Prefab::Type::StaticModel)
	{
		std::unique_ptr<PrefabStaticModelData> data = std::make_unique<PrefabStaticModelData>(name, numInstance, handle, hModel);
		data->Initialize(modelStore_, textureStore_, lightStore_, cameraStore_, heap_, device, log);
		dataTable_.push_back(std::move(data));

		return handle;
	}

	assert(false);
	return 0;
}

/// @brief 更新処理
void Engine::PrefabPrimitiveStore::Update()
{
	for (auto& data : dataTable_)data->Update();
}

/// @brief コマンドリストに登録する
/// @param hPrefabPrimitive 
/// @param commandList 
/// @param pso 
void Engine::PrefabPrimitiveStore::Register(PrefabPrimitiveHandle hPrefabPrimitive, ID3D12GraphicsCommandList* commandList, BasePSOModel* pso)
{
	dataTable_[hPrefabPrimitive]->Register(commandList, pso);
}

/// @brief シャドウマップの描画処理
/// @param viewProjection 
/// @param commandList 
/// @param pso 
void Engine::PrefabPrimitiveStore::ShadowMapDraw(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOShadowMap* pso)
{
	for (auto& data : dataTable_)data->DrawShadowMap(viewProjection, commandList, pso);
}

/// @brief リセット
void Engine::PrefabPrimitiveStore::Reset()
{
	for (auto& data : dataTable_)data->Reset();
}

/// @brief 全てのインスタンスを削除する
void Engine::PrefabPrimitiveStore::DestroyAllInstance()
{
	for (auto& data : dataTable_)data->DestroyAllInstance();
}