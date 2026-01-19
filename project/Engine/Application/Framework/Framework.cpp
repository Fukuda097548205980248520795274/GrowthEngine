#include "Framework.h"

/// @brief コンストラクタ
/// @param screenWidth スクリーン横幅
/// @param screenHeight スクリーン縦幅
/// @param title タイトル
Framework::Framework(int32_t screenWidth, int32_t screenHeight, const std::string& title)
{
	// エンジンの生成と初期化
	GrowthEngine::GetInstance(screenWidth, screenHeight, title);
}

/// @brief 実行
/// @return 
int32_t Framework::Run()
{
	// エンジンを取得
	GrowthEngine* engine = GrowthEngine::GetInstance();

	// 初期化
	Initialize();

	// ゲームループ
	while (engine->GameLoop())
	{
		// 描画前処理
		engine->PreDraw();

		// 更新処理
		Update();

		// 描画処理
		Draw();

		// 描画後処理
		engine->PostDraw();
	}

	return 0;
}