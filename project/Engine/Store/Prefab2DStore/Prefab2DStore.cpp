#include "Prefab2DStore.h"
#include "Prefab2DData/Prefab2DSpriteData/Prefab2DSpriteData.h"

#include "ShaderCompiler/ShaderCompiler.h"

/// @brief コンストラクタ
Engine::Prefab2DStore::Prefab2DStore()
{
	// パラメータの生成
	parameter_ = std::make_unique<Prefab2DParameter>("PrefabSprite");
}

/// @brief 初期化
/// @param device 
/// @param compiler 
/// @param log 
void Engine::Prefab2DStore::Initialize(ID3D12Device* device, ShaderCompiler* compiler, Log* log)
{
	// nullptrチェック
	assert(device);


	// 2Dプレハブ頂点シェーダ
	prefab2DVS_ = compiler->Compile(L"./Assets/Shader/Prefab/Prefab2D/Prefab2D.VS.hlsl", L"vs_6_0");
	assert(prefab2DVS_);

	// 2Dプレハブピクセルシェーダ
	prefab2DPS_ = compiler->Compile(L"./Assets/Shader/Prefab/Prefab2D/Prefab2D.PS.hlsl", L"ps_6_0");
	assert(prefab2DPS_);

	// 2Dプレハブ用PSO
	psoPrefab2D_ = std::make_unique<PSOPrefab2D>();
	psoPrefab2D_->Initialize(device, prefab2DVS_.Get(), prefab2DPS_.Get(), log);



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
void Engine::Prefab2DStore::Update()
{
	// データの更新
	for (auto& data : dataTable_)data->Update();
}

/// @brief シーン前リセット
void Engine::Prefab2DStore::PerSceneReset()
{
	for (auto& data : dataTable_)data->PerSceneReset();
}

/// @brief 読み込み
/// @param name 
/// @param hTexture 
/// @param numInstance 
/// @param textureStore 
/// @param device 
/// @param log 
/// @return 
Prefab2DHandle Engine::Prefab2DStore::Load(const std::string& name, TextureHandle hTexture, uint32_t numInstance,
	TextureStore* textureStore, Camera2DStore* cameraStore, DX12Heap* heap, ID3D12Device* device, Log* log)
{
	// nullptrチェック
	assert(textureStore);
	assert(device);

	// 同じデータがないかどうか
	for (auto& data : dataTable_)
	{
		if (name == data->GetName())
		{
			data->Reset();
			return data->GetHandle();
		}
	}

	// ハンドル
	Prefab2DHandle handle = static_cast<Prefab2DHandle>(dataTable_.size());

	// 名前テーブルに記録する
	nameTable_[name] = handle;

	// データの生成と初期化
	std::unique_ptr<Prefab2DSpriteData> data = std::make_unique<Prefab2DSpriteData>(handle, numInstance, name, parameter_.get());
	data->Initialize(vertexResource_.get(), indexResource_.get(), textureStore, hTexture, cameraStore, heap, device, log);
	dataTable_.push_back(std::move(data));

	return handle;
}

/// @brief コマンドリストに登録する
/// @param commandList 
/// @param pso 
void Engine::Prefab2DStore::AllDrawPrefab(ID3D12GraphicsCommandList* commandList)
{
	for (auto& data : dataTable_)data->Register(commandList, psoPrefab2D_.get());
}

/// @brief プレハブの描画処理
/// @param hPrefab2D 
/// @param commandList 
/// @param pso 
void Engine::Prefab2DStore::DrawPrefab(Prefab2DHandle hPrefab2D, ID3D12GraphicsCommandList* commandList)
{
	dataTable_[hPrefab2D]->Register(commandList, psoPrefab2D_.get());
}

/// @brief プレハブの描画処理
/// @param name 
/// @param commandList 
/// @param pso 
void Engine::Prefab2DStore::DrawPrefab(const std::string& name, ID3D12GraphicsCommandList* commandList)
{
	dataTable_[nameTable_[name]]->Register(commandList, psoPrefab2D_.get());
}

/// @brief リセット
void Engine::Prefab2DStore::Reset()
{
	for (auto& data : dataTable_)data->InstanceReset();
}

/// @brief デバッグ用パラメータ
void Engine::Prefab2DStore::DebugParameter()
{
#ifdef _DEVELOPMENT
	for (auto& data : dataTable_)data->DebugParameter();
#endif
}