#include "../TitleScene.h"

/// @brief タイトルシーンのプレイ初期化処理
void TitleScene::PlayInitialize()
{
	// プレイの経過時間をリセット
	playTimer_ = kPlayDuration;
}

/// @brief タイトルシーンのプレイ更新処理
void TitleScene::PlayUpdate()
{
	// プレイが終了したらゲームシーンに遷移
	if (playTimer_ <= 0.0f)
	{
		Transition("Game");
	}
	else
	{
		// プレイの経過時間を減少
		playTimer_ -= engine_->GetDeltaTime();
	}
}

/// @brief タイトルシーンのプレイ描画処理
void TitleScene::PlayDraw()
{

}