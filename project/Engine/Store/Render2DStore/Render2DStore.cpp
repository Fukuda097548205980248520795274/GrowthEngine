#include "Render2DStore.h"
#include "Render2DData/Render2DSpriteData/Render2DSpriteData.h"
#include "Render2DData/Render2DTextData/Render2DTextData.h"

#include "Store/Camera2DStore/Camera2DStore.h"

#include "ShaderCompiler/ShaderCompiler.h"

/// @brief コンストラクタ
Engine::Render2DStore::Render2DStore()
{
	// パラメータの生成
	parameter_ = std::make_unique<Render2DParameter>("Sprite");
}

/// @brief 初期化
/// @param device 
/// @param log 
void Engine::Render2DStore::Initialize(ID3D12Device* device, ShaderCompiler* compiler, Log* log)
{
	// nullptrチェック
	assert(device);
	assert(compiler);

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


	// 2D描画頂点シェーダ
	render2DVS_ = compiler->Compile(L"./Assets/Shader/Render/Render2D/Render2D.VS.hlsl", L"vs_6_0");
	assert(render2DVS_);

	// 2D描画ピクセルシェーダ
	render2DPS_ = compiler->Compile(L"./Assets/Shader/Render/Render2D/Render2D.PS.hlsl", L"ps_6_0");
	assert(render2DPS_);

	// テキストピクセルシェーダ
	textPS_ = compiler->Compile(L"./Assets/Shader/Render/Text/Text.PS.hlsl", L"ps_6_0");
	assert(textPS_);


	// 2D描画PSOの生成と初期化
	psoRender2D_ = std::make_unique<PSORender2D>();
	psoRender2D_->Initialize(device, render2DVS_.Get(), render2DPS_.Get(), log);

	// テキストPSOの生成と初期化
	psoText_ = std::make_unique<PSORender2D>();
	psoText_->Initialize(device, render2DVS_.Get(), textPS_.Get(), log);
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
void Engine::Render2DStore::Register(Render2DHandle hRender2D, Camera2DStore* cameraStore, ID3D12GraphicsCommandList* commandList)
{
	// Guizmo操作
#ifdef _DEVELOPMENT
	dataTable_[hRender2D]->DebugGuizmo(cameraStore->GetCamera2D().GetViewMatrix(), cameraStore->GetCamera2D().GetProjectionMatrix());
#endif

	// スプライト
	if (dataTable_[hRender2D]->GetType() == Render2D::Type::Sprite)
	{
		dataTable_[hRender2D]->Register(cameraStore->GetCamera2D().GetCurrentVPMatrix(), commandList, psoRender2D_.get());
	}
	else if(dataTable_[hRender2D]->GetType() == Render2D::Type::Text)
	{
		// テキスト
		dataTable_[hRender2D]->Register(cameraStore->GetCamera2D().GetCurrentVPMatrix(), commandList, psoText_.get());
	}
}

/// @brief コマンドリストに登録する
/// @param name 
/// @param viewProjection 
/// @param commandList 
/// @param pso 
void Engine::Render2DStore::Register(const std::string& name, Camera2DStore* cameraStore, ID3D12GraphicsCommandList* commandList)
{
	// Guizmo操作
#ifdef _DEVELOPMENT
	dataTable_[nameTable_[name]]->DebugGuizmo(cameraStore->GetCamera2D().GetViewMatrix(), cameraStore->GetCamera2D().GetProjectionMatrix());
#endif

	// スプライト
	if (dataTable_[nameTable_[name]]->GetType() == Render2D::Type::Sprite)
	{
		dataTable_[nameTable_[name]]->Register(cameraStore->GetCamera2D().GetCurrentVPMatrix(), commandList, psoRender2D_.get());
	}
	else if (dataTable_[nameTable_[name]]->GetType() == Render2D::Type::Text)
	{
		// テキスト
		dataTable_[nameTable_[name]]->Register(cameraStore->GetCamera2D().GetCurrentVPMatrix(), commandList, psoText_.get());
	}
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
void Engine::Render2DStore::DebugPicking(const Vector2& point, std::vector<std::pair<float, DebugData::DebugGuizmoData*>>& pickList)
{
	for (auto& data : dataTable_)data->DebugPicking(point, pickList);
}