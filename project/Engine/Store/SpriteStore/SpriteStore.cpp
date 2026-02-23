#include "SpriteStore.h"

/// @brief 初期化
/// @param device 
/// @param log 
void Engine::SpriteStore::Initialize(ID3D12Device* device, Log* log)
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
void Engine::SpriteStore::Update()
{
	// デバッグ用パラメータ
#ifdef _DEVELOPMENT
	DebugParameter();
#endif
}

/// @brief 読み込み
/// @param name 
/// @param hTexture 
/// @param textureStore 
/// @return 
SpriteHandle Engine::SpriteStore::Load(const std::string& name, TextureHandle hTexture, TextureStore* textureStore, ID3D12Device* device, Log* log)
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
	std::unique_ptr<SpriteResource> data = std::make_unique<SpriteResource>(handle, hTexture, name);
	data->Initialize(vertexResource_.get(), indexResource_.get(), textureStore, device, log);
	dataTable_.push_back(std::move(data));

	return handle;
}

/// @brief コマンドリストに登録する
/// @param hSprite 
/// @param viewProjection 
/// @param commandList 
/// @param pso 
void Engine::SpriteStore::Register(SpriteHandle hSprite, const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOModel* pso)
{
	dataTable_[hSprite]->Register(viewProjection, commandList, pso);
}

/// @brief デバッグ用パラメータ
void Engine::SpriteStore::DebugParameter()
{
#ifdef _DEVELOPMENT
	for (auto& data : dataTable_)data->DebugParameter();
#endif
}