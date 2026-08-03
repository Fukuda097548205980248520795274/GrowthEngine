#include "../TitleScene.h"

/// @brief タイトルシーンのメインメニュー初期化処理
void TitleScene::StageSelectInitialize()
{

}

/// @brief タイトルシーンのメインメニュー更新処理
void TitleScene::StageSelectUpdate()
{
	// 上下のキー入力を処理する
	StageSelectMove();

	// 決定キー入力を処理する
	StageSelectExecute();
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
		static_cast<int>(stageSelectIndex_) + moveDirection < static_cast<int>(stageList.size() - 1))
	{
		stageSelectIndex_ = static_cast<int>(stageSelectIndex_) + moveDirection;
	}
}

/// @brief ステージセレクトの選択肢を実行する
void TitleScene::StageSelectExecute()
{
	if (spaceKey_->IsInput() || aButton_->IsInput())
	{
		nextStageName_ = stageSelectEditor_->GetStageList()[stageSelectIndex_].fileName;
		phaseManager_->ChangePhase(PhaseType::Play);
	}
}