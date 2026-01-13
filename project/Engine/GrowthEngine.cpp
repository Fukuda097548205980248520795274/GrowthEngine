#include "GrowthEngine.h"
#include <cassert>
#include "Log/Log.h"
#include "Func/CrushHandler/CrushHandler.h"

#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"Dbghelp.lib")
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

// インスタンス
std::unique_ptr<GrowthEngine> GrowthEngine::instance_ = nullptr;

/// @brief インスタンスを取得する
/// @param screenWidth スクリーン横幅
/// @param screenHeight スクリーン縦幅
/// @param title タイトル
/// @return 
GrowthEngine* GrowthEngine::GetInstance(int32_t screenWidth, int32_t screenHeight, const std::string& title)
{
	if (instance_ == nullptr)
	{
		instance_.reset(new GrowthEngine());
		instance_->Initialize(screenWidth, screenHeight, title);
	}

	return instance_.get();
}

/// @brief インスタンスを取得する
/// @return 
GrowthEngine* GrowthEngine::GetInstance()
{
	// インスタンスがないと失敗
	assert(instance_ != nullptr);
	return instance_.get();
}


/// @brief 初期化
/// @param screenWidth 
/// @param screenHeight 
/// @param title 
void GrowthEngine::Initialize(int32_t screenWidth, int32_t screenHeight, const std::string& title)
{
	// 例外が発生したときに起動する
	SetUnhandledExceptionFilter(Engine::ExportDump);

	// ログの生成
	log_ = std::make_unique<Engine::Log>();

	// ウィンドウアプリケーションの生成と初期化
	winApp_ = std::make_unique<Engine::WinApp>();
	winApp_->Initialize(screenWidth, screenHeight, title , log_.get());

	// 描画統括の生成と初期化
	renderContext_ = std::make_unique<Engine::RenderContext>();
	renderContext_->Initialize(winApp_.get(), log_.get());
}

/// @brief デストラクタ
GrowthEngine::~GrowthEngine()
{
	// 描画統括の終了
	renderContext_.reset();
	renderContext_ = nullptr;

	// ウィンドウアプリケーションの終了
	winApp_.reset();
	winApp_ = nullptr;

	// ログの終了
	log_.reset();
	log_ = nullptr;
}


/// @brief 描画前処理
void GrowthEngine::PreDraw() 
{
	renderContext_->PreDraw();
}

/// @brief 描画後処理
void GrowthEngine::PostDraw()
{
	renderContext_->PostDraw();
}