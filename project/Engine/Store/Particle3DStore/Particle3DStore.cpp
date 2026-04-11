#include "Particle3DStore.h"
#include "RenderContext/DX12Heap/DX12Heap.h"
#include "ShaderCompiler/ShaderCompiler.h"
#include <cassert>

/// @brief 初期化
/// @param device 
/// @param compiler 
/// @param heap 
/// @param modelStore 
/// @param textureStore 
void Engine::Particle3DStore::Initialize(ID3D12Device* device, ShaderCompiler* compiler, DX12Heap* heap, ModelStore* modelStore, TextureStore* textureStore, Log* log)
{
	// nullptrチェック
	assert(device);
	assert(compiler);
	assert(heap);
	assert(modelStore);
	assert(textureStore);

	// 引数を受け取る
	heap_ = heap;
	modelStore_ = modelStore;
	textureStore_ = textureStore;

	// シェーダーをコンパイルする
	vertexShaderBlob_ = compiler->Compile(L"./Assets/Shader/Particle3D/Draw/Particle3D.VS.hlsl", L"vs_6_0");
	pixelShaderBlob_ = compiler->Compile(L"./Assets/Shader/Particle3D/Draw/Particle3D.PS.hlsl", L"ps_6_0");

	// PSOを生成する
	psoParticle_ = std::make_unique<PSOParticle>();
	psoParticle_->Initialize(device, vertexShaderBlob_.Get(), pixelShaderBlob_.Get(), log);
}