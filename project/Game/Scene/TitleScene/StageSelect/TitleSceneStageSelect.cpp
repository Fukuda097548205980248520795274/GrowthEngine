#include "../TitleScene.h"

/// @brief タイトルシーンのメインメニュー初期化処理
void TitleScene::StageSelectInitialize()
{
	// ステージ選択の処理が実行されたかどうかを初期化
	isStageSelectExecuted_ = false;

	// メインメニューに戻る処理が実行されたかどうかを初期化
	isBackToMainMenu_ = false;
}

/// @brief タイトルシーンのメインメニュー更新処理
void TitleScene::StageSelectUpdate()
{
	float dt = engine_->GetDeltaTime();

	if (isStageSelectExecuted_)
	{
		stageSelectTimer_ -= dt;

		// ステージセレクトのタイマーが0以下になったら選択肢の処理を実行
		if (stageSelectTimer_ <= 0.0f)
		{
			nextStageName_ = stageSelectEditor_->GetStageList()[stageSelectIndex_].fileName;
			phaseManager_->ChangePhase(PhaseType::Play);
		}
	}
	else if (isBackToMainMenu_)
	{
		stageSelectTimer_ -= dt;

		// ステージセレクトのタイマーが0以下になったらタイトルに戻る
		if (stageSelectTimer_ <= 0.0f)
		{
			phaseManager_->ChangePhase(PhaseType::MainMenu);
		}
	}
	else
	{
		// 上下のキー入力を処理する
		StageSelectMove();

		// 決定キー入力を処理する
		StageSelectExecute();

		// タイトルに戻るキー入力を処理する
		BackToMainMenu();
	}
}

/// @brief タイトルシーンのメインメニュー描画処理
void TitleScene::StageSelectDraw()
{

}

/// @brief ステージセレクトの選択肢を処理する
void TitleScene::StageSelectMove()
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

	// ステージセレクトの選択肢を更新する
	auto stageList = stageSelectEditor_->GetStageList();

	if (static_cast<int>(stageSelectIndex_) + moveDirection >= 0 &&
		static_cast<int>(stageSelectIndex_) + moveDirection < static_cast<int>(stageList.size()))
	{
		stageSelectIndex_ = static_cast<int>(stageSelectIndex_) + moveDirection;

		// 選択SEを再生
		selectSe_->Play();
	}
}

/// @brief ステージセレクトの選択肢を実行する
void TitleScene::StageSelectExecute()
{
	if (spaceKey_->IsInput() || aButton_->IsInput())
	{
		// ステージセレクトタイマーをリセット
		stageSelectTimer_ = kStageSelectDuration;

		// ステージセレクトの選択肢が実行されたことをフラグに設定
		isStageSelectExecuted_ = true;

		// 決定SEを再生
		executeSe_->Play();
	}
}

/// @brief タイトルに戻る
void TitleScene::BackToMainMenu()
{
	if (escapeKey_->IsInput() || bButton_->IsInput())
	{
		// ステージセレクトタイマーをリセット
		stageSelectTimer_ = kStageSelectDuration;

		// タイトルに戻ることをフラグに設定
		isBackToMainMenu_ = true;

		// 戻るSEを再生
		backSe_->Play();
	}
}