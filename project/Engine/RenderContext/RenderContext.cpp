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

	// FPS固定初期化
	InitializeFixFPS();

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

	// コマンドリストの取得
	commandList_ = command_->GetCommandList();

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

	// DX12Primitiveの生成と初期化
	primitive_ = std::make_unique<DX12Primitive>();
	primitive_->Initialize(core_->GetDevice(), shaderCompiler_.get(), log);

	// カメラストア
	cameraStore_ = std::make_unique<CameraStore>();

	// テクスチャストアの生成
	textureStore_ = std::make_unique<TextureStore>();

	// モデルストアの生成
	modelStore_ = std::make_unique<ModelStore>();

	// アニメーションストアの生成
	animationStore_ = std::make_unique<AnimationStore>();

	// ライトストアの生成と初期化
	lightStore_ = std::make_unique<LightStore>();


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

	// 描画用のディスクリプタヒープを設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { heap_->GetSrvDescriptorHeap() };
	commandList_->SetDescriptorHeaps(1, descriptorHeaps);





	// オフスクリーンのクリア
	offscreen_->Clear(commandList_);

	// カメラストアの更新
	cameraStore_->Update();

	// プリミティブストアの更新
	primitive_->Update(cameraStore_->GetCamera3D().GetViewProjectionMatrix());

#ifdef _DEVELOPMENT
	// Dockスペースを作成する
	imguiRender_->CreateDockSpace();
	ImGui::ShowDemoWindow();

	// テクスチャストアのUI
	textureStore_->DrawUI();
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

	// FPS固定更新処理
	UpdateFixFPS();

	// 次のフレーム用のコマンドリストを準備
	hr = commandAllocator_->Reset();
	assert(SUCCEEDED(hr));
	hr = commandList_->Reset(commandAllocator_, nullptr);
	assert(SUCCEEDED(hr));
}



/// @brief FPS固定初期化
void Engine::RenderContext::InitializeFixFPS()
{
	// 現在時間を初期化する
	reference_ = std::chrono::steady_clock::now();
}

/// @brief FPS固定更新処理
void Engine::RenderContext::UpdateFixFPS()
{
	// 1/60 秒ぴったりの時間
	const std::chrono::microseconds kMinTime(uint64_t(1000000.0f / 60.0f));

	// 1/60 秒よりわずかに短い時間
	const std::chrono::microseconds kMinCheckTime(uint64_t(1000000.0f / 65.0f));


	// 現在時間を取得する
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

	// 前回の記録から経過時間を取得する
	std::chrono::microseconds elapsed =
		std::chrono::duration_cast<std::chrono::microseconds>(now - reference_);


	// 1/60 秒、経過していない場合
	if (elapsed < kMinCheckTime)
	{
		// 1/60 秒経過するまで微小なスリープを繰り返す
		while (std::chrono::steady_clock::now() - reference_ < kMinTime)
		{
			// 1マイクロ秒スリープ
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}
	}

	// 現在の時間を記録する
	reference_ = std::chrono::steady_clock::now();
}