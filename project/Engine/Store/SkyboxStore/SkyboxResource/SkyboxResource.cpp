#include "SkyboxResource.h"

#include "Store/TextureStore/TextureStore.h"
#include "PSO/PSOSkybox/BasePSOSkybox.h"

#include "Resource/IndexBufferResource/IndexBufferResource.h"

/// @brief 初期化
/// @param textureStore 
void Engine::SkyboxResource::Initialize(TextureStore* textureStore, ID3D12Device* device, 
	IndexBufferResource* indexResource, VertexBufferResource<Vector4>* vertexResource, Log* log)
{
	// nullptrチェック
	assert(textureStore);
	assert(device);
	assert(indexResource);
	assert(vertexResource);

	// 引数を受け取る
	textureStore_ = textureStore;
	indexResource_ = indexResource;
	vertexResource_ = vertexResource;

	// テクスチャがキューブマップかどうか
	assert(textureStore_->GetType(hTexture_) == TextureType::Cubemap);

	// パラメータの生成と初期化
	param_ = std::make_unique<SkyboxData::Param>();
	param_->material.color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	param_->transform.scale = Vector3(1000.0f, 1000.0f, 1000.0f);
	param_->transform.rotate = Vector3(0.0f, 0.0f, 0.0f);
	param_->transform.translate = Vector3(0.0f, 0.0f, 0.0f);

	// マテリアルリソースの生成と初期化
	materialResource_ = std::make_unique<ConstantBufferResource<Vector4>>();
	materialResource_->Initialize(device, log);

	// 座標変換リソースの生成と初期化
	transformationResource_ = std::make_unique<ConstantBufferResource<Matrix4x4>>();
	transformationResource_->Initialize(device, log);
}

/// @brief コマンドリストに登録する
/// @param commandList 
/// @param pso 
void Engine::SkyboxResource::Register(ID3D12GraphicsCommandList* commandList, BasePSOSkybox* pso, const Matrix4x4& viewProjection)
{
	/*-------------------
	    データを入れる
	-------------------*/

	// クォータニオン
	Quaternion qRotate =
		ToQuaternion(param_->transform.rotate.z, Vector3(0.0f, 0.0, 1.0f)).Normalize() *
		ToQuaternion(param_->transform.rotate.y, Vector3(0.0f, 1.0, 0.0f)).Normalize() *
		ToQuaternion(param_->transform.rotate.x, Vector3(1.0f, 0.0, 0.0f)).Normalize();

	// 座標変換
	*transformationResource_->data_ = Make3DAffineMatrix4x4(param_->transform.scale, qRotate, param_->transform.translate) * viewProjection;

	// 色
	*materialResource_->data_ = param_->material.color;


	/*-----------------------
	    コマンドリストに登録
	-----------------------*/

	// psoの設定
	pso->Register(commandList);

	// インデックスの設定
	indexResource_->Register(commandList);

	// 頂点の設定
	vertexResource_->Register(commandList);

	// 座標変換の設定
	transformationResource_->RegisterGraphics(commandList, 0);

	// マテリアルの設定
	materialResource_->RegisterGraphics(commandList, 1);

	// テクスチャの設定
	commandList->SetGraphicsRootDescriptorTable(2, textureStore_->GetSrvGpuHandle(hTexture_));

	// 形状の設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ドローコール
	commandList->DrawIndexedInstanced(36, 1, 0, 0, 0);
}

/// @brief キューブマップをコマンドリストに登録する
/// @param commandList 
/// @param rootParameterIndex 
void Engine::SkyboxResource::RegisterCubeMap(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex)
{
	// テクスチャの設定
	commandList->SetGraphicsRootDescriptorTable(rootParameterIndex, textureStore_->GetSrvGpuHandle(hTexture_));
}