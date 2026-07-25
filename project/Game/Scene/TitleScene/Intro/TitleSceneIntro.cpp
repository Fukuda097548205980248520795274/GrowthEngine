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
	if (introTimer_ <= 0.0f)
	{
		// イントロが終了したらメインメニューに遷移
		phaseManager_->ChangePhase(PhaseType::MainMenu);
	}
	else
	{
		// イントロの経過時間を減少
		introTimer_ -= engine_->GetDeltaTime();
	}
}

/// @brief タイトルシーンのイントロ描画処理
void TitleScene::IntroDraw()
{

}