#include "TrailStore.h"
#include <cassert>

/// @brief 初期化
/// @param device
/// @param compiler 
/// @param textureStore 
/// @param camera3DStore 
/// @param log 
void Engine::TrailStore::Initialize(ID3D12Device* device, ShaderCompiler* compiler, TextureStore* textureStore, Camera3DStore* camera3DStore, Log* log)
{
	// nullptrチェック
	assert(device);
	assert(compiler);
	assert(textureStore);
	assert(camera3DStore);

	// 引数を受け取る
	textureStore_ = textureStore;
	camera3DStore_ = camera3DStore;

	// トレイル用PSOの生成
	psoTrail_ = std::make_unique<PSOTrail>();
	psoTrail_->Initialize(device, compiler, log);
}

/// @brief 更新処理
void Engine::TrailStore::Update()
{
	for (auto& data : dataTable_)
	{
		data->Update();
	}
}

/// @brief トレイル読み込み
/// @param name 
/// @param maxLifeTime 
/// @param device 
/// @param log 
/// @return 
TrailHandle Engine::TrailStore::Load(const std::string& name, float maxLifeTime, TextureHandle hTexture, ID3D12Device* device, Log* log)
{
	// 同じ名前のトレイルがすでに存在するか確認する
	auto it = nameTable_.find(name);
	if (it != nameTable_.end())
	{
		return it->second;
	}

	// ハンドルを取得する
	TrailHandle handle = static_cast<TrailHandle>(dataTable_.size());

	// データテーブルに追加する
	std::unique_ptr<TrailData> data = std::make_unique<TrailData>(handle, name, maxLifeTime, hTexture);
	data->Initialize(device, psoTrail_.get(), textureStore_, camera3DStore_);
	dataTable_.push_back(std::move(data));

	// 名前テーブルに追加する
	nameTable_[name] = handle;

	return handle;
}