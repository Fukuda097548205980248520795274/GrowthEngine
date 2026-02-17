#include "DX12Prefab.h"
#include <cassert>
#include "ShaderCompiler/ShaderCompiler.h"

/// @brief 初期化
/// @param device 
/// @param shaderCompiler 
/// @param log 
void Engine::DX12Prefab::Initialize(ID3D12Device* device, ShaderCompiler* shaderCompiler, Log* log)
{
	// nullptrチェック
	assert(device);
	assert(shaderCompiler);


	// プレハブ用スプライトストアの生成
	prefabSpriteStore_ = std::make_unique<PrefabSpriteStore>();
	prefabSpriteStore_->Initialize(device, log);


	// スプライト用プレハブ頂点シェーダ
	spritePrefabVS_ = shaderCompiler->Compile(L"./Assets/Shader/PrefabSprite/PrefabSprite.VS.hlsl", L"vs_6_0");
	assert(spritePrefabVS_);

	// スプライト用プレハブピクセルシェーダ
	spritePrefabPS_ = shaderCompiler->Compile(L"./Assets/Shader/PrefabSprite/PrefabSprite.PS.hlsl", L"ps_6_0");
	assert(spritePrefabPS_);


	// スプライト用プレハブPSO
	psoPrefabSprite_ = std::make_unique<PSOPrefabSprite>();
	psoPrefabSprite_->Initialize(device, spritePrefabVS_.Get(), spritePrefabPS_.Get(), log);
}

/// @brief 更新処理
void Engine::DX12Prefab::Update()
{

}

/// @brief リセット
void Engine::DX12Prefab::Reset()
{
	prefabSpriteStore_->Reset();
}