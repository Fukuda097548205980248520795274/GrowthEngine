#include "SpriteResource.h"
#include "PSO/PSOModel/BasePSOModel.h"
#include "Store/TextureStore/TextureStore.h"
#include "Resource/IndexBufferResource/IndexBufferResource.h"

/// @brief コンストラクタ
/// @param hSprite 
Engine::SpriteResource::SpriteResource(SpriteHandle hSprite, TextureHandle hTexture, std::string name) 
	: hSprite_(hSprite_), name_(name)
{
	// パラメータの生成と初期化
	param_ = std::make_unique<Sprite::Param>();

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
/// @param device 
void Engine::SpriteResource::Initialize(VertexBufferResource<SpriteVertexData>* vertexResource, IndexBufferResource* indexResource, TextureStore* textureStore,
	ID3D12Device* device, Log* log)
{
	// nullptrチェック
	assert(vertexResource);
	assert(indexResource);
	assert(textureStore);
	assert(device);

	// 引数を受け取る
	vertexResource_ = vertexResource;
	indexResource_ = indexResource;
	textureStore_ = textureStore;

	// テクスチャサイズを取得する
	param_->texture.size =
		Vector2(static_cast<float>(textureStore_->GetTextureWidth(param_->texture.hTexture)),
			static_cast<float>(textureStore_->GetTextureHeight(param_->texture.hTexture)));

	// マテリアルリソースの生成
	materialResource_ = std::make_unique<ConstantBufferResource<Sprite::MaterialDataForGPU>>();
	materialResource_->Initialize(device, log);

	// 座標変換リソースの生成
	transformationResource_ = std::make_unique<ConstantBufferResource<Sprite::TransformationDataForGPU>>();
	transformationResource_->Initialize(device, log);
}

/// @brief コマンドリストに登録
/// @param commandList 
void Engine::SpriteResource::Register(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOModel* pso)
{
	Matrix4x4 worldMatrix = Make2DScaleMatrix4x4(Vector2(param_->transform.scale.x * param_->texture.size.x, param_->transform.scale.y * param_->texture.size.y))
		* Make3DRotateZMatrix4x4(param_->transform.rotate) * Make2DTranslateMatrix4x4(param_->transform.translate);

	// ワールドビュープロジェクション行列
	transformationResource_->data_->worldViewProjectionMatrix = worldMatrix * viewProjection;

	// アンカー
	transformationResource_->data_->anchor = param_->texture.anchor;

	// 色
	materialResource_->data_->color = param_->material.color;

	// UV
	materialResource_->data_->uvMatrix =
		Make2DScaleMatrix4x4(param_->material.uv.scale) * Make3DRotateZMatrix4x4(param_->material.uv.rotate) * Make2DTranslateMatrix4x4(param_->material.uv.translate);


	/*--------------------------
	    コマンドリストに登録する
	--------------------------*/

	// PSOの登録
	pso->Register(commandList);

	// インデックスリソースの設定
	indexResource_->Register(commandList);

	// 頂点リソースの設定
	vertexResource_->Register(commandList);

	// 座標変換リソースの設定
	transformationResource_->RegisterGraphics(commandList, 0);

	// マテリアルリソースの設定
	materialResource_->RegisterGraphics(commandList, 1);

	// テクスチャの設定
	commandList->SetGraphicsRootDescriptorTable(2, textureStore_->GetSrvGpuHandle(param_->texture.hTexture));

	// 形状の設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ドローコール
	commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);
}