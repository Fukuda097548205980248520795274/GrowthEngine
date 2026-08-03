#include "../TitleScene.h"

/// @brief タイトルシーンのメインメニュー初期化処理
void TitleScene::MainMenuInitialize()
{
	// メインメニューの選択肢を初期化
	mainManuOption_ = MainMenuOption::StartGame;
}

/// @brief タイトルシーンのメインメニュー更新処理
void TitleScene::MainMenuUpdate()
{
	// 上下のキー入力を処理する
	SelectMainMenuOption();

	// 決定キー入力を処理する
	ExecuteMainMenuOption();
}

/// @brief タイトルシーンのメインメニュー描画処理
void TitleScene::MainMenuDraw()
{

}

/// @brief メインメニューの選択肢を処理する
void TitleScene::SelectMainMenuOption()
{
	int moveDirection = 0;

	if (wKey_->IsInput() || upLeftStick_->IsInput())
	{
		moveDirection = -1;
	}
	else if (sKey_->IsInput() || downLeftStick_->IsInput())
	{
		moveDirection = 1;
	}

	// 選択肢の移動方向がない場合は処理を終了する
	if (moveDirection == 0)
		return;

	// メインメニューの選択肢を更新する
	if(static_cast<int>(mainManuOption_) + moveDirection >= 0 &&
		static_cast<int>(mainManuOption_) + moveDirection < static_cast<int>(MainMenuOption::MaxOption))
	{
		mainManuOption_ = static_cast<MainMenuOption>(static_cast<int>(mainManuOption_) + moveDirection);
	}
}

/// @brief メインメニューの選択肢を実行する
void TitleScene::ExecuteMainMenuOption()
{
	if (spaceKey_->IsInput() || aButton_->IsInput())
	{
		switch (mainManuOption_)
		{
			// ゲームを開始する処理
		case MainMenuOption::StartGame:

			// チュートリアルをクリアしているかどうかでフェーズを変更する
			if (sceneManager_->IsTutorialCleared())
			{
				phaseManager_->ChangePhase(PhaseType::StageSelect);
			}
			else
			{
				nextStageName_ = stageSelectEditor_->GetTutorialStageName();
				phaseManager_->ChangePhase(PhaseType::Play);
			}

			break;


			// ゲームを終了する処理
		case MainMenuOption::QuitGame:

			phaseManager_->ChangePhase(PhaseType::Quit);

			break;
		}
	}
}