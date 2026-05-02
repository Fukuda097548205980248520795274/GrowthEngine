#include "DX12Offscreen.h"
#include "Log/Log.h"
#include "RenderContext/DX12Heap/DX12Heap.h"
#include "RenderContext/DX12Buffering/DX12Buffering.h"
#include "ShaderCompiler/ShaderCompiler.h"
#include "Func/ResourceFunc/ResourceFunc.h"
#include <cassert>

#include "RenderContext/ImGuiRender/ImGuiRender.h"

/// @brief 初期化
/// @param device 
/// @param heap 
/// @param buffering 
/// @param compiler 
/// @param log 
void Engine::DX12Offscreen::Initialize(ID3D12Device* device, DX12Heap* heap, DX12Buffering* buffering, ShaderCompiler* compiler, TextureStore* textureStore, Log* log)
{
	// nullptrチェック
	assert(device);
	assert(heap);
	assert(buffering);
	assert(compiler);
	assert(textureStore);

	// 引数を受け取る
	heap_ = heap;
	buffering_ = buffering;

	int width = static_cast<int32_t>(buffering->GetSwapChainDesc().Width);
	int height = static_cast<int32_t>(buffering->GetSwapChainDesc().Height);

	// オフスクリーンのリソースを生成する
	for (int32_t i = 0; i < kMaxOffscreenCount; ++i)
	{
		offscreenResource_[i] = std::make_unique<OffscreenResource>();
		offscreenResource_[i]->Initialize(device, heap,
			static_cast<int32_t>(buffering->GetSwapChainDesc().Width), static_cast<int32_t>(buffering->GetSwapChainDesc().Height), log);
	}

	// 深度リソースを生成する
	depthResource_ = std::make_unique<DepthResource>();
	depthResource_->Initialize(device, width, height, heap, log);


	// 頂点シェーダを読み込む
	vertexShaderBlob_ = compiler->Compile(L"./Assets/Shader/Fullscreen/Fullscreen.VS.hlsl", L"vs_6_0");

	// PSO CopyImage の生成と初期化
	psoCopyImage_ = std::make_unique<PSOCopyImage>();
	psoCopyImage_->Initialize(device, compiler, vertexShaderBlob_.Get(), log);


	// ポストエフェクトストアの生成
	postEffectStore_ = std::make_unique<PostEffectStore>();
	postEffectStore_->Initialize(device, compiler, vertexShaderBlob_.Get(),
		heap, textureStore, static_cast<int32_t>(buffering_->GetSwapChainDesc().Width), static_cast<int32_t>(buffering_->GetSwapChainDesc().Height), log);
}

/// @brief サイズを作り直す
/// @param device 
/// @param buffering 
void Engine::DX12Offscreen::Resize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, DX12Buffering* buffering)
{
	int width = static_cast<int32_t>(buffering->GetSwapChainDesc().Width);
	int height = static_cast<int32_t>(buffering->GetSwapChainDesc().Height);

	offscreenResource_[0]->Resize(device, width, height);
	offscreenResource_[1]->Resize(device, width, height);
	depthResource_->Resize(device, width, height);

	// ポストエフェクトストアのリサイズ
	postEffectStore_->Resize(device, commandList, width, height);
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
	offscreenResource_[currentOffscreen_]->RegisterGraphics(commandList, 0);

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
	// デプスステンシルのクリア
	depthResource_->ClearDepthStencil(commandList);
}



/// @brief ポストエフェクトを描画する
/// @param hPostEffect 
/// @param commandList 
void Engine::DX12Offscreen::DrawPostEffect(PostEffectHandle hPostEffect, ID3D12GraphicsCommandList* commandList, const PostEffectRenderContext& context)
{
	// nullptrチェック
	assert(commandList);

	// このポストエフェクトが深度を必要とするかどうか
	const bool isUseDepth = postEffectStore_->IsRequiredInput(hPostEffect, PostEffectInput::DepthTexture);
	const bool isBloom = postEffectStore_->IsBloom(hPostEffect);
	const int32_t sourceOffscreenIndex = currentOffscreen_;

	// ブルームは複数回描画する必要があるため、描画コマンドの登録の仕方を変える
	if (isBloom)
	{
		// 登録用コンテキストを作る
		PostEffectRenderContext registerContext{};
		registerContext = context;
		registerContext.commandList = commandList;
		registerContext.offscreenPixelShaderResource = offscreenResource_[sourceOffscreenIndex].get();
		registerContext.offscreenRenderTargetResource = offscreenResource_[currentOffscreen_].get();
		registerContext.depthResource = isUseDepth ? depthResource_.get() : nullptr;

		// ポストエフェクトの描画コマンドを登録する
		postEffectStore_->DrawPostEffect(hPostEffect, registerContext);
	}
	else
	{
		// カウントする
		++currentOffscreen_;
		currentOffscreen_ = currentOffscreen_ % 2;

		// オフスクリーンのレンダーターゲット・デプスステンシルの設定とクリア
		ClearRenderTarget(commandList);

		// 書き込み対象 -> 読み込ませテクスチャ
		TransitionBarrier(offscreenResource_[sourceOffscreenIndex]->GetResource(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, commandList);

		if (isUseDepth)
		{
			// 深度書き込み -> 読み込みテクスチャ
			TransitionBarrier(depthResource_->GetResource(),
				D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, commandList);
		}

		// 登録用コンテキストを作る
		PostEffectRenderContext registerContext{};
		registerContext = context;
		registerContext.commandList = commandList;
		registerContext.offscreenPixelShaderResource = offscreenResource_[sourceOffscreenIndex].get();
		registerContext.offscreenRenderTargetResource = offscreenResource_[currentOffscreen_].get();
		registerContext.depthResource = isUseDepth ? depthResource_.get() : nullptr;

		// ポストエフェクトの描画コマンドを登録する
		postEffectStore_->DrawPostEffect(hPostEffect, registerContext);

		if (isUseDepth)
		{
			// 読み込みテクスチャ -> 深度書き込み
			TransitionBarrier(depthResource_->GetResource(),
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE, commandList);
		}

		// 読み込ませテクスチャ -> 書き込み対象
		TransitionBarrier(offscreenResource_[sourceOffscreenIndex]->GetResource(),
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET, commandList);
	}
}

/// @brief ポストエフェクトを描画する
/// @param name 
/// @param commandList 
void Engine::DX12Offscreen::DrawPostEffect(const std::string& name, ID3D12GraphicsCommandList* commandList, const PostEffectRenderContext& context)
{
	// nullptrチェック
	assert(commandList);

	// このポストエフェクトが深度を必要とするかどうか
	const bool isUseDepth = postEffectStore_->IsRequiredInput(name, PostEffectInput::DepthTexture);
	const bool isBloom = postEffectStore_->IsBloom(name);
	const int32_t sourceOffscreenIndex = currentOffscreen_;

	// ブルームは複数回描画する必要があるため、描画コマンドの登録の仕方を変える
	if (isBloom)
	{
		// 登録用コンテキストを作る
		PostEffectRenderContext registerContext{};
		registerContext = context;
		registerContext.commandList = commandList;
		registerContext.offscreenPixelShaderResource = offscreenResource_[sourceOffscreenIndex].get();
		registerContext.offscreenRenderTargetResource = offscreenResource_[currentOffscreen_].get();
		registerContext.depthResource = isUseDepth ? depthResource_.get() : nullptr;

		// ポストエフェクトの描画コマンドを登録する
		postEffectStore_->DrawPostEffect(name, registerContext);
	}
	else
	{
		// カウントする
		++currentOffscreen_;
		currentOffscreen_ = currentOffscreen_ % 2;

		// オフスクリーンのレンダーターゲット・デプスステンシルの設定とクリア
		ClearRenderTarget(commandList);

		// 書き込み対象 -> 読み込ませテクスチャ
		TransitionBarrier(offscreenResource_[sourceOffscreenIndex]->GetResource(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, commandList);

		if (isUseDepth)
		{
			// 深度書き込み -> 読み込みテクスチャ
			TransitionBarrier(depthResource_->GetResource(),
				D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, commandList);
		}

		// 登録用コンテキストを作る
		PostEffectRenderContext registerContext{};
		registerContext = context;
		registerContext.commandList = commandList;
		registerContext.offscreenPixelShaderResource = offscreenResource_[sourceOffscreenIndex].get();
		registerContext.offscreenRenderTargetResource = offscreenResource_[currentOffscreen_].get();
		registerContext.depthResource = isUseDepth ? depthResource_.get() : nullptr;

		// ポストエフェクトの描画コマンドを登録する
		postEffectStore_->DrawPostEffect(name, registerContext);

		if (isUseDepth)
		{
			// 読み込みテクスチャ -> 深度書き込み
			TransitionBarrier(depthResource_->GetResource(),
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE, commandList);
		}

		// 読み込ませテクスチャ -> 書き込み対象
		TransitionBarrier(offscreenResource_[sourceOffscreenIndex]->GetResource(),
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET, commandList);
	}
}

/// @brief モーションベクトルを描画する
/// @param commandList 
/// @param render 
/// @param prefab 
void Engine::DX12Offscreen::DrawMotionVector(ID3D12GraphicsCommandList* commandList, DX12Render* render, DX12Prefab* prefab)
{
	// モーションベクトルが必要なポストエフェクトがない場合は描画しない
	if (!postEffectStore_->IsEnableMotionVector())
		return;

	// nullptrチェック
	assert(commandList);
	assert(render);
	assert(prefab);

	// モーションベクトルの描画コマンドを登録する
	postEffectStore_->DrawMotionVector(commandList, depthResource_->GetDsvCpuHandle(), render, prefab);

	// レンダーターゲットを戻す
	offscreenResource_[currentOffscreen_]->SetRenderTarget(commandList, depthResource_->GetDsvCpuHandle());
}

/// @brief TAAを描画する
/// @param context 
void Engine::DX12Offscreen::DrawTAA(ID3D12GraphicsCommandList* commandList)
{
	// モーションベクトルが必要なポストエフェクトがない場合は描画しない
	if (!postEffectStore_->IsEnableMotionVector())return;

	// TAAを読み込んでいない場合は描画しない
	if (!postEffectStore_->IsLoadTAA())return;

	// nullptrチェック
	assert(commandList);

	const int32_t sourceOffscreenIndex = currentOffscreen_;

	// カウントする
	++currentOffscreen_;
	currentOffscreen_ = currentOffscreen_ % 2;

	// オフスクリーンのレンダーターゲット・デプスステンシルの設定とクリア
	ClearRenderTarget(commandList);

	PostEffectRenderContext context{};
	context.commandList = commandList;
	context.offscreenPixelShaderResource = offscreenResource_[sourceOffscreenIndex].get();
	context.offscreenRenderTargetResource = offscreenResource_[currentOffscreen_].get();
	context.psoCopyImage = psoCopyImage_.get();

	// 書き込み対象 -> 読み込ませテクスチャ
	TransitionBarrier(offscreenResource_[sourceOffscreenIndex]->GetResource(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, commandList);

	// TAAの描画コマンドを登録する
	postEffectStore_->DrawTAA(context);

	// 読み込ませテクスチャ -> 書き込み対象
	TransitionBarrier(offscreenResource_[sourceOffscreenIndex]->GetResource(),
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET, commandList);
}

/// @brief モーションブラーを描画する
/// @param commandList 
void Engine::DX12Offscreen::DrawMotionBlur(ID3D12GraphicsCommandList* commandList)
{
	// モーションベクトルが必要なポストエフェクトがない場合は描画しない
	if (!postEffectStore_->IsEnableMotionVector())return;

	// モーションブラーを読み込んでいない場合は描画しない
	if (!postEffectStore_->IsLoadMotionBlur())return;

	// nullptrチェック
	assert(commandList);

	const int32_t sourceOffscreenIndex = currentOffscreen_;

	// カウントする
	++currentOffscreen_;
	currentOffscreen_ = currentOffscreen_ % 2;

	// オフスクリーンのレンダーターゲット・デプスステンシルの設定とクリア
	ClearRenderTarget(commandList);

	PostEffectRenderContext context{};
	context.commandList = commandList;
	context.offscreenPixelShaderResource = offscreenResource_[sourceOffscreenIndex].get();
	context.psoCopyImage = psoCopyImage_.get();

	// 書き込み対象 -> 読み込ませテクスチャ
	TransitionBarrier(offscreenResource_[sourceOffscreenIndex]->GetResource(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, commandList);

	// モーションブラーの描画コマンドを登録する
	postEffectStore_->DrawMotionBlur(context);

	// 読み込ませテクスチャ -> 書き込み対象
	TransitionBarrier(offscreenResource_[sourceOffscreenIndex]->GetResource(),
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET, commandList);
}

/// @brief 残像を描画する
/// @param commandList 
void Engine::DX12Offscreen::DrawAfterImage(ID3D12GraphicsCommandList* commandList, Camera3DStore* cameraStore)
{
	// 残像を読み込んでいない場合は描画しない
	if (!postEffectStore_->IsLoadAfterImage())return;

	// nullptrチェック
	assert(commandList);
	assert(cameraStore);
	const int32_t sourceOffscreenIndex = currentOffscreen_;

	// カウントする
	++currentOffscreen_;
	currentOffscreen_ = currentOffscreen_ % 2;

	PostEffectRenderContext context{};
	context.commandList = commandList;
	context.offscreenPixelShaderResource = offscreenResource_[sourceOffscreenIndex].get();
	context.offscreenRenderTargetResource = offscreenResource_[currentOffscreen_].get();
	context.camera3DStore = cameraStore;
	context.depthResource = depthResource_.get();
	context.psoCopyImage = psoCopyImage_.get();

	// 書き込み対象 -> 読み込ませテクスチャ
	TransitionBarrier(offscreenResource_[sourceOffscreenIndex]->GetResource(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, commandList);

	// 残像の描画コマンドを登録する
	postEffectStore_->DrawAfterImage(context);

	// 読み込ませテクスチャ -> 書き込み対象
	TransitionBarrier(offscreenResource_[sourceOffscreenIndex]->GetResource(),
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET, commandList);
}

/// @brief デバッグ用パラメータ
void Engine::DX12Offscreen::DebugParameter()
{
#ifdef _DEVELOPMENT

	// メニューバーを使用する
	if (!ImGui::Begin("PostEffect"))
	{
		ImGui::End();
		return;
	}

	postEffectStore_->DebugParameter();

	// 終了
	ImGui::End();

#endif
}