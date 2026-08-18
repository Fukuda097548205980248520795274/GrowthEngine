#include "../TitleScene.h"
#include <numbers>

/// @brief タイトルシーンのメインメニュー初期化処理
void TitleScene::MainMenuInitialize()
{
	// メインメニューの選択肢を初期化
	mainManuOption_ = MainMenuOption::StartGame;

	// メインメニューの選択肢が実行されたかどうかを初期化
	isMainMenuOptionExecuted_ = false;

	// ゲーム終了が実行されたかどうかを初期化
	isBackToTitle_ = false;

	// メインメニューのタイマーを初期化
	mainMenuSpriteParamAlpha_ = 0.0f;


	// メインメニューのスプライトの拡縮を初期化
	for (int i = 0; i < static_cast<int>(MainMenuOption::MaxOption); ++i)
	{
		mainMenuSpriteBG_[i]->param_->transform.scale = Vector2(0.75f, 0.75f);
	}
}

/// @brief タイトルシーンのメインメニュー更新処理
void TitleScene::MainMenuUpdate()
{
	float dt = engine_->GetDeltaTime();

	// メインメニューのスプライトのアルファ値を更新
	mainMenuSpriteParamAlpha_ += 0.05f;
	mainMenuSpriteParamAlpha_ = std::fmod(mainMenuSpriteParamAlpha_, std::numbers::pi_v<float>);

	if (isMainMenuOptionExecuted_)
	{
		// メインメニューのタイマーを更新
		mainMenuTimer_ -= dt;

		// メインメニューのタイマーが0以下になったら選択肢の処理を実行
		if (mainMenuTimer_ <= 0.0f)
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
				isQuitExecuted_ = true;

				break;
			}
		}
	}
	else if (isBackToTitle_)
	{
		// メインメニューのタイマーを更新
		mainMenuTimer_ -= dt;

		// メインメニューのタイマーが0以下になったらタイトルに戻る
		if (mainMenuTimer_ <= 0.0f)
		{
			phaseManager_->ChangePhase(PhaseType::Title);
		}
	}
	else
	{
		// 上下のキー入力を処理する
		SelectMainMenuOption();

		// 決定キー入力を処理する
		ExecuteMainMenuOption();

		// タイトルに戻るキー入力を処理する
		BackToTitle();
	}

	// メインメニューの選択肢のスプライトを更新する
	UpdateMainMenuOptionSprite();
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

		// 選択SEを再生
		selectSe_->Play();
	}
}

/// @brief メインメニューの選択肢を実行する
void TitleScene::ExecuteMainMenuOption()
{
	if (spaceKey_->IsInput() || aButton_->IsInput())
	{
		// メインメニューの選択肢が実行されたことをフラグに設定
		isMainMenuOptionExecuted_ = true;

		// メインメニューのタイマーをリセット
		mainMenuTimer_ = kMainMenuDuration;

		// 決定SEを再生
		executeSe_->Play();
	}
}

/// @brief タイトルに戻る
void TitleScene::BackToTitle()
{
	if (escapeKey_->IsInput() || bButton_->IsInput())
	{
		// メインメニューのタイマーをリセット
		mainMenuTimer_ = kMainMenuDuration;

		// タイトルに戻ることをフラグに設定
		isBackToTitle_ = true;

		// 戻るSEを再生
		backSe_->Play();
	}
}

/// @brief メインメニューの選択肢のスプライトを更新する
void TitleScene::UpdateMainMenuOptionSprite()
{
	// メインメニューのスプライトのアルファ値を更新
	float alpha = 1.0f - std::sin(mainMenuSpriteParamAlpha_) * 0.6f;

	if (isMainMenuOptionExecuted_)
	{
		float t = 1.0f - (mainMenuTimer_ / kMainMenuDuration);
		float easing = 1.0f - std::pow(1.0f - t, 3.0f);

		for (int i = 0; i < static_cast<int>(MainMenuOption::MaxOption); ++i)
		{
			if (i == static_cast<int>(mainManuOption_))
			{
				mainMenuSpriteBG_[i]->param_->material.color.w = Lerp(1.0f, 0.0f, easing);
				mainMenuSpriteBG_[i]->param_->transform.scale = Lerp(Vector2(0.75f, 0.75f), Vector2(1.5f, 1.5f), easing);
				mainMenuSprite_[i]->param_->material.color = Lerp(mainMenuSprite_[i]->param_->material.color, Vector4(0.0f, 0.0f, 0.0f, 0.0f), easing);
			}
			else
			{
				mainMenuSpriteBG_[i]->param_->material.color.w = Lerp(mainMenuSpriteBG_[i]->param_->material.color.w, 0.0f, t);
				mainMenuSprite_[i]->param_->material.color = Lerp(mainMenuSprite_[i]->param_->material.color, Vector4(1.0f, 1.0f, 1.0f, 0.0f), t);
			}
		}
	}
	else if (isBackToTitle_)
	{
		float t = 1.0f - (mainMenuTimer_ / kMainMenuDuration);

		// メインメニューのスプライトのアルファ値を更新
		for (int i = 0; i < static_cast<int>(MainMenuOption::MaxOption); ++i)
		{
			mainMenuSpriteBG_[i]->param_->material.color.w = Lerp(mainMenuSpriteBG_[i]->param_->material.color.w, 0.0f, t);
			mainMenuSprite_[i]->param_->material.color = Lerp(mainMenuSprite_[i]->param_->material.color, Vector4(1.0f, 1.0f, 1.0f, 0.0f), t);
		}

		// タイトルバーのスプライトのアルファ値を更新
		auto pushAnyButtonBG = uiEditor_->GetSprite("pushAnyButton_BG");
		pushAnyButtonBG->param_->material.color.w = t;

		// タイトルのスプライトのアルファ値を更新
		auto pushAnyButton = uiEditor_->GetSprite("pushAnyButton");
		pushAnyButton->param_->material.color.w = t;

		// タイトルロゴのスプライトのアルファ値を更新
		auto titleLogo = uiEditor_->GetSprite("TitleLogo");
		titleLogo->param_->material.color.w = t;
	}
	else
	{
		for (int i = 0; i < static_cast<int>(MainMenuOption::MaxOption); ++i)
		{
			if (i == static_cast<int>(mainManuOption_))
			{
				mainMenuSpriteBG_[i]->param_->material.color.w = Lerp(mainMenuSpriteBG_[i]->param_->material.color.w, alpha, 0.3f);
				mainMenuSprite_[i]->param_->material.color = Lerp(mainMenuSprite_[i]->param_->material.color, Vector4(0.0f, 0.0f, 0.0f, 1.0f), 0.3f);
			}
			else
			{
				mainMenuSpriteBG_[i]->param_->material.color.w = Lerp(mainMenuSpriteBG_[i]->param_->material.color.w, 0.0f, 0.3f);
				mainMenuSprite_[i]->param_->material.color = Lerp(mainMenuSprite_[i]->param_->material.color, Vector4(1.0f, 1.0f, 1.0f, 1.0f), 0.3f);
			}
		}
	}

	// フェーズビューのスプライトのアルファ値を更新
	if (!isQuitExecuted_ && !isBackToTitle_)
	{
		auto phaseView = uiEditor_->GetSprite("Phase_View");
		phaseView->param_->material.color.w = Lerp(phaseView->param_->material.color.w, 1.0f, 0.3f);
	}
	else
	{
		auto phaseView = uiEditor_->GetSprite("Phase_View");
		phaseView->param_->material.color.w = Lerp(phaseView->param_->material.color.w, 0.0f, 0.3f);
	}
}