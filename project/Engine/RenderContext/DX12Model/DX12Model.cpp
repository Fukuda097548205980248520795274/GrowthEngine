#include "DX12Model.h"
#include "ShaderCompiler/ShaderCompiler.h"
#include <cassert>

/// @brief 初期化
/// @param device 
/// @param shaderCompiler 
void Engine::DX12Model::Initialize(ID3D12Device* device, ShaderCompiler* shaderCompiler, DX12Heap* heap,
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
}

/// @brief 更新処理
/// @param viewProjection 
void Engine::DX12Model::Update(ID3D12GraphicsCommandList* commandList)
{
	primitiveStore_->Update(commandList);
	spriteStore_->Update();
}

/// @brief リセット
void Engine::DX12Model::Reset()
{
	psoPrimitive_->ResetBlendMode();
	psoSprite_->ResetBlendMode();
}

/// @brief シャドウアップ用描画処理
/// @param commandList 
void Engine::DX12Model::ShadowMapDraw(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOShadowMap* pso)
{
	// プリミティブストアの描画
	primitiveStore_->ShadowMapDraw(viewProjection, commandList, pso);
}