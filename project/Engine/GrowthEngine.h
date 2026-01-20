#pragma once

#include <cstdint>
#include <string>
#include <memory>
#include <Windows.h>

#include "Log/Log.h"
#include "WinApp/WinApp.h"
#include "RenderContext/RenderContext.h"

#include "Math/Vector/Vector3/Vector3.h"

#include "Application/Framework/Framework.h"

class GrowthEngine
{
public:

	/// @brief インスタンスを取得する
	/// @param screenWidth スクリーン横幅
	/// @param screenHeight スクリーン縦幅
	/// @param title タイトル
	/// @return 
	static GrowthEngine* GetInstance(int32_t screenWidth, int32_t screenHeight, const std::string& title);

	/// @brief インスタンスを取得する
	/// @return 
	static GrowthEngine* GetInstance();

	/// @brief デストラクタ
	~GrowthEngine();

	/// @brief ゲームループ
	/// @return 
	bool GameLoop() { return winApp_->ProcessMessage(); }

	/// @brief 描画前処理
	void PreDraw();

	/// @brief 描画後処理
	void PostDraw();

	/// @brief テクスチャを読み込む
	/// @param filePath 
	/// @return 
	TextureHandle LoadTexture(const std::string& filePath) {return renderContext_->LoadTexture(filePath, log_.get()); }


public:

	/// @brief 画面の横幅を取得する
	/// @return 
	int32_t GetScreenWidth()const { return winApp_->GetClientWidth(); }

	/// @brief 画面の縦幅を取得する
	/// @return 
	int32_t GetScreenHeight()const { return winApp_->GetClientHeight(); }



private:

	// 生成できないようにする
	GrowthEngine() = default;
	GrowthEngine(GrowthEngine&) = delete;
	GrowthEngine& operator=(GrowthEngine&) = delete;

	// インスタンス
	static std::unique_ptr<GrowthEngine> instance_;

	/// @brief 初期化
	/// @param screenWidth 
	/// @param screenHeight 
	/// @param title 
	void Initialize(int32_t screenWidth, int32_t screenHeight, const std::string& title);


private:

	// ログ
	std::unique_ptr<Engine::Log> log_ = nullptr;

	// ウィンドウアプリケーション
	std::unique_ptr<Engine::WinApp> winApp_ = nullptr;

	// 描画統括
	std::unique_ptr<Engine::RenderContext> renderContext_ = nullptr;


private:


#ifdef _DEVELOPMENT

	/// @brief メニューバー
	void MenuBer();

	/// @brief シーン追加
	void CreateScene();

	/// @brief シーンファイルを作成する
	/// @param fileName 
	void CreateSceneFile(const std::string& fileName);


	// シーン生成フラグ
	bool isSceneCreate_ = false;

#endif
};

