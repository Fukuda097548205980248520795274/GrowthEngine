#include "SkyboxStore.h"
#include "Store/TextureStore/TextureStore.h"

/// @brief 初期化
/// @param device 
/// @param compiler 
/// @param log 
void Engine::SkyboxStore::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, 
	ShaderCompiler* compiler, TextureStore* textureStore, DX12Heap* heap, Log* log)
{
	// nullptrチェック
	assert(device);
	assert(commandList);
	assert(compiler);
	assert(textureStore);
	assert(heap);

	// スカイボックスPSOの生成と初期化
	psoSkybox_ = std::make_unique<PSOSkybox>();
	psoSkybox_->Initialize(device, compiler, log);


	// インデックスリソースの生成と初期化
	indexResource_ = std::make_unique<IndexBufferResource>();
	indexResource_->Initialize(device, 36, log);

	// 番号を与える
	for (int32_t i = 0; i < 6; ++i)
	{
		int32_t startIndex = i * 6;
		int32_t index = i * 4;

		indexResource_->data_[startIndex] = index;
		indexResource_->data_[startIndex + 1] = index + 1;
		indexResource_->data_[startIndex + 2] = index + 2;
		indexResource_->data_[startIndex + 3] = index + 1;
		indexResource_->data_[startIndex + 4] = index + 3;
		indexResource_->data_[startIndex + 5] = index + 2;
	}


	// 頂点リソースの生成と初期化
	vertexResource_ = std::make_unique<VertexBufferResource<Vector4>>();
	vertexResource_->Initialize(device, 24, log);

	// ローカル座標を与える
	vertexResource_->data_[0] = Vector4(-1.0f, -1.0f, -1.0f, 1.0f);
	vertexResource_->data_[1] = Vector4(-1.0f, 1.0f, -1.0f, 1.0f);
	vertexResource_->data_[2] = Vector4(1.0f, -1.0f, -1.0f, 1.0f);
	vertexResource_->data_[3] = Vector4(1.0f, 1.0f, -1.0f, 1.0f);

	vertexResource_->data_[4] = Vector4(1.0f, -1.0f, -1.0f, 1.0f);
	vertexResource_->data_[5] = Vector4(1.0f, 1.0f, -1.0f, 1.0f);
	vertexResource_->data_[6] = Vector4(1.0f, -1.0f, 1.0f, 1.0f);
	vertexResource_->data_[7] = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

	vertexResource_->data_[8] = Vector4(1.0f, -1.0f, 1.0f, 1.0f);
	vertexResource_->data_[9] = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	vertexResource_->data_[10] = Vector4(-1.0f, -1.0f, 1.0f, 1.0f);
	vertexResource_->data_[11] = Vector4(-1.0f, 1.0f, 1.0f, 1.0f);

	vertexResource_->data_[12] = Vector4(-1.0f, -1.0f, 1.0f, 1.0f);
	vertexResource_->data_[13] = Vector4(-1.0f, 1.0f, 1.0f, 1.0f);
	vertexResource_->data_[14] = Vector4(-1.0f, -1.0f, -1.0f, 1.0f);
	vertexResource_->data_[15] = Vector4(-1.0f, 1.0f, -1.0f, 1.0f);

	vertexResource_->data_[16] = Vector4(-1.0f, 1.0f, -1.0f, 1.0f);
	vertexResource_->data_[17] = Vector4(-1.0f, 1.0f, 1.0f, 1.0f);
	vertexResource_->data_[18] = Vector4(1.0f, 1.0f, -1.0f, 1.0f);
	vertexResource_->data_[19] = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

	vertexResource_->data_[20] = Vector4(1.0f, -1.0f, -1.0f, 1.0f);
	vertexResource_->data_[21] = Vector4(1.0f, -1.0f, 1.0f, 1.0f);
	vertexResource_->data_[22] = Vector4(-1.0f, -1.0f, -1.0f, 1.0f);
	vertexResource_->data_[23] = Vector4(-1.0f, -1.0f, 1.0f, 1.0f);


	// 初期読み込み
	currentHandle_ = InitialLoad(textureStore->Load("./Assets/Textures/rostock_laage_airport_4k.dds", heap, device, commandList, log), textureStore, device, log);
}

/// @brief 読み込む
/// @param name 
/// @param hTexture 
SkyboxHandle Engine::SkyboxStore::Load(const std::string& name, TextureHandle hTexture, 
	TextureStore* textureStore,ID3D12Device* device, Log* log)
{
	// 同じデータがあるか探す
	for (auto& data : dataTable_)
	{
		if (data->GetName() == name)
			return data->GetHandle();
	}

	// ハンドル
	SkyboxHandle hSkybox = static_cast<SkyboxHandle>(dataTable_.size());

	// 初めての読み込みは自動で切り替え
	if (dataTable_.size() == 1)currentHandle_ = hSkybox;

	// データを生成
	std::unique_ptr<SkyboxResource> data = std::make_unique<SkyboxResource>(name, hTexture, hSkybox);
	data->Initialize(textureStore, device, indexResource_.get(), vertexResource_.get(), log);
	dataTable_.push_back(std::move(data));

	return hSkybox;
}

/// @brief 初期用読み込み
/// @param hTexture 
/// @param textureStore 
/// @param device 
/// @param log 
SkyboxHandle Engine::SkyboxStore::InitialLoad(TextureHandle hTexture, TextureStore* textureStore, ID3D12Device* device, Log* log)
{
	// ハンドル
	SkyboxHandle hSkybox = static_cast<SkyboxHandle>(dataTable_.size());

	// データを生成
	std::unique_ptr<SkyboxResource> data = std::make_unique<SkyboxResource>("Initial", hTexture, hSkybox);
	data->Initialize(textureStore, device, indexResource_.get(), vertexResource_.get(), log);
	dataTable_.push_back(std::move(data));

	return hSkybox;
}