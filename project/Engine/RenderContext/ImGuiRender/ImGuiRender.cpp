#include "ImGuiRender.h"
#include "WinApp/WinApp.h"
#include "RenderContext/DX12Buffering/DX12Buffering.h"
#include "RenderContext/DX12Heap/DX12Heap.h"
#include "Func/ResourceFunc/ResourceFunc.h"
#include "Log/Log.h"
#include <cassert>
#include <filesystem>

// Dear ImGuiの初期レイアウト
const char* defaultImguiIni = R"(
[Window][DockSpace]
Pos=0,18
Size=1280,702
Collapsed=0

[Window][Debug##Default]
Pos=60,60
Size=400,400
Collapsed=0

[Window][Dear ImGui Demo]
Pos=0,18
Size=967,490
Collapsed=0
DockId=0x00000001,1

[Window][Texture]
Pos=0,510
Size=1222,210
Collapsed=0
DockId=0x00000005,0

[Window][Reset Scene]
Pos=1224,510
Size=56,210
Collapsed=0
DockId=0x00000006,0

[Window][Stage Editor]
Pos=0,550
Size=1222,170
Collapsed=0
DockId=0x00000005,2

[Window][Chip Selector]
Pos=0,550
Size=1222,170
Collapsed=0
DockId=0x00000005,1

[Window][Camera3D]
Pos=928,18
Size=352,490
Collapsed=0
DockId=0x00000002,4

[Window][Model]
Pos=928,18
Size=352,490
Collapsed=0
DockId=0x00000002,3

[Window][Prefab]
Pos=928,18
Size=352,490
Collapsed=0
DockId=0x00000002,2

[Window][PostEffect]
Pos=928,18
Size=352,490
Collapsed=0
DockId=0x00000002,1

[Window][Light]
Pos=928,18
Size=352,490
Collapsed=0
DockId=0x00000002,0

[Window][View]
Pos=0,18
Size=926,490
Collapsed=0
DockId=0x00000001,0

[Window][Particle]
Pos=928,18
Size=352,490
Collapsed=0
DockId=0x00000002,5

[Window][Sound]
Pos=928,18
Size=352,490
Collapsed=0
DockId=0x00000002,6

[Window][Behavior Tree Editor]
Pos=0,18
Size=926,490
Collapsed=0
DockId=0x00000001,1

[Window][Motion Manager Editor]
Pos=0,18
Size=926,490
Collapsed=0
DockId=0x00000001,2

[Docking][Data]
DockSpace     ID=0x14621557 Window=0x3DA2F1DE Pos=0,18 Size=1280,702 Split=Y Selected=0x5E5F7166
  DockNode    ID=0x00000003 Parent=0x14621557 SizeRef=1280,490 Split=X
    DockNode  ID=0x00000001 Parent=0x00000003 SizeRef=926,702 CentralNode=1 Selected=0xCD8A0BD6
    DockNode  ID=0x00000002 Parent=0x00000003 SizeRef=352,702 Selected=0x216DA292
  DockNode    ID=0x00000004 Parent=0x14621557 SizeRef=1280,210 Split=X Selected=0xBE78FE5F
    DockNode  ID=0x00000005 Parent=0x00000004 SizeRef=1222,170 Selected=0xFFE73297
    DockNode  ID=0x00000006 Parent=0x00000004 SizeRef=56,170 Selected=0xFDB80A1C
)";

/// @brief デストラクタ
Engine::ImGuiRender::~ImGuiRender()
{
	// imgui-node-editor のコンテキストを破棄する
	if (nodeEditorContext_ != nullptr)
	{
		ax::NodeEditor::DestroyEditor(nodeEditorContext_);
		nodeEditorContext_ = nullptr;
	}

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

	// 引数を受け取る
	winApp_ = winApp;

	// SRVハンドルを取得する
	srvHandle_.first = heap->GetSrvCPUDescriptorHandle();
	srvHandle_.second = heap->GetSrvGPUDescriptorHandle();

	// サイズを取得
	screenWidth_ = static_cast<float>(buffering->GetSwapChainDesc().Width);
	screenHeight_ = static_cast<float>(buffering->GetSwapChainDesc().Height);

	
	// ImGuiを初期化する
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// ImguiNodeEditorの初期化
	if (nodeEditorContext_ == nullptr)
	{
		ImNode::Config config;
		config.SettingsFile = "NodeEditor.json"; // 設定を保存するファイル名（任意）
		nodeEditorContext_ = ImNode::CreateEditor(&config);
	}

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

	// imgui.iniの読み込み
	std::fstream f("imgui.ini");

	// ファイルが存在すれば読み込む、なければデフォルトのレイアウトを読み込む
	if (f.is_open()) 
	{
		f.close();
	}
	else 
	{
		// デフォルトのレイアウトを読み込む
		ImGui::LoadIniSettingsFromMemory(defaultImguiIni);
	}

	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(winApp_->GetHwnd());
	ImGui_ImplDX12_Init(device, buffering->GetSwapChainDesc().BufferCount,
		buffering->GetRtvDesc().Format, heap->GetSrvDescriptorHeap(),srvHandle_.first, srvHandle_.second);
}

/// @brief リサイズ
/// @param width 
/// @param height 
void Engine::ImGuiRender::Resize(int32_t width, int32_t height)
{
	screenWidth_ = static_cast<float>(width);
	screenHeight_ = static_cast<float>(height);
}

/// @brief フレーム開始
void Engine::ImGuiRender::FrameStart()
{
	// フレームの開始をImGuiに伝える
	ImGui_ImplWin32_NewFrame();
	ImGui_ImplDX12_NewFrame();
	ImGui::NewFrame();

	ImGuizmo::BeginFrame();
	ImGuizmo::SetOrthographic(true);
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

	ImGui::Begin("View");

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

	// 画像が描かれている領域をそのままギズモの Rect にする
	ImVec2 windowPos = ImGui::GetWindowPos();
	ImVec2 gizmoPos = ImVec2(windowPos.x + newCursorPos.x,
		windowPos.y + newCursorPos.y);


	ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());

	ImGuizmo::SetRect(gizmoPos.x, gizmoPos.y, imageSize.x, imageSize.y);

	ImGui::SetCursorPos(newCursorPos);

	ImGui::Image(texId, imageSize);


	// Imageの矩形取得
	ImVec2 imageMin = ImGui::GetItemRectMin();

	// マウス
	ImVec2 mousePos = ImGui::GetMousePos();

	float renderWidth = static_cast<float>(winApp_->GetClientWidth());
	float renderHeight = static_cast<float>(winApp_->GetClientHeight());

	viewWindowCursorPos_.x = (mousePos.x - imageMin.x) * (renderWidth / imageSize.x);
	viewWindowCursorPos_.y = (mousePos.y - imageMin.y) * (renderHeight / imageSize.y);

	// ウィンドウ内をホバーしているかどうか
	isViewWindowHover_ = ImGui::IsItemHovered();

	// ローカル座標
	viewWindowCursorPos_.x = std::clamp(viewWindowCursorPos_.x, 0.0f, renderWidth);
	viewWindowCursorPos_.y = std::clamp(viewWindowCursorPos_.y, 0.0f, renderHeight);


	ImGui::End();


	// ImGuiの内部コマンドを生成する
	ImGui::Render();

	// ImGuiを描画する
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);

	// PixelShaderResource -> RenderTarget
	TransitionBarrier(resource,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET, commandList);
}