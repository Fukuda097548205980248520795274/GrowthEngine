#include "DX12Model.h"
#include "ShaderCompiler/ShaderCompiler.h"
#include <cassert>
#include "Camera/Camera3D/Camera3D.h"
#include "Store/TextureStore/TextureStore.h"

/// @brief 初期化
/// @param device 
/// @param shaderCompiler 
void Engine::DX12Model::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, ShaderCompiler* shaderCompiler, DX12Heap* heap,
	ModelStore* modelStore, TextureStore* textureStore, AnimationStore* animationStore, SkeletonStore* skeletonStore, LightStore* lightStore, Log* log)
{
	// nullptrチェック
	assert(device);
	assert(shaderCompiler);


	// プリミティブストアの生成と初期化
	primitiveStore_ = std::make_unique<PrimitiveStore>();
	primitiveStore_->Initialize(device, shaderCompiler, heap, modelStore, textureStore, animationStore, skeletonStore, lightStore, log);

	// スプライトストアの生成と初期化
	spriteStore_ = std::make_unique<SpriteStore>();
	spriteStore_->Initialize(device, log);


	// プリミティブ頂点シェーダ
	primitiveVS_ = shaderCompiler->Compile(L"./Assets/Shader/Primitive/Primitive.VS.hlsl", L"vs_6_0");
	assert(primitiveVS_);

	// プリミティブピクセルシェーダ
	primitivePS_ = shaderCompiler->Compile(L"./Assets/Shader/Primitive/Primitive.PS.hlsl", L"ps_6_0");
	assert(primitivePS_);


	// スプライト頂点シェーダ
	spriteVS_ = shaderCompiler->Compile(L"./Assets/Shader/Sprite/Sprite.VS.hlsl", L"vs_6_0");
	assert(spriteVS_);

	// スプライトピクセルシェーダ
	spritePS_ = shaderCompiler->Compile(L"./Assets/Shader/Sprite/Sprite.PS.hlsl", L"ps_6_0");
	assert(spritePS_);



	// プリミティブPSOの生成と初期化
	psoPrimitive_ = std::make_unique<PSOPrimitive>();
	psoPrimitive_->Initialize(device, primitiveVS_.Get(), primitivePS_.Get(), log);

	// スプライトPSOの生成と初期化
	psoSprite_ = std::make_unique<PSOSprite>();
	psoSprite_->Initialize(device, spriteVS_.Get(), spritePS_.Get(), log);




	// パーティクルリソースの生成と初期化
	particleResource_ = std::make_unique<RWStructuredBufferResource<ParticleCS>>();
	particleResource_->Initialize(device, commandList, heap, 1024, log);

	// エミッターリソース
	emitterResource_ = std::make_unique<ConstantBufferResource<EmitterSphere>>();
	emitterResource_->Initialize(device, log);
	emitterResource_->data_->translate = Vector3(0.0f, 0.0f, 0.0f);
	emitterResource_->data_->radius = 1.0f;
	emitterResource_->data_->count = 10;
	emitterResource_->data_->frequency = 0.5f;
	emitterResource_->data_->frequencyTime = 0.0f;
	emitterResource_->data_->emit = 0;

	// 初期化用パーティクルPSOの生成と初期化
	psoParticleInitialize_ = std::make_unique<ComputePSOParticleInitialize>();
	psoParticleInitialize_->Initialize(device, shaderCompiler, log);

	// 放出用パーティクルPSOの生成と初期化
	psoEmitParticle_ = std::make_unique<ComputePSOEmitParticle>();
	psoEmitParticle_->Initialize(device, shaderCompiler, log);


	// パーティクル頂点シェーダ
	particleVS_ = shaderCompiler->Compile(L"./Assets/Shader/Particle/Particle.VS.hlsl", L"vs_6_0");
	assert(particleVS_);

	// パーティクルピクセルシェーダ
	particlePS_ = shaderCompiler->Compile(L"./Assets/Shader/Particle/Particle.PS.hlsl", L"ps_6_0");
	assert(particlePS_);

	// パーティクルPSOの生成と初期化
	psoParticle_ = std::make_unique<PSOParticle>();
	psoParticle_->Initialize(device, particleVS_.Get(), particlePS_.Get(), log);


	// ビューリソースの生成と初期化
	viewResource_ = std::make_unique<ConstantBufferResource<PreViewDataForGPU>>();
	viewResource_->Initialize(device, log);


	// 頂点リソースの生成と初期化
	particleVertexResource_ = std::make_unique<VertexBufferResource<SpriteVertexData>>();
	particleVertexResource_->Initialize(device, 4, log);

	particleVertexResource_->data_[0].position = Vector4(-1.0f, -1.0f, 0.0f, 1.0f);
	particleVertexResource_->data_[0].texcoord = Vector2(0.0f, 1.0f);

	particleVertexResource_->data_[1].position = Vector4(-1.0f, 1.0f, 0.0f, 1.0f);
	particleVertexResource_->data_[1].texcoord = Vector2(0.0f, 0.0f);

	particleVertexResource_->data_[2].position = Vector4(1.0f, -1.0f, 0.0f, 1.0f);
	particleVertexResource_->data_[2].texcoord = Vector2(1.0f, 1.0f);

	particleVertexResource_->data_[3].position = Vector4(1.0f, 1.0f, 0.0f, 1.0f);
	particleVertexResource_->data_[3].texcoord = Vector2(1.0f, 0.0f);


	// インデックスリソースの初期化
	particleIndexResource_ = std::make_unique<IndexBufferResource>();
	particleIndexResource_->Initialize(device, 6, log);

	particleIndexResource_->data_[0] = 0;
	particleIndexResource_->data_[1] = 1;
	particleIndexResource_->data_[2] = 2;
	particleIndexResource_->data_[3] = 1;
	particleIndexResource_->data_[4] = 3;
	particleIndexResource_->data_[5] = 2;
}

/// @brief 更新処理
/// @param viewProjection 
void Engine::DX12Model::Update(ID3D12GraphicsCommandList* commandList)
{
	psoParticle_->ResetBlendMode();

	ParticleInitialize(commandList);

	primitiveStore_->Update(commandList);
	spriteStore_->Update();


	// タイマーを進める
	emitterResource_->data_->frequencyTime += 1.0f / 60.0f;
	
	// 射出間隔を上回ったら、射出許可を出して時間を調整
	if (emitterResource_->data_->frequencyTime >= emitterResource_->data_->frequency)
	{
		emitterResource_->data_->frequencyTime -= emitterResource_->data_->frequency;
		emitterResource_->data_->emit = 1;
	}
	else
	{
		// まだ射出しない
		emitterResource_->data_->emit = 0;
	}


	psoEmitParticle_->Register(commandList);

	particleResource_->RegisterCompute(commandList, 0);

	emitterResource_->RegisterCompute(commandList, 1);

	commandList->Dispatch(1, 1, 1);
}

/// @brief シャドウアップ用描画処理
/// @param commandList 
void Engine::DX12Model::ShadowMapDraw(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOShadowMap* pso)
{
	// プリミティブストアの描画
	primitiveStore_->ShadowMapDraw(viewProjection, commandList, pso);
}

/// @brief パーティクル初期化
void Engine::DX12Model::ParticleInitialize(ID3D12GraphicsCommandList* commandList)
{
	if (isParticleInitialize)return;

	psoParticleInitialize_->Register(commandList);
	particleResource_->RegisterCompute(commandList, 0);
	commandList->Dispatch(1, 1, 1);

	isParticleInitialize = true;
}

/// @brief パーティクルを描画する
/// @param commandList 
/// @param camera3d 
void Engine::DX12Model::DrawParticle(ID3D12GraphicsCommandList* commandList, const Camera3D& camera3d, TextureStore* textureStore)
{
	particleResource_->Barrier(commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ);

	psoParticle_->Register(commandList);

	particleIndexResource_->Register(commandList);
	particleVertexResource_->Register(commandList);

	particleResource_->RegisterGraphics(commandList, 0);

	viewResource_->data_->viewProjection = camera3d.GetViewProjectionMatrix();
	viewResource_->data_->billboardMatrix = Make3DRotateMatrix4x4(camera3d.GetQuaternion());
	viewResource_->RegisterGraphics(commandList, 1);

	commandList->SetGraphicsRootDescriptorTable(2, textureStore->GetSrvGpuHandle(0));

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	commandList->DrawIndexedInstanced(6, 1024, 0, 0, 0);

	particleResource_->Barrier(commandList, D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
}