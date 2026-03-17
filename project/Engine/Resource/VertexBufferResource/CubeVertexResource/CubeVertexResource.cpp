#include "CubeVertexResource.h"

/// @brief 初期化
/// @param device 
/// @param log 
void Engine::CubeVertexResource::Initialize(ID3D12Device* device, Log* log)
{
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
	vertexResource_ = std::make_unique<VertexBufferResource<VertexDataForGPU>>();
	vertexResource_->Initialize(device, 24, log);


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
	vertexResource_->data_[5].position = Vector4(1.0f, 1.0f, -1.0f, 1.0f);
	vertexResource_->data_[5].texcoord = Vector2(0.0f, 0.0f);
	vertexResource_->data_[5].normal = Vector3(1.0f, 0.0f, 0.0f);
	vertexResource_->data_[6].position = Vector4(1.0f, -1.0f, 1.0f, 1.0f);
	vertexResource_->data_[6].texcoord = Vector2(1.0f, 1.0f);
	vertexResource_->data_[6].normal = Vector3(1.0f, 0.0f, 0.0f);
	vertexResource_->data_[7].position = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	vertexResource_->data_[7].texcoord = Vector2(1.0f, 0.0f);
	vertexResource_->data_[7].normal = Vector3(1.0f, 0.0f, 0.0f);

	vertexResource_->data_[8].position = Vector4(1.0f, -1.0f, 1.0f, 1.0f);
	vertexResource_->data_[8].texcoord = Vector2(0.0f, 1.0f);
	vertexResource_->data_[8].normal = Vector3(0.0f, 0.0f, 1.0f);
	vertexResource_->data_[9].position = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	vertexResource_->data_[9].texcoord = Vector2(0.0f, 0.0f);
	vertexResource_->data_[9].normal = Vector3(0.0f, 0.0f, 1.0f);
	vertexResource_->data_[10].position = Vector4(-1.0f, -1.0f, 1.0f, 1.0f);
	vertexResource_->data_[10].texcoord = Vector2(1.0f, 1.0f);
	vertexResource_->data_[10].normal = Vector3(0.0f, 0.0f, 1.0f);
	vertexResource_->data_[11].position = Vector4(-1.0f, 1.0f, 1.0f, 1.0f);
	vertexResource_->data_[11].texcoord = Vector2(1.0f, 0.0f);
	vertexResource_->data_[11].normal = Vector3(0.0f, 0.0f, 1.0f);

	vertexResource_->data_[12].position = Vector4(-1.0f, -1.0f, 1.0f, 1.0f);
	vertexResource_->data_[12].texcoord = Vector2(0.0f, 1.0f);
	vertexResource_->data_[12].normal = Vector3(-1.0f, 0.0f, 0.0f);
	vertexResource_->data_[13].position = Vector4(-1.0f, 1.0f, 1.0f, 1.0f);
	vertexResource_->data_[13].texcoord = Vector2(0.0f, 0.0f);
	vertexResource_->data_[13].normal = Vector3(-1.0f, 0.0f, 0.0f);
	vertexResource_->data_[14].position = Vector4(-1.0f, -1.0f, -1.0f, 1.0f);
	vertexResource_->data_[14].texcoord = Vector2(1.0f, 1.0f);
	vertexResource_->data_[14].normal = Vector3(-1.0f, 0.0f, 0.0f);
	vertexResource_->data_[15].position = Vector4(-1.0f, 1.0f, -1.0f, 1.0f);
	vertexResource_->data_[15].texcoord = Vector2(1.0f, 0.0f);
	vertexResource_->data_[15].normal = Vector3(-1.0f, 0.0f, 0.0f);

	vertexResource_->data_[16].position = Vector4(-1.0f, 1.0f, -1.0f, 1.0f);
	vertexResource_->data_[16].texcoord = Vector2(0.0f, 1.0f);
	vertexResource_->data_[16].normal = Vector3(0.0f, 1.0f, 0.0f);
	vertexResource_->data_[17].position = Vector4(-1.0f, 1.0f, 1.0f, 1.0f);
	vertexResource_->data_[17].texcoord = Vector2(0.0f, 0.0f);
	vertexResource_->data_[17].normal = Vector3(0.0f, 1.0f, 0.0f);
	vertexResource_->data_[18].position = Vector4(1.0f, 1.0f, -1.0f, 1.0f);
	vertexResource_->data_[18].texcoord = Vector2(1.0f, 1.0f);
	vertexResource_->data_[18].normal = Vector3(0.0f, 1.0f, 0.0f);
	vertexResource_->data_[19].position = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	vertexResource_->data_[19].texcoord = Vector2(1.0f, 0.0f);
	vertexResource_->data_[19].normal = Vector3(0.0f, 1.0f, 0.0f);

	vertexResource_->data_[20].position = Vector4(1.0f, -1.0f, -1.0f, 1.0f);
	vertexResource_->data_[20].texcoord = Vector2(0.0f, 1.0f);
	vertexResource_->data_[20].normal = Vector3(0.0f, -1.0f, 0.0f);
	vertexResource_->data_[21].position = Vector4(1.0f, -1.0f, 1.0f, 1.0f);
	vertexResource_->data_[21].texcoord = Vector2(0.0f, 0.0f);
	vertexResource_->data_[21].normal = Vector3(0.0f, -1.0f, 0.0f);
	vertexResource_->data_[22].position = Vector4(-1.0f, -1.0f, -1.0f, 1.0f);
	vertexResource_->data_[22].texcoord = Vector2(1.0f, 1.0f);
	vertexResource_->data_[22].normal = Vector3(0.0f, -1.0f, 0.0f);
	vertexResource_->data_[23].position = Vector4(-1.0f, -1.0f, 1.0f, 1.0f);
	vertexResource_->data_[23].texcoord = Vector2(1.0f, 0.0f);
	vertexResource_->data_[23].normal = Vector3(0.0f, -1.0f, 0.0f);
}

/// @brief コマンドリストに登録する
/// @param commandList 
void Engine::CubeVertexResource::Register(ID3D12GraphicsCommandList* commandList)
{
	indexResource_->Register(commandList);
	vertexResource_->Register(commandList);
}