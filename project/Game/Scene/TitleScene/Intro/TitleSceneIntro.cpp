#include "../TitleScene.h"

/// @brief タイトルシーンのイントロ初期化処理
void TitleScene::IntroInitialize()
{
	// イントロの経過時間をリセット
	introTimer_ = kIntroDuration;
}

/// @brief タイトルシーンのイントロ更新処理
void TitleScene::IntroUpdate()
{
	float t = std::clamp(1.0f - (introTimer_ / kIntroDuration), 0.0f, 1.0f); // 経過時間の割合を計算
	fadeSprite_->param_->material.color.w = 1.0f - t; // フェードアウトのアルファ値を設定

	// BGMの音量を設定
	titleBgm_->param_->volume = t * kBgmMaxVolume;

	if (introTimer_ <= 0.0f)
	{
		// イントロが終了したらメインメニューに遷移
		phaseManager_->ChangePhase(PhaseType::Title);
	}
	else
	{
		// イントロの経過時間を減少
		introTimer_ -= engine_->GetDeltaTime();
		introTimer_ = std::max(introTimer_, 0.0f); // 0未満にならないようにする
	}
}

/// @brief タイトルシーンのイントロ描画処理
void TitleScene::IntroDraw()
{

}