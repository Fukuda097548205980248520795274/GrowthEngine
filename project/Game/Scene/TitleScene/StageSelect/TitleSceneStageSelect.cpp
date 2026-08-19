#include "../TitleScene.h"
#include <numbers>

/// @brief タイトルシーンのメインメニュー初期化処理
void TitleScene::StageSelectInitialize()
{
	// ステージ選択の処理が実行されたかどうかを初期化
	isStageSelectExecuted_ = false;

	// メインメニューに戻る処理が実行されたかどうかを初期化
	isBackToMainMenu_ = false;

	stageSelectSpriteParamAlpha_ = 0.0f;

	for (int i = 0; i < stageSelectOptionSprites_.size(); ++i)
	{
		// ステージセレクトの選択肢のスプライトの目標位置を計算
		Vector2 targetPosition = kStageSelectOptionSpritePosition;

		// 選択肢のインデックスに応じてY座標を調整
		targetPosition.y -= static_cast<float>(i - stageSelectIndex_) * kStageSelectOptionSpriteSpacing;

		// ステージセレクトの選択肢のスプライトの現在位置を取得
		auto& currentPos = stageSelectOptionSprites_[i]->param_.transform.translate;
		currentPos = targetPosition;

		// ステージセレクトの選択肢のスプライトのアルファ値を更新
		stageSelectOptionSprites_[i]->param_.material.color.w = 0.0f;

		// ステージセレクトの選択肢のスプライトを描画
		stageSelectOptionSprites_[i]->Draw();
	}
}

/// @brief タイトルシーンのメインメニュー更新処理
void TitleScene::StageSelectUpdate()
{
	float dt = engine_->GetDeltaTime();

	// ステージセレクトのスプライトのアルファ値を更新
	stageSelectSpriteParamAlpha_ += 0.05f;
	stageSelectSpriteParamAlpha_ = std::fmod(stageSelectSpriteParamAlpha_, std::numbers::pi_v<float>);

	// ステージセレクトの背景スプライトを更新
	StageSelectBgSpriteUpdate();

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

/// @brief ステージセレクトの背景スプライトを更新する
void TitleScene::StageSelectBgSpriteUpdate()
{
	float dt = engine_->GetDeltaTime();

	// フェーズビューのスプライトのアルファ値を更新
	if (!isStageSelectExecuted_ && !isBackToMainMenu_)
	{
		auto phaseView = uiEditor_->GetSprite("Phase_View");
		phaseView->param_->material.color.w = Lerp(phaseView->param_->material.color.w, 1.0f, 0.3f);
	}

	for (int i = 0; i < stageSelectOptionSprites_.size(); ++i)
	{
		// ステージセレクトの選択肢のスプライトの目標位置を計算
		Vector2 targetPosition = kStageSelectOptionSpritePosition;

		// 選択肢のインデックスに応じてY座標を調整
		targetPosition.y -= static_cast<float>(i - stageSelectIndex_) * kStageSelectOptionSpriteSpacing;

		// ステージセレクトの選択肢のスプライトの現在位置を取得
		auto& currentPos = stageSelectOptionSprites_[i]->param_.transform.translate;
		currentPos.x = Lerp(currentPos.x, targetPosition.x, dt * 15.0f);
		currentPos.y = Lerp(currentPos.y, targetPosition.y, dt * 15.0f);

		// 現在のアルファ値とスケールの参照を取得
		auto& currentAlpha = stageSelectOptionSprites_[i]->param_.material.color.w;
		auto& currentScale = stageSelectOptionSprites_[i]->param_.transform.scale;

		// 決定時の演出
		if (isStageSelectExecuted_)
		{
			// タイマーから進行度(0.0 ~ 1.0)とイージングを計算
			float t = 1.0f - (stageSelectTimer_ / kStageSelectDuration);
			float easing = 1.0f - std::pow(1.0f - t, 3.0f);

			if (i == stageSelectIndex_)
			{
				// 選択された項目は大きくしながらフェードアウト
				currentAlpha = Lerp(1.0f, 0.0f, easing);
				currentScale = Lerp(Vector2(0.75f, 0.75f), Vector2(1.5f, 1.5f), easing);
			}
			else
			{
				// 選択されていない項目はそのままフェードアウト
				currentAlpha = Lerp(currentAlpha, 0.0f, t);
			}
		}
		// メインメニューに戻る時の演出
		else if (isBackToMainMenu_)
		{
			float t = 1.0f - (stageSelectTimer_ / kStageSelectDuration);
			currentAlpha = Lerp(currentAlpha, 0.0f, t);
		}
		// 通常時（選択中）の演出
		else
		{
			// 選択中の項目は点滅させる
			float blinkingAlpha = 1.0f - std::sin(stageSelectSpriteParamAlpha_) * 0.6f;

			// 目標アルファ値の決定
			float targetAlpha = 0.0f;

			if (i == stageSelectIndex_)
			{
				targetAlpha = blinkingAlpha;
			}
			else if (i >= stageSelectIndex_ - 1 && i <= stageSelectIndex_ + 1)
			{
				targetAlpha = 0.1f;
			}

			targetAlpha = std::clamp(targetAlpha, 0.0f, 1.0f);

			// アルファ値をLerpで滑らかに補間（フェードイン/フェードアウト）
			currentAlpha = Lerp(currentAlpha, targetAlpha, dt * 10.0f);

			// スケールは通常のサイズに維持・補間
			currentScale = Lerp(currentScale, Vector2(0.75f, 0.75f), dt * 15.0f);
		}

		// ステージセレクトの選択肢のスプライトを描画
		stageSelectOptionSprites_[i]->Draw();
	}

	if (isStageSelectExecuted_)
	{
		float t = 1.0f - (stageSelectTimer_ / kStageSelectDuration);
		t = std::clamp(t, 0.0f, 1.0f);

		auto phaseView = uiEditor_->GetSprite("Phase_View");
		if (phaseView)
		{
			phaseView->param_->material.color.w = Lerp(1.0f, 0.0f, t);
		}
	}

	if (isStageSelectExecuted_ || isBackToMainMenu_)
	{
		float t = 1.0f - (stageSelectTimer_ / kStageSelectDuration);
		t = std::clamp(t, 0.0f, 1.0f);

		auto stageSelectText = uiEditor_->GetSprite("stageSelect");
		if (stageSelectText)
		{
			stageSelectText->param_->material.color.w = Lerp(stageSelectText->param_->material.color.w, 0.0f, t);
		}
	}
	else
	{
		auto stageSelectText = uiEditor_->GetSprite("stageSelect");
		if (stageSelectText)
		{
			stageSelectText->param_->material.color.w = Lerp(stageSelectText->param_->material.color.w, 1.0f, dt * 10.0f);
		}
	}
}