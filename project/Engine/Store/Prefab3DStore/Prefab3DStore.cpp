#include "Prefab3DStore.h"
#include <cassert>

#include "Prefab3DData/Prefab3DStaticModelData/Prefab3DStaticModelData.h"
#include "Prefab3DData/Prefab3DCubeData/Prefab3DCubeData.h"

/// @brief コンストラクタ
Engine::Prefab3DStore::Prefab3DStore()
{
	// パラメータの生成
	parameter_ = std::make_unique<Prefab3DParameter>("PrefabPrimitive");
}

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
void Engine::Prefab3DStore::Initialize(ID3D12Device* device, ShaderCompiler* compiler, DX12Heap* heap,
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

	// 立方体頂点リソースの生成と初期化
	cubeVertexResource_ = std::make_unique<CubeVertexResource>();
	cubeVertexResource_->Initialize(device, log);
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
Prefab3DHandle Engine::Prefab3DStore::Load(ID3D12Device* device, ID3D12GraphicsCommandList* commandList,
	TextureHandle hTexture, ModelHandle hModel, AnimationHandle hAnimation, SkeletonHandle hSkeleton,
	const std::string& name, uint32_t numInstance, Prefab3D::Type type, Log* log)
{
	// nullptrチェック
	assert(device);
	assert(commandList);

	// 同じデータがないか探す
	for (auto& data : dataTable_)
	{
		if (data->GetName() == name && data->GetType() == type)
		{
			// リセット
			data->Reset();
			return data->GetHandle();
		}
	}

	// ハンドルを取得する
	Prefab3DHandle handle = static_cast<Prefab3DHandle>(dataTable_.size());

	// 名前テーブルに記録する
	nameTable_[name] = handle;

	// 静的モデルプレハブデータ
	if (type == Prefab3D::Type::StaticModel)
	{
		std::unique_ptr<Prefab3DStaticModelData> data = std::make_unique<Prefab3DStaticModelData>(name, numInstance, handle, hModel, parameter_.get());
		data->Initialize(modelStore_, textureStore_, lightStore_, cameraStore_, heap_, device, log);
		dataTable_.push_back(std::move(data));

		return handle;
	}

	// 立方体プレハブデータ
	if (type == Prefab3D::Type::Cube)
	{
		std::unique_ptr<Prefab3DCubeData> data = std::make_unique<Prefab3DCubeData>(name, numInstance, handle, hTexture, parameter_.get());
		data->Initialize(textureStore_, lightStore_, cameraStore_,cubeVertexResource_.get(), heap_, device, log);
		dataTable_.push_back(std::move(data));

		return handle;
	}

	assert(false);
	return 0;
}

/// @brief 更新処理
void Engine::Prefab3DStore::Update()
{
	for (auto& data : dataTable_)data->Update();
}

/// @brief コマンドリストに登録する
/// @param hPrefabPrimitive 
/// @param commandList 
/// @param pso 
void Engine::Prefab3DStore::Register(SkyboxStore* skyboxStore, ID3D12GraphicsCommandList* commandList, BasePSOModel* pso)
{
	for (auto& data : dataTable_)data->Register(skyboxStore, commandList, pso);
}

/// @brief シャドウマップの描画処理
/// @param viewProjection 
/// @param commandList 
/// @param pso 
void Engine::Prefab3DStore::ShadowMapDraw(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOShadowMap* pso)
{
	for (auto& data : dataTable_)data->DrawShadowMap(viewProjection, commandList, pso);
}

/// @brief リセット
void Engine::Prefab3DStore::Reset()
{
	for (auto& data : dataTable_)data->InstanceReset();
}

/// @brief シーン前のリセット
void Engine::Prefab3DStore::PerSceneReset()
{
	for (auto& data : dataTable_)data->PerSceneReset();
}

/// @brief デバッグ用パラメータ
void Engine::Prefab3DStore::DebugParameter()
{
#ifdef _DEVELOPMENT
	for (auto& data : dataTable_)data->DebugParameter();
#endif
}