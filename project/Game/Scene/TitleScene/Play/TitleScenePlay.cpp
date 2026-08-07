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
	float t = std::clamp(1.0f - (playTimer_ / kPlayDuration), 0.0f, 1.0f); // 経過時間の割合を計算
	fadeSprite_->param_->material.color.w = t; // フェードインのアルファ値を設定

	// BGMの音量を設定
	titleBgm_->param_->volume = (1.0f - t) * kBgmMaxVolume;

	// プレイが終了したらゲームシーンに遷移
	if (playTimer_ <= 0.0f)
	{
		Transition("Game", nextStageName_);
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