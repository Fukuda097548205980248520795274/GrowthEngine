#include "Particle3DData.h"

/// @brief 初期化
/// @param device 
/// @param commandList 
/// @param log 
void Engine::Particle3DData::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, DX12Heap* heap, BaseComputePSO* psoInit, Log* log)
{
	// nullptrチェック
	assert(device);
	assert(commandList);
	assert(psoInit);
	assert(heap);

	// パーティクルリソースを生成する
	particleResource_ = std::make_unique<RWSTructuredBufferResource<Particle3DDataForGPU>>();
	particleResource_->Initialize(device, commandList, heap, numInstance_, log);

	// パーティクル数リソースを生成する
	particleNumResource_ = std::make_unique<ConstantBufferResource<ParticleNumDataForGPU>>();
	particleNumResource_->Initialize(device,log);
	particleNumResource_->data_->num = numInstance_;


	/*-----------------------
	    パーティクルの初期化
	-----------------------*/
	
	// PSOの設定
	psoInit->Register(commandList);

	// パーティクルリソースを登録する
	particleResource_->RegisterComputeUAV(commandList, 0);

	// パーティクル数リソースを登録する
	particleNumResource_->RegisterCompute(commandList, 1);

	// ディスパッチする
	commandList->Dispatch((numInstance_ + 255) / 256, 1, 1);
}

/// @brief 更新処理
/// @param commandList 
/// @param psoEmitter 
/// @param psoUpdate 
void Engine::Particle3DData::Update(ID3D12GraphicsCommandList* commandList, BaseComputePSO* psoEmitter, BaseComputePSO* psoUpdate)
{
	// nullptrチェック
	assert(commandList);
	assert(psoEmitter);
	assert(psoUpdate);
}

/// @brief 描画処理
/// @param commandList 
/// @param psoDraw 
void Engine::Particle3DData::Draw(ID3D12GraphicsCommandList* commandList, BaseComputePSO* psoDraw, const Matrix4x4& viewProjection)
{
	// nullptrチェック
	assert(commandList);
	assert(psoDraw);
}