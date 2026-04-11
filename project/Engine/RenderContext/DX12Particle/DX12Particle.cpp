#include "DX12Particle.h"
#include <cassert>

/// @brief 初期化
/// @param device 
/// @param compiler 
/// @param heap 
/// @param modelStore 
/// @param textureStore
/// @param log 
void Engine::DX12Particle::Initialize(ID3D12Device* device, ShaderCompiler* compiler, DX12Heap* heap, ModelStore* modelStore, TextureStore* textureStore, Log* log)
{
	// nullptrチェック
	assert(device);
	assert(compiler);
	assert(heap);
	assert(modelStore);
	assert(textureStore);

	// 3Dパーティクルストアを生成する
	particle3DStore_ = std::make_unique<Particle3DStore>();
	particle3DStore_->Initialize(device, compiler, heap, modelStore, textureStore, log);
}

/// @brief 更新処理
/// @param commandList 
void Engine::DX12Particle::Update(ID3D12GraphicsCommandList* commandList)
{
	particle3DStore_->Update(commandList);
}

/// @brief 描画処理
/// @param commandList 
/// @param viewProjection 
void Engine::DX12Particle::Draw(ID3D12GraphicsCommandList* commandList, const Matrix4x4& viewProjection)
{
	particle3DStore_->Draw(commandList, viewProjection);
}