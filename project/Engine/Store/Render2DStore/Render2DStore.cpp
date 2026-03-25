#include "Render2DStore.h"
#include "Render2DData/Render2DSpriteData/Render2DSpriteData.h"
#include "Render2DData/Render2DTextData/Render2DTextData.h"

/// @brief コンストラクタ
Engine::Render2DStore::Render2DStore()
{
	// パラメータの生成
	parameter_ = std::make_unique<Render2DParameter>("Sprite");
}

/// @brief 初期化
/// @param device 
/// @param log 
void Engine::Render2DStore::Initialize(ID3D12Device* device, Log* log)
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
void Engine::Render2DStore::Update()
{
	
}

/// @brief シーン前のリセット
void Engine::Render2DStore::PerSceneReset()
{
	for (auto& data : dataTable_)data->PerSceneReset();
}

/// @brief 読み込み
/// @param name 
/// @param hTexture 
/// @param textureStore 
/// @return 
Render2DHandle Engine::Render2DStore::Load(const std::string& name, Render2D::Type type, TextureHandle hTexture, TextHandle hText,
	TextureStore* textureStore, FontStore* fontStore, ID3D12Device* device, Log* log)
{
	// nullptrチェック
	assert(textureStore);
	assert(device);

	// 同じデータがないかどうか
	for (auto& data : dataTable_)
	{
		if (name == data->GetName() && type == data->GetType())
		{
			// リセット
			data->Reset();
			return data->GetHandle();
		}
	}

	// ハンドル
	Render2DHandle handle = static_cast<Render2DHandle>(dataTable_.size());

	// 名前テーブルに記録する
	nameTable_[name] = handle;

	// スプライト
	if (type == Render2D::Type::Sprite)
	{
		// データの生成と初期化
		std::unique_ptr<Render2DSpriteData> data = std::make_unique<Render2DSpriteData>(handle, name, parameter_.get());
		data->Initialize(vertexResource_.get(), indexResource_.get(), textureStore, hTexture, device, log);
		dataTable_.push_back(std::move(data));
	}

	// テキスト
	if (type == Render2D::Type::Text)
	{
		// データの生成と初期化
		std::unique_ptr<Render2DTextData> data = std::make_unique<Render2DTextData>(handle, name, parameter_.get(), hText);
		data->Initialize(vertexResource_.get(), indexResource_.get(), fontStore, device, log);
		dataTable_.push_back(std::move(data));
	}

	return handle;
}

/// @brief コマンドリストに登録する
/// @param hSprite 
/// @param viewProjection 
/// @param commandList 
/// @param pso 
void Engine::Render2DStore::Register(Render2DHandle hRender2D, const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOModel* pso)
{
	// Guizmo操作
#ifdef _DEVELOPMENT
	dataTable_[hRender2D]->DebugGuizmo(viewProjection, viewProjection);
#endif

	dataTable_[hRender2D]->Register(viewProjection, commandList, pso);
}

/// @brief コマンドリストに登録する
/// @param name 
/// @param viewProjection 
/// @param commandList 
/// @param pso 
void Engine::Render2DStore::Register(const std::string& name, const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOModel* pso)
{
	// Guizmo操作
#ifdef _DEVELOPMENT
	dataTable_[nameTable_[name]]->DebugGuizmo(viewProjection, viewProjection);
#endif

	dataTable_[nameTable_[name]]->Register(viewProjection, commandList, pso);
}

/// @brief デバッグ用パラメータ
void Engine::Render2DStore::DebugParameter()
{
#ifdef _DEVELOPMENT
	for (auto& data : dataTable_)data->DebugParameter();
#endif
}

/// @brief デバッグ用ピッキング
/// @param point 
/// @param pickList 
void Engine::Render2DStore::DebugPicking(const Vector2& point, std::vector<std::pair<float, bool*>>& pickList)
{
	for (auto& data : dataTable_)data->DebugPicking(point, pickList);
}