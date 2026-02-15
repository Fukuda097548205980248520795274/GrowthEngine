#include "DX12Primitive.h"
#include "ShaderCompiler/ShaderCompiler.h"
#include <cassert>

/// @brief 初期化
/// @param device 
/// @param shaderCompiler 
void Engine::DX12Primitive::Initialize(ID3D12Device* device, ShaderCompiler* shaderCompiler, Log* log)
{
	// nullptrチェック
	assert(device);
	assert(shaderCompiler);


	// プリミティブストアの生成
	primitiveStore_ = std::make_unique<PrimitiveStore>();


	// モデル用プリミティブ頂点シェーダ
	primitiveModelVS_ = shaderCompiler->Compile(L"./Assets/Shader/PrimitiveModel/PrimitiveModel.VS.hlsl", L"vs_6_0");
	assert(primitiveModelVS_);

	// モデル用プリミティブピクセルシェーダ
	primitiveModelPS_ = shaderCompiler->Compile(L"./Assets/Shader/PrimitiveModel/PrimitiveModel.PS.hlsl", L"ps_6_0");
	assert(primitiveModelPS_);



	// モデル用プリミティブPSOの生成と初期化
	psoPrimitiveModel_ = std::make_unique<PSOPrimitiveModel>();
	psoPrimitiveModel_->Initialize(device, primitiveModelVS_.Get(), primitiveModelPS_.Get(), log);
}

/// @brief 更新処理
/// @param viewProjection 
void Engine::DX12Primitive::Update()
{
	primitiveStore_->Update();
}

/// @brief シャドウアップ用描画処理
/// @param commandList 
void Engine::DX12Primitive::ShadowMapDraw(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOShadowMap* pso)
{
	// プリミティブストアの描画
	primitiveStore_->ShadowMapDraw(viewProjection, commandList, pso);
}