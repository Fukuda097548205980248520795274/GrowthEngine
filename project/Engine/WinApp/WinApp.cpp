#include "WinApp.h"

#include <format>
#include <imgui.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>

#include "Func/ConvertString/ConvertString.h"
#include "Log/Log.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

/// @brief ウィンドウプロシージャ
/// @param hwnd 
/// @param msg 
/// @param wparam 
/// @param lparam 
/// @return 
LRESULT CALLBACK Engine::WinApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	// ImGuiを操作すると途中で打ち切ることができる
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}

	// メッセージに応じて固有の処理を行う
	switch (msg)
	{
	case WM_DESTROY:
		// ウィンドウが破棄された

		// OSに対してアプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}

	// 標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);
}



/// @brief デストラクタ
Engine::WinApp::~WinApp()
{
	// OSの精度を戻す
	timeEndPeriod(1);

	// ウィンドウハンドルを破棄する
	if (hwnd_)
		DestroyWindow(hwnd_);
}


/// @brief 初期化
/// @param clientWidth 
/// @param clientHeight 
/// @param title 
/// @param log 
void Engine::WinApp::Initialize(int32_t clientWidth, int32_t clientHeight, const std::string& title, Log* log)
{

	// 引数を受け取る
	clientWidth_ = clientWidth;
	clientHeight_ = clientHeight;

	// システムタイマーの分解能を上げる
	timeBeginPeriod(1);


	/*---------------------------
		ウィンドウクラスを登録する
	---------------------------*/

	// ウィンドウプロシージャ
	wc_.lpfnWndProc = WindowProc;

	// ウィンドウクラス名
	wc_.lpszClassName = L"Growth";

	// インスタンスハンドル
	wc_.hInstance = GetModuleHandle(nullptr);

	// カーソル
	wc_.hCursor = LoadCursor(nullptr, IDC_ARROW);

	// 登録する
	RegisterClass(&wc_);


	// ログ
	if (log)log->Logging("lpszClassName : Growth");
	if (log)log->Logging("RegisterClass \n");


	/*---------------------------
		ウィンドウサイズを決める
	---------------------------*/

	// ウィンドウサイズを表す構造体にクライアント領域を入れる
	wrc_ = { 0 , 0 , clientWidth_ , clientHeight_ };

	// クライアント領域をもとに、実際のサイズにwrcを変更してもらう
	AdjustWindowRect(&wrc_, WS_OVERLAPPEDWINDOW, false);


	// ログ
	if (log)log->Logging(std::format("clientSize : width = {} , height = {}",clientWidth_, clientHeight_));
	if (log)log->Logging("AdjustWindowRect \n");


	/*---------------------------
		ウィンドウを生成して表示
	---------------------------*/

	// ウィンドウの生成
	hwnd_ = CreateWindow(
		// 利用するクラス名
		wc_.lpszClassName,

		// タイトルバーの文字
		ConvertString(title).c_str(),

		// ウィンドウスタイル
		WS_OVERLAPPEDWINDOW,

		// 表示座標
		CW_USEDEFAULT,
		CW_USEDEFAULT,

		// ウィンドウの大きさ
		wrc_.right - wrc_.left,
		wrc_.bottom - wrc_.top,

		// 親ウィンドウハンドル
		nullptr,

		// メニューハンドル
		nullptr,

		// インスタンスハンドル
		wc_.hInstance,

		// オプション
		nullptr
	);

	// ウィンドウを表示する
	ShowWindow(hwnd_, SW_SHOW);


	// ログ
	if (log)log->Logging("ShowWindow \n");
}

/// @brief 更新処理
void Engine::WinApp::Update()
{
	prevClientWidth_ = clientWidth_;
	prevClientHeight_ = clientHeight_;

	GetClientRect(hwnd_, &wrc_);

	clientWidth_ = wrc_.right - wrc_.left;
	clientHeight_ = wrc_.bottom - wrc_.top;

	isResized_ =
		(clientWidth_ != prevClientWidth_) ||
		(clientHeight_ != prevClientHeight_);
}

/// @brief ウィンドウにメッセージを渡して応答する
/// @return 
bool Engine::WinApp::ProcessMessage()
{
	MSG msg{};

	// ウィンドウにメッセージが来ていたら最優先で処理させる
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		// ウィンドウでxボタンが押されたら終了
		if (msg.message == WM_QUIT)return false;

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return true;
}