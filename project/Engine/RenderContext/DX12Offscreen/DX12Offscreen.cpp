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
void Engine::DX12Offscreen::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList,
	DX12Heap* heap, DX12Buffering* buffering, ShaderCompiler* compiler, TextureStore* textureStore, Log* log)
{
	// nullptrチェック
	assert(device);
	assert(commandList);
	assert(heap);
	assert(buffering);
	assert(compiler);
	assert(textureStore);

	// 引数を受け取る
	heap_ = heap;
	buffering_ = buffering;

	int width = static_cast<int32_t>(buffering->GetSwapChainDesc().Width);
	int height = static_cast<int32_t>(buffering->GetSwapChainDesc().Height);

	// 深度リソースを生成する
	depthResource_ = std::make_unique<DepthResource>();
	depthResource_->Initialize(device, width, height, heap, log);


	// 頂点シェーダを読み込む
	vertexShaderBlob_ = compiler->Compile(L"./Assets/Shader/Fullscreen/Fullscreen.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob_);

	// ピクセルシェーダーを読み込む
	pixelShaderBlob_ = compiler->Compile(L"./Assets/Shader/Fullscreen/Fullscreen.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob_);

	// フルスクリーンPSOの生成
	psoFullscreen_ = std::make_unique<PSOFullscreen>();
	psoFullscreen_->Initialize(device, vertexShaderBlob_.Get(), pixelShaderBlob_.Get(), log);


	// レンダーターゲットプールの生成
	renderTargetPool_ = std::make_unique<RenderTargetPool>();
	renderTargetPool_->Initialize(device, heap, buffering, commandList, 10);

	// ポストエフェクトストアの生成
	postEffectStore_ = std::make_unique<PostEffectStore>();
	postEffectStore_->Initialize(device, compiler, vertexShaderBlob_.Get(),
		heap, textureStore, static_cast<int32_t>(buffering_->GetSwapChainDesc().Width), static_cast<int32_t>(buffering_->GetSwapChainDesc().Height), log);

	// レンダーパスストアの生成
	renderPassStore_ = std::make_unique<RenderPassStore>();
	renderPassStore_->Initialize(renderTargetPool_.get());
}

/// @brief シーン前のリセット
void Engine::DX12Offscreen::PerSceneReset()
{
	postEffectStore_->PerSceneReset();
}

/// @brief フレームの最後の処理
void Engine::DX12Offscreen::EndFrame(ID3D12GraphicsCommandList* commandList)
{
	renderPassStore_->Return();
	sourceResource_ = nullptr;
}

/// @brief サイズを作り直す
/// @param device 
/// @param buffering 
void Engine::DX12Offscreen::Resize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, DX12Buffering* buffering)
{
	int width = static_cast<int32_t>(buffering->GetSwapChainDesc().Width);
	int height = static_cast<int32_t>(buffering->GetSwapChainDesc().Height);

	// レンダーターゲットプールのリサイズ
	renderTargetPool_->Resize(width, height);

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

	// オフスクリーンのデプスステンシルの設定とクリア
	ClearDepthStencil(commandList);
}


/// @brief スワップチェインのRTVリソースにオフクリーンリソースを書き込む
/// @param commandList 
void Engine::DX12Offscreen::RenderSwapChain(ID3D12GraphicsCommandList* commandList)
{
	// nullptrチェック
	assert(commandList);

	// PSOの設定
	psoFullscreen_->Register(commandList, BlendMode::kNone);

	// テクスチャ
	currentResource_->RegisterGraphics(commandList, 0);

	// 形状は三角形
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 頂点は3つ
	commandList->DrawInstanced(3, 1, 0, 0);
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
	const bool kIsUseDepth = postEffectStore_->IsRequiredInput(hPostEffect, PostEffectInput::DepthTexture);
	const bool kIsBloom = postEffectStore_->IsBloom(hPostEffect);

	// ソースリソースがnullptrの場合はレンダリングターゲットプールから借りる
	if (!sourceResource_)
	{
		sourceResource_ = renderTargetPool_->Rent(commandList);
		renderPassStore_->SetActiveResources(sourceResource_);
	}

	// レンダーターゲットの設定とクリア
	sourceResource_->Barrier(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
	sourceResource_->ClearRenderTarget(commandList, depthResource_->GetDsvCpuHandle());

	destinationResource_->Barrier(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	// sourceとdestinationを入れ替える
	OffscreenResource* temp = sourceResource_;
	sourceResource_ = destinationResource_;
	destinationResource_ = temp;

	// ブルームは複数回描画する必要があるため、描画コマンドの登録の仕方を変える
	if (kIsBloom)
	{
		// 登録用コンテキストを作る
		PostEffectRenderContext registerContext{};
		registerContext = context;
		registerContext.commandList = commandList;
		registerContext.offscreenPixelShaderResource = sourceResource_;
		registerContext.offscreenRenderTargetResource = destinationResource_;
		registerContext.depthResource = kIsUseDepth ? depthResource_.get() : nullptr;

		// ポストエフェクトの描画コマンドを登録する
		postEffectStore_->DrawPostEffect(hPostEffect, registerContext);
	}
	else
	{
		if (kIsUseDepth)
		{
			// 深度書き込み -> 読み込みテクスチャ
			TransitionBarrier(depthResource_->GetResource(),
				D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, commandList);
		}

		// 登録用コンテキストを作る
		PostEffectRenderContext registerContext{};
		registerContext = context;
		registerContext.commandList = commandList;
		registerContext.offscreenPixelShaderResource = sourceResource_;
		registerContext.offscreenRenderTargetResource = destinationResource_;
		registerContext.depthResource = kIsUseDepth ? depthResource_.get() : nullptr;

		// ポストエフェクトの描画コマンドを登録する
		postEffectStore_->DrawPostEffect(hPostEffect, registerContext);

		if (kIsUseDepth)
		{
			// 読み込みテクスチャ -> 深度書き込み
			TransitionBarrier(depthResource_->GetResource(),
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE, commandList);
		}
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
	const bool kIsUseDepth = postEffectStore_->IsRequiredInput(name, PostEffectInput::DepthTexture);
	const bool kIsBloom = postEffectStore_->IsBloom(name);

	// ソースリソースがnullptrの場合はレンダリングターゲットプールから借りる
	if (!sourceResource_)
	{
		sourceResource_ = renderTargetPool_->Rent(commandList);
		renderPassStore_->SetActiveResources(sourceResource_);
	}

	// レンダーターゲットの設定とクリア
	sourceResource_->Barrier(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
	sourceResource_->ClearRenderTarget(commandList, depthResource_->GetDsvCpuHandle());

	destinationResource_->Barrier(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	// sourceとdestinationを入れ替える
	OffscreenResource* temp = sourceResource_;
	sourceResource_ = destinationResource_;
	destinationResource_ = temp;

	// ブルームは複数回描画する必要があるため、描画コマンドの登録の仕方を変える
	if (kIsBloom)
	{
		// 登録用コンテキストを作る
		PostEffectRenderContext registerContext{};
		registerContext = context;
		registerContext.commandList = commandList;
		registerContext.offscreenPixelShaderResource = sourceResource_;
		registerContext.offscreenRenderTargetResource = destinationResource_;
		registerContext.depthResource = kIsUseDepth ? depthResource_.get() : nullptr;
		registerContext.psoFullscreen = psoFullscreen_.get();

		// ポストエフェクトの描画コマンドを登録する
		postEffectStore_->DrawPostEffect(name, registerContext);
	}
	else
	{
		if (kIsUseDepth)
		{
			// 深度書き込み -> 読み込みテクスチャ
			TransitionBarrier(depthResource_->GetResource(),
				D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, commandList);
		}

		// 登録用コンテキストを作る
		PostEffectRenderContext registerContext{};
		registerContext = context;
		registerContext.commandList = commandList;
		registerContext.offscreenPixelShaderResource = sourceResource_;
		registerContext.offscreenRenderTargetResource = destinationResource_;
		registerContext.depthResource = kIsUseDepth ? depthResource_.get() : nullptr;

		// ポストエフェクトの描画コマンドを登録する
		postEffectStore_->DrawPostEffect(name, registerContext);

		if (kIsUseDepth)
		{
			// 読み込みテクスチャ -> 深度書き込み
			TransitionBarrier(depthResource_->GetResource(),
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE, commandList);
		}
	}
}

/// @brief レンダーパスを実行する
/// @param handle 
/// @param commandList 
/// @param dsvHandle 
/// @param inputResource 
void Engine::DX12Offscreen::ExecuteRenderPass(RenderPassHandle handle, ID3D12GraphicsCommandList* commandList)
{
	// nullptrチェック
	assert(commandList);
	assert(renderPassStore_);
	assert(depthResource_);

	// 内部のデプスリソースからDSVハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = depthResource_->GetDsvCpuHandle();

	// レンダーパスを実行し、出力リソースを取得してcurrentResource_に設定する
	currentResource_ = renderPassStore_->RenderPassDraw(handle, this, commandList, dsvHandle);
}

/// @brief レンダーパスを実行する
/// @param name 
/// @param commandList 
/// @param dsvHandle 
/// @param inputResource 
void Engine::DX12Offscreen::ExecuteRenderPass(const std::string& name, ID3D12GraphicsCommandList* commandList)
{
	// nullptrチェック
	assert(commandList);
	assert(renderPassStore_);
	assert(depthResource_);

	// 内部のデプスリソースからDSVハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = depthResource_->GetDsvCpuHandle();

	// レンダーパスを実行し、出力リソースを取得してcurrentResource_に設定する
	currentResource_ = renderPassStore_->RenderPassDraw(name, this, commandList, dsvHandle);
}

/// @brief レンダーパスに描画する
/// @param renderTargetHandle 
/// @param sourceHandle 
/// @param commandList 
void Engine::DX12Offscreen::DrawToRenderPass(RenderPassHandle renderTargetHandle, RenderPassHandle sourceHandle, ID3D12GraphicsCommandList* commandList)
{
	renderPassStore_->DrawToRenderPass(renderTargetHandle, sourceHandle, commandList, psoFullscreen_.get());
}

/// @brief レンダーパスに描画する
/// @param renderTargetName 
/// @param sourceName 
/// @param commandList 
void Engine::DX12Offscreen::DrawToRenderPass(const std::string& renderTargetName, const std::string& sourceName, ID3D12GraphicsCommandList* commandList)
{
	renderPassStore_->DrawToRenderPass(renderTargetName, sourceName, commandList, psoFullscreen_.get());
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
	currentResource_->SetRenderTarget(commandList, depthResource_->GetDsvCpuHandle());
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


	// レンダーターゲットの設定とクリア
	sourceResource_->Barrier(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
	sourceResource_->ClearRenderTarget(commandList, depthResource_->GetDsvCpuHandle());

	destinationResource_->Barrier(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	// sourceとdestinationを入れ替える
	OffscreenResource* temp = sourceResource_;
	sourceResource_ = destinationResource_;
	destinationResource_ = temp;


	PostEffectRenderContext context{};
	context.commandList = commandList;
	context.offscreenPixelShaderResource = sourceResource_;
	context.offscreenRenderTargetResource = destinationResource_;
	context.psoFullscreen = psoFullscreen_.get();

	// TAAの描画コマンドを登録する
	postEffectStore_->DrawTAA(context);

	// CSで書き込んだリソースをPSで読み込むためにバリアを張る
	sourceResource_->Barrier(commandList, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
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


	// レンダーターゲットの設定とクリア
	sourceResource_->Barrier(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
	sourceResource_->ClearRenderTarget(commandList, depthResource_->GetDsvCpuHandle());

	destinationResource_->Barrier(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	// sourceとdestinationを入れ替える
	OffscreenResource* temp = sourceResource_;
	sourceResource_ = destinationResource_;
	destinationResource_ = temp;


	PostEffectRenderContext context{};
	context.commandList = commandList;
	context.offscreenPixelShaderResource = sourceResource_;
	context.offscreenRenderTargetResource = destinationResource_;
	context.psoFullscreen = psoFullscreen_.get();

	// モーションブラーの描画コマンドを登録する
	postEffectStore_->DrawMotionBlur(context);


	sourceResource_->Barrier(commandList, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
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


	// レンダーターゲットの設定とクリア
	sourceResource_->Barrier(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);

	destinationResource_->Barrier(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	// sourceとdestinationを入れ替える
	OffscreenResource* temp = sourceResource_;
	sourceResource_ = destinationResource_;
	destinationResource_ = temp;


	PostEffectRenderContext context{};
	context.commandList = commandList;
	context.offscreenPixelShaderResource = sourceResource_;
	context.offscreenRenderTargetResource = destinationResource_;
	context.camera3DStore = cameraStore;
	context.depthResource = depthResource_.get();
	context.psoFullscreen = psoFullscreen_.get();

	// 残像の描画コマンドを登録する
	postEffectStore_->DrawAfterImage(context);

	sourceResource_->Barrier(commandList, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

/// @brief アウトラインを描画する
/// @param commandList 
/// @param render 
/// @param prefab 
void Engine::DX12Offscreen::DrawOutline(ID3D12GraphicsCommandList* commandList, DX12Render* render, DX12Prefab* prefab)
{
	// 残像を読み込んでいない場合は描画しない
	if (!postEffectStore_->IsLoadOutline())return;

	// nullptrチェック
	assert(commandList);
	assert(render);
	assert(prefab);

	// バリアを張る
	sourceResource_->Barrier(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
	destinationResource_->Barrier(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	// sourceとdestinationを入れ替える
	OffscreenResource* temp = sourceResource_;
	sourceResource_ = destinationResource_;
	destinationResource_ = temp;


	PostEffectRenderContext context{};
	context.commandList = commandList;
	context.offscreenPixelShaderResource = sourceResource_;
	context.offscreenRenderTargetResource = destinationResource_;
	context.depthResource = depthResource_.get();
	context.psoFullscreen = psoFullscreen_.get();
	context.dx12Render = render;
	context.dx12Prefab = prefab;

	// アウトラインの描画コマンドを登録する
	postEffectStore_->DrawOutline(context);
}

/// @brief デバッグ用パラメータ
void Engine::DX12Offscreen::DebugParameter()
{
#ifdef DEVELOPMENT

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