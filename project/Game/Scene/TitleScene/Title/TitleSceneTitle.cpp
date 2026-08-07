#include "../TitleScene.h"

/// @brief タイトルシーンのタイトル初期化処理
void TitleScene::TitleInitialize()
{
	// タイトル実行フラグを初期化
	isTitleOptionExecuted_ = false;
}

/// @brief タイトルシーンのタイトル更新処理
void TitleScene::TitleUpdate()
{
	if (isTitleOptionExecuted_)
	{
		// タイトルのタイマーを更新
		titleTimer_ -= engine_->GetDeltaTime();

		// タイトルのタイマーが0以下になったらメインメニューに遷移
		if (titleTimer_ <= 0.0f)
		{
			phaseManager_->ChangePhase(PhaseType::MainMenu);
		}
	}
	else
	{
		// タイトルの選択肢を処理する
		ExecuteTitleOption();
	}
}

/// @brief タイトルシーンのタイトル描画処理
void TitleScene::TitleDraw()
{

}

/// @brief タイトルの選択肢を処理する
void TitleScene::ExecuteTitleOption()
{
	if ((spaceKey_->IsInput() || aButton_->IsInput()) && !isTitleOptionExecuted_)
	{
		isTitleOptionExecuted_ = true;

		// タイトルの選択肢が実行されたらSEを再生
		executeSe_->Play();

		// タイトルのタイマーをリセット
		titleTimer_ = kTitleDuration;
	}
}