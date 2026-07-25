#include "../TitleScene.h"

/// @brief タイトルシーンの終了初期化処理
void TitleScene::QuitInitialize()
{
	// 終了の経過時間をリセット
	quitTimer_ = kQuitDuration;
}

/// @brief タイトルシーンの終了更新処理
void TitleScene::QuitUpdate()
{
	if (quitTimer_ <= 0.0f)
	{
		// 終了が終了したらアプリケーションを終了
		engine_->CloseWindow();
	}
	else
	{
		// 終了の経過時間を減少
		quitTimer_ -= engine_->GetDeltaTime();
	}
}

/// @brief タイトルシーンの終了描画処理
void TitleScene::QuitDraw()
{

}