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
	float t = 1.0f - (playTimer_ / kPlayDuration); // 経過時間の割合を計算
	fadeSprite_->param_->material.color.w = t; // フェードインのアルファ値を設定

	// プレイが終了したらゲームシーンに遷移
	if (playTimer_ <= 0.0f)
	{
		Transition("Game");
	}
	else
	{
		// プレイの経過時間を減少
		playTimer_ -= engine_->GetDeltaTime();
		playTimer_ = std::max(playTimer_, 0.0f); // 0未満にならないようにする
	}
}

/// @brief タイトルシーンのプレイ描画処理
void TitleScene::PlayDraw()
{

}