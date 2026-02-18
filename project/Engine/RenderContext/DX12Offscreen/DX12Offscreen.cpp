#include "DX12Offscreen.h"
#include "Log/Log.h"
#include "RenderContext/DX12Heap/DX12Heap.h"
#include "RenderContext/DX12Buffering/DX12Buffering.h"
#include "ShaderCompiler/ShaderCompiler.h"
#include "Func/ResourceFunc/ResourceFunc.h"
#include <cassert>

/// @brief 初期化
/// @param device 
/// @param heap 
/// @param buffering 
/// @param compiler 
/// @param log 
void Engine::DX12Offscreen::Initialize(ID3D12Device* device, DX12Heap* heap, DX12Buffering* buffering, ShaderCompiler* compiler, Log* log)
{
	// nullptrチェック
	assert(device);
	assert(heap);

	// オフスクリーンのリソースを生成する
	for (int32_t i = 0; i < kMaxOffscreenCount; ++i)
	{
		offscreenResource_[i] = std::make_unique<OffscreenResource>();
		offscreenResource_[i]->Initialize(device, buffering, heap, log);
	}

	// 深度リソースを生成する
	depthResource_ = std::make_unique<DepthResource>();
	depthResource_->Initialize(device, buffering, heap, log);


	// 頂点シェーダを読み込む
	vertexShaderBlob_ = compiler->Compile(L"./Assets/Shader/Fullscreen/Fullscreen.VS.hlsl", L"vs_6_0");

	// PSO CopyImage の生成と初期化
	psoCopyImage_ = std::make_unique<PSOCopyImage>();
	psoCopyImage_->Initialize(device, compiler, vertexShaderBlob_.Get(), log);


	// ポストエフェクトストアの生成
	postEffectStore_ = std::make_unique<PostEffectStore>();
	postEffectStore_->Initialize(device, compiler, vertexShaderBlob_.Get(), log);
}

/// @brief クリア
/// @param commandList 
void Engine::DX12Offscreen::Clear(ID3D12GraphicsCommandList* commandList)
{
	// nullptrチェック
	assert(commandList);

	// 値を初期化
	currentOffscreen_ = 0;

	// オフスクリーンのレンダーターゲット・デプスステンシルの設定とクリア
	ClearRenderTarget(commandList);
	ClearDepthStencil(commandList);
}


/// @brief スワップチェインのRTVリソースにオフクリーンリソースを書き込む
/// @param commandList 
void Engine::DX12Offscreen::RenderSwapChain(ID3D12GraphicsCommandList* commandList)
{
	// nullptrチェック
	assert(commandList);

	// 書き込み対象 -> 読み込ませテクスチャ
	TransitionBarrier(offscreenResource_[currentOffscreen_]->GetResource(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, commandList);

	// PSOの設定
	psoCopyImage_->Register(commandList);

	// テクスチャ
	offscreenResource_[currentOffscreen_]->Register(commandList, 0);

	// 形状は三角形
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 頂点は3つ
	commandList->DrawInstanced(3, 1, 0, 0);

	// 読み込ませテクスチャ -> 書き込み対象
	TransitionBarrier(offscreenResource_[currentOffscreen_]->GetResource(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET, commandList);
}


/// @brief レンダーターゲットのクリア
/// @param commandList 
void Engine::DX12Offscreen::ClearRenderTarget(ID3D12GraphicsCommandList* commandList)
{
	// レンダーターゲットのクリアと設定
	offscreenResource_[currentOffscreen_]->ClearRenderTarget(commandList, depthResource_->GetDsvCpuHandle());
}

/// @brief デプスステンシルのクリア
/// @param commandList 
void Engine::DX12Offscreen::ClearDepthStencil(ID3D12GraphicsCommandList* commandList)
{
	// てぷスステンシルのクリア
	depthResource_->ClearDepthStencil(commandList);
}



/// @brief ポストエフェクトを描画する
/// @param hPostEffect 
/// @param commandList 
void Engine::DX12Offscreen::DrawPostEffect(PostEffectHandle hPostEffect, ID3D12GraphicsCommandList* commandList)
{
	// nullptrチェック
	assert(commandList);

	// カウントする
	++currentOffscreen_;
	currentOffscreen_ = currentOffscreen_ % 2;

	// オフスクリーンのレンダーターゲット・デプスステンシルの設定とクリア
	ClearRenderTarget(commandList);
	

	// 書き込み対象 -> 読み込ませテクスチャ
	TransitionBarrier(offscreenResource_[1 - currentOffscreen_]->GetResource(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, commandList);

	postEffectStore_->DrawPostEffect(hPostEffect, commandList, offscreenResource_[1 - currentOffscreen_].get());

	// 読み込ませテクスチャ -> 書き込み対象
	TransitionBarrier(offscreenResource_[1 - currentOffscreen_]->GetResource(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET, commandList);
}