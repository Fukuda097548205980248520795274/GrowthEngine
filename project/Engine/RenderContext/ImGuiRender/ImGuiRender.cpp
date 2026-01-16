#include "ImGuiRender.h"
#include "WinApp/WinApp.h"
#include "RenderContext/DX12Buffering/DX12Buffering.h"
#include "RenderContext/DX12Heap/DX12Heap.h"
#include "Func/ResourceFunc/ResourceFunc.h"
#include "Log/Log.h"
#include <cassert>
#include <filesystem>

#include <imgui.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>

/// @brief デストラクタ
Engine::ImGuiRender::~ImGuiRender()
{
	// ImGuiの終了処理
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

/// @brief 初期化
/// @param device 
/// @param winApp 
/// @param heap 
/// @param buffering 
void Engine::ImGuiRender::Initialize(ID3D12Device* device, WinApp* winApp, DX12Heap* heap, DX12Buffering* buffering, Log* log)
{
	// nullptrチェック
	assert(device);
	assert(winApp);
	assert(heap);
	assert(buffering);

	// SRVハンドルを取得する
	srvHandle_.first = heap->GetSrvCPUDescriptorHandle();
	srvHandle_.second = heap->GetSrvGPUDescriptorHandle();

	// サイズを取得
	screenWidth_ = static_cast<float>(buffering->GetSwapChainDesc().Width);
	screenHeight_ = static_cast<float>(buffering->GetSwapChainDesc().Height);

	
	// ImGuiを初期化する
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();

	namespace fs = std::filesystem;


	ImFontConfig config = {};
	config.SizePixels = 12.0f;

	const char* fontPath = "C:/Windows/Fonts/YuGothB.ttc";

	if (fs::exists(fontPath)) {
		ImFont* font = io.Fonts->AddFontFromFileTTF(
			fontPath,
			config.SizePixels,
			&config,
			io.Fonts->GetGlyphRangesJapanese());

		if (font) {
			io.FontDefault = font;
			io.FontGlobalScale = 1.0f;
			io.Fonts->Build();
		}
	}
	else {
		OutputDebugStringA("フォントファイルが存在しません: YuGothB.ttc\n");
	}

	// ドッキング機能を有効にする
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	// Viewport機能の初期化
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		// Win32で追加のウィンドウを作成・管理するための処理をフック
		ImGui_ImplWin32_EnableDpiAwareness();
	}

	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(winApp->GetHwnd());
	ImGui_ImplDX12_Init(device, buffering->GetSwapChainDesc().BufferCount,
		buffering->GetRtvDesc().Format, heap->GetSrvDescriptorHeap(),srvHandle_.first, srvHandle_.second);
}

/// @brief フレーム開始
void Engine::ImGuiRender::FrameStart()
{
	// フレームの開始をImGuiに伝える
	ImGui_ImplWin32_NewFrame();
	ImGui_ImplDX12_NewFrame();
	ImGui::NewFrame();
}

/// @brief Dockスペースを作成する
void Engine::ImGuiRender::CreateDockSpace()
{
	static bool opt_fullscreen = true;
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;

	if (opt_fullscreen)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}

	// パディングを0に（メインDockSpaceの余白をなくす）
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	ImGui::Begin("DockSpace", nullptr, window_flags);

	ImGui::PopStyleVar(3); // WindowPadding, Rounding, BorderSizeを戻す

	// DockSpace作成（バーなし、背景のみ）
	ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

	ImGui::End();
}

/// @brief ImGuiスクリーンを描画する
/// @param resource 
/// @param gpuHandle 
/// @param commandList 
void Engine::ImGuiRender::DrawImGuiScreen(ID3D12Resource* resource, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle, ID3D12GraphicsCommandList* commandList)
{
	// RenderTarget -> PixelShaderResource
	TransitionBarrier(resource,
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, commandList);

	ImGui::Begin("Scene");

	ImTextureID texId = (ImTextureID)(gpuHandle.ptr);

	ImVec2 availSize = ImGui::GetContentRegionAvail(); // ウィンドウ内の空きサイズ

	float aspectRatio = screenWidth_ / screenHeight_;

	// アスペクト比を保ちつつ、ウィンドウサイズ内に最大表示
	ImVec2 imageSize;

	float availAspect = availSize.x / availSize.y;
	if (availAspect > aspectRatio) {
		// 横に余裕あり → 高さに合わせる
		imageSize.y = availSize.y;
		imageSize.x = availSize.y * aspectRatio;
	}
	else {
		imageSize.x = availSize.x;
		imageSize.y = availSize.x / aspectRatio;
	}

	// 中央寄せ（X方向、Y方向両方）
	ImVec2 cursorPos = ImGui::GetCursorPos();
	ImVec2 newCursorPos = ImVec2(
		cursorPos.x + (availSize.x - imageSize.x) * 0.5f,
		cursorPos.y + (availSize.y - imageSize.y) * 0.5f
	);

	ImGui::SetCursorPos(newCursorPos);

	ImGui::Image(texId, imageSize);

	ImGui::End();


	// ImGuiの内部コマンドを生成する
	ImGui::Render();

	// ImGuiを描画する
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);

	// PixelShaderResource -> RenderTarget
	TransitionBarrier(resource,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET, commandList);
}