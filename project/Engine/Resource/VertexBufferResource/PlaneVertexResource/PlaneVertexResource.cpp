#include "PlaneVertexResource.h"
/// @brief 初期化
/// @param device 
/// @param log 
void Engine::PlaneVertexResource::Initialize(ID3D12Device* device, Log* log)
{
	// インデックスリソースの生成と初期化
	indexResource_ = std::make_unique<IndexBufferResource>();
	indexResource_->Initialize(device, 6, log);

	indexResource_->data_[0] = 0;
	indexResource_->data_[1] = 1;
	indexResource_->data_[2] = 2;
	indexResource_->data_[3] = 1;
	indexResource_->data_[4] = 3;
	indexResource_->data_[5] = 2;


	// 頂点リソースの生成と初期化
	vertexResource_ = std::make_unique<VertexBufferResource<VertexDataForGPU>>();
	vertexResource_->Initialize(device, 4, log);

	// ローカル座標を与える
	vertexResource_->data_[0].position = Vector4(-1.0f, -1.0f, -1.0f, 1.0f);
	vertexResource_->data_[0].texcoord = Vector2(0.0f, 1.0f);
	vertexResource_->data_[0].normal = Vector3(0.0f, 0.0f, -1.0f);
	vertexResource_->data_[1].position = Vector4(-1.0f, 1.0f, -1.0f, 1.0f);
	vertexResource_->data_[1].texcoord = Vector2(0.0f, 0.0f);
	vertexResource_->data_[1].normal = Vector3(0.0f, 0.0f, -1.0f);
	vertexResource_->data_[2].position = Vector4(1.0f, -1.0f, -1.0f, 1.0f);
	vertexResource_->data_[2].texcoord = Vector2(1.0f, 1.0f);
	vertexResource_->data_[2].normal = Vector3(0.0f, 0.0f, -1.0f);
	vertexResource_->data_[3].position = Vector4(1.0f, 1.0f, -1.0f, 1.0f);
	vertexResource_->data_[3].texcoord = Vector2(1.0f, 0.0f);
	vertexResource_->data_[3].normal = Vector3(0.0f, 0.0f, -1.0f);
	vertexResource_->data_[4].position = Vector4(1.0f, -1.0f, -1.0f, 1.0f);
	vertexResource_->data_[4].texcoord = Vector2(0.0f, 1.0f);
	vertexResource_->data_[4].normal = Vector3(1.0f, 0.0f, 0.0f);
}

/// @brief コマンドリストに登録する
/// @param commandList 
void Engine::PlaneVertexResource::Register(ID3D12GraphicsCommandList* commandList)
{
	indexResource_->Register(commandList);
	vertexResource_->Register(commandList);
}