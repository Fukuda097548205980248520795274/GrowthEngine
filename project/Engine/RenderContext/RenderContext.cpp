#include "RenderContext.h"
#include "Log/Log.h"
#include <cassert>
#include "Func/ResourceFunc/ResourceFunc.h"
#include "WinApp/WinApp.h"


/// @brief 初期化
/// @param log 
void Engine::RenderContext::Initialize(WinApp* winApp, Log* log)
{
	// nullptrチェック
	assert(winApp);

#ifdef _DEBUG
	// DX12Debugの生成と初期化
	debug_ = std::make_unique<DX12Debug>();
	debug_->Initialize(log);
#endif

	// DX12Coreの生成と初期化
	core_ = std::make_unique<DX12Core>();
	core_->Initialize(log);

#ifdef _DEBUG
	// デバッグモードならデバイス生成後に警告・エラーで停止させる
	debug_->Stop(core_->GetDevice());
#endif

	// DX12Commandの生成と初期化
	command_ = std::make_unique<DX12Command>();
	command_->Initialize(core_->GetDevice(), log);

	// DX12Heapの生成と初期化
	heap_ = std::make_unique<DX12Heap>();
	heap_->Initialize(core_->GetDevice(), log);

	// DX12Bufferingの生成と初期化
	buffering_ = std::make_unique<DX12Buffering>();
	buffering_->Initialize(log, heap_.get(), winApp,
		core_->GetDXGIFactory(), core_->GetDevice(), command_->GetCommandQueue());

	// DX12Fenceの生成と初期化
	fence_ = std::make_unique<DX12Fence>();
	fence_->Initialize(log, core_->GetDevice());

	// シェーダコンパイラの生成と初期化
	shaderCompiler_ = std::make_unique<ShaderCompiler>();
	shaderCompiler_->Initialize(log);

	// DX12Offscreenの生成と初期化
	offscreen_ = std::make_unique<DX12Offscreen>();
	offscreen_->Initialize(core_->GetDevice(), heap_.get(), buffering_.get(), shaderCompiler_.get(), log);

	// テクスチャストアの生成
	textureStore_ = std::make_unique<TextureStore>();


	// ビューポートの設定
	viewport_.Width = static_cast<float>(winApp->GetClientWidth());
	viewport_.Height = static_cast<float>(winApp->GetClientHeight());
	viewport_.TopLeftX = 0;
	viewport_.TopLeftY = 0;
	viewport_.MinDepth = 0.0f;
	viewport_.MaxDepth = 1.0f;

	// シザー矩形の設定
	scissorRect_.left = 0;
	scissorRect_.right = winApp->GetClientWidth();
	scissorRect_.top = 0;
	scissorRect_.bottom = winApp->GetClientHeight();


	// ImGuiの初期設定
#ifdef _DEVELOPMENT
	imguiRender_ = std::make_unique<ImGuiRender>();
	imguiRender_->Initialize(core_->GetDevice(), winApp, heap_.get(), buffering_.get(), log);
#endif
}

/// @brief 描画前処理
void Engine::RenderContext::PreDraw()
{
#ifdef _DEVELOPMENT
	// フレームの開始をImGuiに伝える
	imguiRender_->FrameStart();
#endif

	// コマンドリストの取得
	commandList_ = command_->GetCommandList();

	// ビューポート、シザー矩形の設定
	commandList_->RSSetViewports(1, &viewport_);
	commandList_->RSSetScissorRects(1, &scissorRect_);

	// オフスクリーンのクリア
	offscreen_->Clear(commandList_);

	// 描画用のディスクリプタヒープを設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { heap_->GetSrvDescriptorHeap() };
	commandList_->SetDescriptorHeaps(1, descriptorHeaps);

#ifdef _DEVELOPMENT
	// Dockスペースを作成する
	imguiRender_->CreateDockSpace();
	ImGui::ShowDemoWindow();
#endif 
}

/// @brief 描画後処理
void Engine::RenderContext::PostDraw()
{
	// コマンドリスト・アロケータの取得
	commandList_ = command_->GetCommandList();
	commandAllocator_ = command_->GetCommandAllocator();

	// バックバッファのインデックス・リソース・RTV用CPUハンドルを取得
	UINT backBufferIndex = buffering_->GetSwapChain()->GetCurrentBackBufferIndex();
	ID3D12Resource* backBufferResource = buffering_->GetSwapChainResource(backBufferIndex);
	D3D12_CPU_DESCRIPTOR_HANDLE backBufferCPUHandle = buffering_->GetSwapChainRtvCPUHandle(backBufferIndex);



	// バックバッファリソース Present -> RenderTarget
	TransitionBarrier(backBufferResource, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET, commandList_);

	// 描画先のRTVを設定する
	commandList_->OMSetRenderTargets(1, &backBufferCPUHandle, false, nullptr);

	// 指定した色で画面全体をクリアする
	float clearColor[] = { 0.1f , 0.1f , 0.1f , 0.0f };
	commandList_->ClearRenderTargetView(backBufferCPUHandle, clearColor, 0, nullptr);

	// スワップチェインのリソースにオフスクリーンテクスチャを書き込む
	offscreen_->RenderSwapChain(commandList_);

	// ImGuiDockingに最終的なオフスクリーンを描画する
#ifdef _DEVELOPMENT
	// ImGuiに表示するスクリーンを描画する
	imguiRender_->DrawImGuiScreen(offscreen_->GetCurrentResource(), offscreen_->GetCurrentResourceSrvHandle(), commandList_);
#endif

	// バックバッファリソース RenderTarget -> Present
	TransitionBarrier(backBufferResource, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT, commandList_);



	// コマンドの内容を確定させる（閉じる）
	HRESULT hr = commandList_->Close();
	assert(SUCCEEDED(hr));

	// GPUにコマンドリストの実行を行わせる
	ID3D12CommandList* commandLists[] = { commandList_ };
	command_->GetCommandQueue()->ExecuteCommandLists(1, commandLists);

	// GPUとOSに画面の交換を行うよう通知する
	buffering_->GetSwapChain()->Present(1, 0);

	// GPUにシグナルを送る
	fence_->SendSignal(command_->GetCommandQueue());

	// フェンスの値を確認してGPUを待つ
	fence_->WaitGPU();

	// 次のフレーム用のコマンドリストを準備
	hr = commandAllocator_->Reset();
	assert(SUCCEEDED(hr));
	hr = commandList_->Reset(commandAllocator_, nullptr);
	assert(SUCCEEDED(hr));
}