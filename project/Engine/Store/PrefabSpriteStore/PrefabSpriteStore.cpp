#include "PrefabSpriteStore.h"

/// @brief 初期化
/// @param device 
/// @param log 
void Engine::PrefabSpriteStore::Initialize(ID3D12Device* device, Log* log)
{
	// nullptrチェック
	assert(device);

	// 頂点リソースの生成と初期化
	vertexResource_ = std::make_unique<VertexBufferResource<SpriteVertexData>>();
	vertexResource_->Initialize(device, 4, log);

	vertexResource_->data_[0].position = Vector4(0.0f, -1.0f, 0.0f, 1.0f);
	vertexResource_->data_[0].texcoord = Vector2(0.0f, 1.0f);

	vertexResource_->data_[1].position = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
	vertexResource_->data_[1].texcoord = Vector2(0.0f, 0.0f);

	vertexResource_->data_[2].position = Vector4(1.0f, -1.0f, 0.0f, 1.0f);
	vertexResource_->data_[2].texcoord = Vector2(1.0f, 1.0f);

	vertexResource_->data_[3].position = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
	vertexResource_->data_[3].texcoord = Vector2(1.0f, 0.0f);


	// インデックスリソースの初期化
	indexResource_ = std::make_unique<IndexBufferResource>();
	indexResource_->Initialize(device, 6, log);

	indexResource_->data_[0] = 0;
	indexResource_->data_[1] = 1;
	indexResource_->data_[2] = 2;
	indexResource_->data_[3] = 1;
	indexResource_->data_[4] = 3;
	indexResource_->data_[5] = 2;
}

/// @brief 更新処理
void Engine::PrefabSpriteStore::Update()
{
	// データの更新
	for (auto& data : dataTable_)data->Update();
}

/// @brief 読み込み
/// @param name 
/// @param hTexture 
/// @param numInstance 
/// @param textureStore 
/// @param device 
/// @param log 
/// @return 
PrefabSpriteHandle Engine::PrefabSpriteStore::Load(const std::string& name, TextureHandle hTexture, uint32_t numInstance,
	TextureStore* textureStore, Camera2DStore* cameraStore, DX12Heap* heap, ID3D12Device* device, Log* log)
{
	// nullptrチェック
	assert(textureStore);
	assert(device);

	// 同じデータがないかどうか
	for (auto& data : dataTable_)
	{
		if (name == data->GetName())
			return data->GetHandle();
	}

	// ハンドル
	SpriteHandle handle = static_cast<SpriteHandle>(dataTable_.size());

	// データの生成と初期化
	std::unique_ptr<PrefabSpriteResource> data = std::make_unique<PrefabSpriteResource>(handle, hTexture, numInstance, name);
	data->Initialize(vertexResource_.get(), indexResource_.get(), textureStore,cameraStore, heap, device, log);
	dataTable_.push_back(std::move(data));

	return handle;
}

/// @brief コマンドリストに登録する
/// @param hSprite 
/// @param viewProjection 
/// @param commandList 
/// @param pso 
void Engine::PrefabSpriteStore::Register(PrefabSpriteHandle hPrefabSprite,  ID3D12GraphicsCommandList* commandList, BasePSOModel* pso)
{
	dataTable_[hPrefabSprite]->Register(commandList, pso);
}

/// @brief リセット
void Engine::PrefabSpriteStore::Reset()
{
	for (auto& data : dataTable_)data->Reset();
}

/// @brief 全てのインスタンスを削除する
void Engine::PrefabSpriteStore::DestroyAllInstance()
{

}