#include "Particle3DData.h"
#include "PSO/ComputePSO/BaseComputePSO.h"
#include "PSO/PSOModel/BasePSOModel.h"
#include "Store/ModelStore/ModelStore.h"
#include "Store/TextureStore/TextureStore.h"
#include "GrowthEngine.h"

/// @brief 初期化
/// @param device 
/// @param commandList 
/// @param log 
void Engine::Particle3DData::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, DX12Heap* heap,
	ModelStore* modelStore, TextureStore* textureStore, BasePSOModel* psoDraw, BaseComputePSO* psoInit, Log* log)
{
	// nullptrチェック
	assert(device);
	assert(commandList);
	assert(psoDraw);
	assert(psoInit);
	assert(modelStore);
	assert(textureStore);
	assert(heap);

	// 引数を受け取る
	psoDraw_ = psoDraw;
	modelStore_ = modelStore;
	textureStore_ = textureStore;

	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

	// パーティクルリソースを生成する
	particleResource_ = std::make_unique<RWSTructuredBufferResource<Particle3DDataForGPU>>();
	particleResource_->Initialize(device, commandList, heap, numInstance_, log);

	// パーティクル数リソースを生成する
	particleNumResource_ = std::make_unique<ConstantBufferResource<ParticleNumDataForGPU>>();
	particleNumResource_->Initialize(device,log);
	particleNumResource_->data_->num = numInstance_;

	// パーティクルビューリソースを生成する
	particleViewResource_ = std::make_unique<ConstantBufferResource<ParticlePreViewDataForGPU>>();
	particleViewResource_->Initialize(device, log);

	// エミッターリソースを生成する
	particleEmitterPointResource_ = std::make_unique<ConstantBufferResource<Particle3DEmitterPointDataForGPU>>();
	particleEmitterPointResource_->Initialize(device, log);

	// パーティクルフレームリソースを生成する
	particlePerFrameResource_ = std::make_unique<ConstantBufferResource<ParticlePerFrameDataForGPU>>();
	particlePerFrameResource_->Initialize(device, log);
	particlePerFrameResource_->data_->deltaTime = 0.0f;
	particlePerFrameResource_->data_->time = 0.0f;

	// フリーリストインデックスリソースを生成する
	freeListIndexResource_ = std::make_unique<RWSTructuredBufferResource<int32_t>>();
	freeListIndexResource_->Initialize(device, commandList, heap, 1, log);

	// フリーリストリソースを生成する
	freeListResource_ = std::make_unique<RWSTructuredBufferResource<uint32_t>>();
	freeListResource_->Initialize(device, commandList, heap, numInstance_, log);


	particleEmitterPointResource_->data_->translate = Vector3(0.0f, 0.0f, 0.0f);
	particleEmitterPointResource_->data_->count = 100;
	particleEmitterPointResource_->data_->frequency = 1.0f;
	particleEmitterPointResource_->data_->frequencyTimer = 0.0f;
	particleEmitterPointResource_->data_->emit = 0;
	particleEmitterPointResource_->data_->startColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	particleEmitterPointResource_->data_->endColor = Vector4(1.0f, 1.0f, 1.0f, 0.0f);
	particleEmitterPointResource_->data_->startScale = 1.0f;
	particleEmitterPointResource_->data_->endScale = 1.0f;
	particleEmitterPointResource_->data_->minLifeTime = 1.0f;
	particleEmitterPointResource_->data_->maxLifeTime = 1.0f;
	particleEmitterPointResource_->data_->startSpeed = 8.0f;
	particleEmitterPointResource_->data_->endSpeed = 0.0f;


	// テクスチャを取得する
	hTexture_ = modelStore_->GetModelData(hModel_).meshes[0].material.handle;


	/*-----------------------
	    パーティクルの初期化
	-----------------------*/
	
	// PSOの設定
	psoInit->Register(commandList);

	// パーティクルリソースを登録する
	particleResource_->RegisterComputeUAV(commandList, 0);

	// パーティクル数リソースを登録する
	particleNumResource_->RegisterCompute(commandList, 1);

	// フリーリストインデックスリソースを登録する
	freeListIndexResource_->RegisterComputeUAV(commandList, 2);

	// フリーリストリソースを登録する
	freeListResource_->RegisterComputeUAV(commandList, 3);

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


	// タイマーを進める
	particleEmitterPointResource_->data_->frequencyTimer += engine_->GetDeltaTime();
	particlePerFrameResource_->data_->time += engine_->GetDeltaTime();

	// デルタタイムを取得する
	particlePerFrameResource_->data_->deltaTime = engine_->GetDeltaTime();

	// タイマーが時間を超えたら放出する
	if(particleEmitterPointResource_->data_->frequencyTimer >= particleEmitterPointResource_->data_->frequency)
	{
		particleEmitterPointResource_->data_->emit = 1;
		particleEmitterPointResource_->data_->frequencyTimer = 0.0f;
	}
	else
	{
		particleEmitterPointResource_->data_->emit = 0;
	}


	/*------------
	   エミッター
	------------*/

	// PSOの設定
	psoEmitter->Register(commandList);

	// パーティクルリソースを登録する
	particleResource_->RegisterComputeUAV(commandList, 0);

	// エミッターリソースを登録する
	particleEmitterPointResource_->RegisterCompute(commandList, 1);

	// パーティクル数リソースを登録する
	particleNumResource_->RegisterCompute(commandList, 2);

	// パーティクルフレームリソースを登録する
	particlePerFrameResource_->RegisterCompute(commandList, 3);

	// フリーリストインデックスリソースを登録する
	freeListIndexResource_->RegisterComputeUAV(commandList, 4);

	// フリーリストリソースを登録する
	freeListResource_->RegisterComputeUAV(commandList, 5);

	// ディスパッチする
	commandList->Dispatch(1, 1, 1);


	// UAVバリアを張る
	UAVBarrier(particleResource_->GetResource(), commandList);
	UAVBarrier(freeListIndexResource_->GetResource(), commandList);
	UAVBarrier(freeListResource_->GetResource(), commandList);


	/*-------------
	    更新処理
	-------------*/

	// PSOの設定
	psoUpdate->Register(commandList);

	// パーティクルリソースを登録する
	particleResource_->RegisterComputeUAV(commandList, 0);

	// パーティクル数リソースを登録する
	particleNumResource_->RegisterCompute(commandList, 1);

	// パーティクルフレームリソースを登録する
	particlePerFrameResource_->RegisterCompute(commandList, 2);

	// フリーリストインデックスリソースを登録する
	freeListIndexResource_->RegisterComputeUAV(commandList, 3);

	// フリーリストリソースを登録する
	freeListResource_->RegisterComputeUAV(commandList, 4);

	// エミッターリソースを登録する
	particleEmitterPointResource_->RegisterCompute(commandList, 5);

	// ディスパッチする
	commandList->Dispatch((numInstance_ + 255) / 256, 1, 1);
}

/// @brief 描画処理
/// @param commandList 
/// @param psoDraw 
void Engine::Particle3DData::Draw(ID3D12GraphicsCommandList* commandList,const Matrix4x4& viewProjection)
{
	// nullptrチェック
	assert(commandList);

	// バリアを張る
	particleResource_->Barrier(commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);



	// データを渡す
	particleViewResource_->data_->viewProjection = viewProjection;
	particleViewResource_->data_->billboard = MakeIdentityMatrix4x4();


	/*------------------------
	    コマンドリストに登録する
	------------------------*/

	// PSOの設定
	psoDraw_->Register(commandList);

	// 頂点を登録する
	modelStore_->Register(commandList, hModel_, 0);

	// パーティクルリソースを登録する
	particleResource_->RegisterGraphicsSRV(commandList, 0);

	// パーティクルビューリソースを登録する
	particleViewResource_->RegisterGraphics(commandList, 1);

	// テクスチャを登録する
	commandList->SetGraphicsRootDescriptorTable(2, textureStore_->GetSrvGpuHandle(hTexture_));

	// ドローコール
	commandList->DrawIndexedInstanced(static_cast<UINT>(modelStore_->GetModelData(hModel_).meshes[0].indices.size()), numInstance_, 0, 0, 0);



	// バリアを張る
	particleResource_->Barrier(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
}