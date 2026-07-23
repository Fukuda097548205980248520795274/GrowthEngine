#include "GrowthEngine.h"
#include <cassert>
#include "Log/Log.h"
#include "Func/CrushHandler/CrushHandler.h"
#include "Func/ConvertString/ConvertString.h"
#include <chrono>

#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"Dbghelp.lib")
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dxcompiler.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "xinput.lib")
#pragma comment(lib, "Mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "Mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "xaudio2.lib")

// インスタンス
std::unique_ptr<GrowthEngine> GrowthEngine::pInstance_ = nullptr;

/// @brief インスタンスを取得する
/// @param screenWidth スクリーン横幅
/// @param screenHeight スクリーン縦幅
/// @param title タイトル
/// @return 
GrowthEngine* GrowthEngine::GetInstance(int32_t screenWidth, int32_t screenHeight, const std::string& title)
{
	if (pInstance_ == nullptr)
	{
		pInstance_.reset(new GrowthEngine());
		pInstance_->Initialize(screenWidth, screenHeight, title);
	}

	return pInstance_.get();
}

/// @brief インスタンスを取得する
/// @return 
GrowthEngine* GrowthEngine::GetInstance()
{
	// インスタンスがないと失敗
	assert(pInstance_ != nullptr);
	return pInstance_.get();
}


/// @brief 初期化
/// @param screenWidth 
/// @param screenHeight 
/// @param title 
void GrowthEngine::Initialize(int32_t screenWidth, int32_t screenHeight, const std::string& title)
{
	// 例外が発生したときに起動する
	SetUnhandledExceptionFilter(Engine::ExportDump);

	// COM初期化
	CoInitializeEx(0, COINIT_MULTITHREADED);

	// ログの生成
	pLog_ = std::make_unique<Engine::Log>();

	// ウィンドウアプリケーションの生成と初期化
	pWinApp_ = std::make_unique<Engine::WinApp>();
	pWinApp_->Initialize(screenWidth, screenHeight, title , pLog_.get());

	// 入力の生成と初期化
	pInput_ = std::make_unique<Engine::Input>();
	pInput_->Initialize(pWinApp_.get(), pLog_.get());

	// オーディオストアの生成と初期化
	pAudioStore_ = std::make_unique<Engine::AudioStore>();
	pAudioStore_->Initialize(pLog_.get());

	// サウンドストアの生成と初期化
	pSoundStore_ = std::make_unique<Engine::SoundStore>();
	pSoundStore_->Initialize(pAudioStore_.get());

	// 入力ストアの生成と初期化
	pInputStore_ = std::make_unique<Engine::InputStore>();
	pInputStore_->Initialize(pInput_.get());

	// 描画統括の生成と初期化
	pRenderContext_ = std::make_unique<Engine::RenderContext>();
	pRenderContext_->Initialize(pWinApp_.get(), pLog_.get());
}

/// @brief デストラクタ
GrowthEngine::~GrowthEngine()
{
	// 描画統括の終了
	pRenderContext_.reset();
	pRenderContext_ = nullptr;
	if (pLog_)pLog_->Logging("RenderContext released \n");

	// 入力ストアの終了
	pInputStore_.reset();
	pInputStore_ = nullptr;
	if (pLog_)pLog_->Logging("InputStore released \n");

	// サウンドストアの終了
	pSoundStore_.reset();
	pSoundStore_ = nullptr;
	if (pLog_)pLog_->Logging("SoundStore released \n");

	// オーディオストアの終了
	pAudioStore_.reset();
	pAudioStore_ = nullptr;
	if (pLog_)pLog_->Logging("AudioStore released \n");

	// 入力の終了
	pInput_.reset();
	pInput_ = nullptr;
	if (pLog_)pLog_->Logging("Input released \n");

	// ウィンドウアプリケーションの終了
	pWinApp_.reset();
	pWinApp_ = nullptr;
	if (pLog_)pLog_->Logging("WinApp released \n");

	// 解放漏れを検知する
#ifdef _DEBUG
	Engine::LeakChecker();
	if (pLog_)pLog_->Logging("LeakChecker executed \n");
#endif

	// ログの終了
	pLog_.reset();
	pLog_ = nullptr;

	// COM終了
	CoUninitialize();
}

/// @brief シーン前処理
void GrowthEngine::PerScene()
{
	// シーン前処理
	pRenderContext_->PerScene();

	// デルタタイムの初期化
	isDeltaTimeFirst_ = true;

	// タイムスケールの初期化
	timeScale_ = 1.0f;
	slowDuration_ = 0.0f;
}

/// @brief 新フレーム処理
void GrowthEngine::NewFrame()
{
	TimePoint currentTime = Clock::now();
	std::chrono::duration<float> deltaTime = currentTime - previousTime_;
	previousTime_ = currentTime;
	deltaTime_ = deltaTime.count();

	// デルタタイムが一週目のとき、または二週目のときはデルタタイムを0にする
	if(isDeltaTimeFirst_ || isDeltaTimeSecond_)
		deltaTime_ = 0.0f;

	// 全ての入力情報を取得する
	pInput_->CheckInputInfo();

	// Alt + Enter の同時押し
	bool isAltPressed = GetKeyPress(DIK_LALT) || GetKeyPress(DIK_RALT);
	bool isEnterPressed = GetKeyPress(DIK_RETURN);

	if (isAltPressed && isEnterPressed)
	{
		if (!isPushFullscreenButton_)
		{
			pWinApp_->Fullscreen();
		}
		isPushFullscreenButton_ = true;
	} else
	{
		isPushFullscreenButton_ = false;
	}

	// 入力ストアの更新
	pInputStore_->Update();

	// サウンドストアの更新
	pSoundStore_->Update();

	// オーディオストアの更新
	pAudioStore_->Update();

	// 新フレーム処理
	pRenderContext_->NewFrame();

	// ウィンドウの更新
	pWinApp_->Update();

	// タイムスケールのタイマーを更新する
	UpdateTimeScale();
}

/// @brief 描画前処理
void GrowthEngine::PreDraw() 
{
	// 描画前処理
	pRenderContext_->PreDraw();

#ifdef DEVELOPMENT

	// サウンドストアのパラメータを表示する
	pSoundStore_->DebugParameter();

	// マウスでオブジェクト選択
	if (pInput_->GetMouseTrigger(static_cast<uint32_t>(MouseButton::Left)))
		if (!ImGuizmo::IsOver())
			pRenderContext_->DebugRayPicking();


	// ImGuiがキーボード入力をキャプチャしているときはデルタタイムを0にする
	if (ImGui::GetIO().WantCaptureKeyboard)
	{
		isDeltaTimeFirst_ = true;
		isDeltaTimeSecond_ = true;
	}

#endif
}

/// @brief 描画後処理
void GrowthEngine::PostDraw()
{
#ifdef DEVELOPMENT

	if (ImGui::Begin("FPS Overlay"))
	{
		ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
		ImGui::Text("%.3f ms", 1000.0f / ImGui::GetIO().Framerate);
	}
	ImGui::End();
#endif

	// 描画後処理
	pRenderContext_->PostDraw();

	// 全ての入力情報をコピーする
	pInput_->CopyInputInfo();

	// デルタタイム二週目を終わらせる
	if(!isDeltaTimeFirst_)
		isDeltaTimeSecond_ = false;

	// デルタタイム一週目を終わらせる
	isDeltaTimeFirst_ = false;
}

/// @brief スローモーションを開始する
/// @param scale 
/// @param duration 
void GrowthEngine::StartSlowMotion(float scale, float duration)
{
	timeScale_ = scale;
	slowDuration_ = duration;
}

/// @brief タイムスケールのタイマーを更新する
void GrowthEngine::UpdateTimeScale()
{
	// スローモーションの時間を減らす
	if (slowDuration_ > 0.0f)
	{
		slowDuration_ -= GetDeltaTime();

		// スローモーションの時間が0以下になったらタイムスケールを元に戻す
		if (slowDuration_ <= 0.0f)
		{
			timeScale_ = 1.0f;
		}
	}
}

/// @brief マウスの位置を取得する
/// @return 
Vector2 GrowthEngine::GetMousePosition()const
{
	Vector2 mousePos = Vector2(0.0f, 0.0f);

#ifdef DEVELOPMENT

	// ビューウィンドウ内
	mousePos = pRenderContext_->GetViewWindowCursorPos();

#else

	// ウィンドウ内
	POINT p;
	GetCursorPos(&p);
	ScreenToClient(pWinApp_->GetHwnd(), &p);
	mousePos = Vector2(static_cast<float>(p.x), static_cast<float>(p.y));

#endif

	return mousePos;
}

/// @brief カーソルがウィンドウ内にホバーしているかどうか
/// @return 
bool GrowthEngine::IsCursorWindowHover() const
{
	bool isHover = false;

#ifdef DEVELOPMENT

	// ビューウィンドウ内
	isHover = pRenderContext_->IsViewWindowHover();

#else


	Vector2 mousePos = GetMousePosition();

	// ウィンドウ内
	if (mousePos.x >= 0.0f && mousePos.x <= static_cast<float>(pWinApp_->GetClientWidth()) &&
		mousePos.y >= 0.0f && mousePos.y <= static_cast<float>(pWinApp_->GetClientHeight()))
	{
		isHover = true;
	}

#endif

	return isHover;
}