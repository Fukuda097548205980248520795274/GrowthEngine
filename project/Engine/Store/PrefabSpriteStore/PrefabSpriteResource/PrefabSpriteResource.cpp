#include "PrefabSpriteResource.h"
#include "Store/TextureStore/TextureStore.h"
#include "PSO/PSOModel/BasePSOModel.h"
#include "Resource/IndexBufferResource/IndexBufferResource.h"

/// @brief コンストラクタ
/// @param hPrefabSprite 
/// @param hTexture 
/// @param numInstance 
/// @param name 
Engine::PrefabSpriteResource::PrefabSpriteResource(PrefabSpriteHandle hPrefabSprite, TextureHandle hTexture, uint32_t numInstance, const std::string& name)
	: hPrefabSprite_(hPrefabSprite), numInstance_(numInstance) , name_(name)
{
	// パラメータの生成
	param_ = std::make_unique<Prefab::Sprite::Base::Param>();

	// トラスフォーム
	param_->transform.scale = Vector2(1.0f, 1.0f);
	param_->transform.rotate = 0.0f;
	param_->transform.translate = Vector2(0.0f, 0.0f);

	// マテリアル
	param_->material.color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	param_->material.uv.scale = Vector2(1.0f, 1.0f);
	param_->material.uv.rotate = 0.0f;
	param_->material.uv.translate = Vector2(0.0f, 0.0f);

	// テクスチャ
	param_->texture.hTexture = hTexture;
	param_->texture.anchor = Vector2(0.0f, 0.0f);
}

/// @brief 初期化
/// @param vertexResource 
/// @param indexResource 
/// @param textureStore 
/// @param heap 
/// @param device 
/// @param log 
void Engine::PrefabSpriteResource::Initialize(VertexBufferResource<SpriteVertexData>* vertexResource, IndexBufferResource* indexResource,
	TextureStore* textureStore, DX12Heap* heap, ID3D12Device* device, Log* log)
{
	// nullptrチェック
	assert(vertexResource);
	assert(indexResource);
	assert(textureStore);
	assert(heap);
	assert(device);

	// 引数を受け取る
	vertexResource_ = vertexResource;
	indexResource_ = indexResource;
	textureStore_ = textureStore;

	// テクスチャサイズを取得する
	param_->texture.size =
		Vector2(static_cast<float>(textureStore_->GetTextureWidth(param_->texture.hTexture)),
			static_cast<float>(textureStore_->GetTextureHeight(param_->texture.hTexture)));
	
	// リソースの生成と初期化
	resource_ = std::make_unique<StructuredBufferResource<Prefab::SpriteDataForGPU>>();
	resource_->Initialize(device, heap, numInstance_, log);
}

/// @brief コマンドリストに登録する
/// @param commandList 
/// @param pso 
void Engine::PrefabSpriteResource::Register(ID3D12GraphicsCommandList* commandList, BasePSOModel* pso)
{
	// インスタンスがないときは処理しない
	if (useInstance_ <= 0)
		return;

	// PSOの設定
	pso->Register(commandList);

	// インデックスの設定
	indexResource_->Register(commandList);

	// 頂点の設定
	vertexResource_->Register(commandList);

	// パラメータの設定
	resource_->RegisterCompute(commandList, 0);

	// テクスチャの設定
	commandList->SetGraphicsRootDescriptorTable(1, textureStore_->GetSrvGpuHandle(param_->texture.hTexture));

	// 形状の設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ドローコール
	commandList->DrawIndexedInstanced(6, useInstance_, 0, 0, 0);
}