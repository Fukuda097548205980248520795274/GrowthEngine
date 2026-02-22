#include "DX12Prefab.h"
#include <cassert>
#include "ShaderCompiler/ShaderCompiler.h"

/// @brief 初期化
/// @param device 
/// @param compiler 
/// @param heap 
/// @param modelStore 
/// @param textureStore 
/// @param animationStore 
/// @param skeletonStore 
/// @param lightStore 
/// @param cameraStore 
/// @param log 
void Engine::DX12Prefab::Initialize(ID3D12Device* device, ShaderCompiler* compiler, DX12Heap* heap,
	ModelStore* modelStore, TextureStore* textureStore, AnimationStore* animationStore, SkeletonStore* skeletonStore,
	LightStore* lightStore, Camera3DStore* cameraStore, Log* log)
{
	// nullptrチェック
	assert(device);
	assert(compiler);
	assert(heap);
	assert(modelStore);
	assert(textureStore);
	assert(animationStore);
	assert(skeletonStore);
	assert(lightStore);
	assert(cameraStore);


	// プリミティブ用プレハブストアの生成
	prefabPrimitiveStore_ = std::make_unique<PrefabPrimitiveStore>();
	prefabPrimitiveStore_->Initialize(device, compiler, heap, modelStore, textureStore, animationStore, skeletonStore, lightStore, cameraStore, log);

	// スプライト用プレハブストアの生成
	prefabSpriteStore_ = std::make_unique<PrefabSpriteStore>();
	prefabSpriteStore_->Initialize(device, log);


	// プリミティブ用プレハブ頂点シェーダ
	primitivePrefabVS_ = compiler->Compile(L"./Assets/Shader/PrefabPrimitive/PrefabPrimitive.VS.hlsl", L"vs_6_0");
	assert(primitivePrefabVS_);

	// プリミティブ用プレハブピクセルシェーダ
	primitivePrefabPS_ = compiler->Compile(L"./Assets/Shader/PrefabPrimitive/PrefabPrimitive.PS.hlsl", L"ps_6_0");
	assert(primitivePrefabPS_);


	// スプライト用プレハブ頂点シェーダ
	spritePrefabVS_ = compiler->Compile(L"./Assets/Shader/PrefabSprite/PrefabSprite.VS.hlsl", L"vs_6_0");
	assert(spritePrefabVS_);

	// スプライト用プレハブピクセルシェーダ
	spritePrefabPS_ = compiler->Compile(L"./Assets/Shader/PrefabSprite/PrefabSprite.PS.hlsl", L"ps_6_0");
	assert(spritePrefabPS_);


	// プリミティブ用プレハブPSO
	psoPrefabPrimitive_ = std::make_unique<PSOPrefabPrimitive>();
	psoPrefabPrimitive_->Initialize(device, primitivePrefabVS_.Get(), primitivePrefabPS_.Get(), log);

	// スプライト用プレハブPSO
	psoPrefabSprite_ = std::make_unique<PSOPrefabSprite>();
	psoPrefabSprite_->Initialize(device, spritePrefabVS_.Get(), spritePrefabPS_.Get(), log);
}

/// @brief 更新処理
void Engine::DX12Prefab::Update()
{
	// 更新
	prefabPrimitiveStore_->Update();
	prefabSpriteStore_->Update();
}

/// @brief リセット
void Engine::DX12Prefab::Reset()
{
	psoPrefabPrimitive_->ResetBlendMode();
	psoPrefabSprite_->ResetBlendMode();
}

/// @brief シャドウマップの描画処理
/// @param viewProjection 
/// @param commandList 
/// @param pso 
void Engine::DX12Prefab::ShadowMapDraw(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOShadowMap* pso)
{
	prefabPrimitiveStore_->ShadowMapDraw(viewProjection, commandList, pso);
}

/// @brief 全てのインスタンスを削除する
void Engine::DX12Prefab::DestroyAllInstance()
{
	prefabPrimitiveStore_->DestroyAllInstance();
	prefabSpriteStore_->DestroyAllInstance();
}

/// @brief リセット
void Engine::DX12Prefab::PrefabReset()
{
	prefabPrimitiveStore_->Reset();
	prefabSpriteStore_->Reset();
}