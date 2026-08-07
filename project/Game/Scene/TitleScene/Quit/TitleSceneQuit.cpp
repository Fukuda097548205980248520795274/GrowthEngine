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
	float t = std::clamp(1.0f - (quitTimer_ / kQuitDuration), 0.0f, 1.0f); // 経過時間の割合を計算
	fadeSprite_->param_->material.color.w = t; // フェードインのアルファ値を設定

	// BGMの音量を設定
	titleBgm_->param_->volume = (1.0f - t) * kBgmMaxVolume;

	if (quitTimer_ <= 0.0f)
	{
		// 終了が終了したらアプリケーションを終了
		engine_->CloseWindow();
	}
	else
	{
		// 終了の経過時間を減少
		quitTimer_ -= engine_->GetDeltaTime();
		quitTimer_ = std::max(quitTimer_, 0.0f); // 0未満にならないようにする
	}
}

/// @brief タイトルシーンの終了描画処理
void TitleScene::QuitDraw()
{

}