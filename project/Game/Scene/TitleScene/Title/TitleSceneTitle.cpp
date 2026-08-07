#include "../TitleScene.h"
#include <numbers>

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
		titleTimer_ = std::max(titleTimer_, 0.0f);

		// タイトルバーのスプライトのアルファ値を更新
		auto pushAnyButtonBG = uiEditor_->GetSprite("pushAnyButton_BG");
		pushAnyButtonBG->param_->material.color.w = titleTimer_ / kTitleDuration;

		// タイトルのスプライトのアルファ値を更新
		auto pushAnyButton = uiEditor_->GetSprite("pushAnyButton");
		pushAnyButton->param_->material.color.w = titleTimer_ / kTitleDuration;

		// タイトルロゴのスプライトのアルファ値を更新
		auto titleLogo = uiEditor_->GetSprite("TitleLogo");
		titleLogo->param_->material.color.w = titleTimer_ / kTitleDuration;

		// タイトルのタイマーが0以下になったらメインメニューに遷移
		if (titleTimer_ <= 0.0f)
		{
			phaseManager_->ChangePhase(PhaseType::MainMenu);
		}
	}
	else
	{
		// タイトルバーのスプライトのアルファ値を更新
		titleSpriteParamAlpha_ += 0.05f;
		titleSpriteParamAlpha_ = std::fmod(titleSpriteParamAlpha_, std::numbers::pi_v<float>);
		auto pushAnyButtonBG = uiEditor_->GetSprite("pushAnyButton_BG");
		pushAnyButtonBG->param_->material.color.w = 1.0f - std::sin(titleSpriteParamAlpha_) * 0.6f;

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

		// タイトルバーのスプライトのアルファ値をリセット
		auto pushAnyButtonBG = uiEditor_->GetSprite("pushAnyButton_BG");
		pushAnyButtonBG->param_->material.color.w = 1.0f;

		// タイトルのスプライトのアルファ値をリセット
		auto pushAnyButton = uiEditor_->GetSprite("pushAnyButton");
		pushAnyButton->param_->material.color.w = 1.0f;

		// タイトルロゴ
		auto titleLogo = uiEditor_->GetSprite("TitleLogo");
		titleLogo->param_->material.color.w = 1.0f;
	}
}