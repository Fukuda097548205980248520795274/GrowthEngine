#include "../TitleScene.h"

/// @brief タイトルシーンのイントロ初期化処理
void TitleScene::IntroInitialize()
{
	// イントロの経過時間をリセット
	introTimer_ = kIntroDuration;

	// タイトルバーのスプライトのアルファ値を更新
	auto pushAnyButtonBG = uiEditor_->GetSprite("pushAnyButton_BG");
	pushAnyButtonBG->param_->material.color.w = 0.0f;

	// タイトルのスプライトのアルファ値を更新
	auto pushAnyButton = uiEditor_->GetSprite("pushAnyButton");
	pushAnyButton->param_->material.color.w = 0.0f;

	// タイトルロゴのスプライトのアルファ値を更新
	auto titleLogo = uiEditor_->GetSprite("TitleLogo");
	titleLogo->param_->material.color.w = 0.0f;
}

/// @brief タイトルシーンのイントロ更新処理
void TitleScene::IntroUpdate()
{
	float t = std::clamp(1.0f - (introTimer_ / kIntroDuration), 0.0f, 1.0f); // 経過時間の割合を計算
	fadeSprite_->param_->material.color.w = 1.0f - t; // フェードアウトのアルファ値を設定

	// BGMの音量を設定
	titleBgm_->param_->volume = t * kBgmMaxVolume;

	// 前のシーンがゲームシーンでない場合、タイトルバーとタイトルロゴのアルファ値を更新
	if (!(sceneManager_->GetPrevSceneName() == "Game"))
	{
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

	if (introTimer_ <= 0.0f)
	{
		// イントロが終了したらタイトルシーンの次のフェーズに遷移
		if (sceneManager_->GetPrevSceneName() == "Game")
		{
			phaseManager_->ChangePhase(PhaseType::StageSelect);
		}
		else
		{
			phaseManager_->ChangePhase(PhaseType::Title);
		}
	}
	else
	{
		// イントロの経過時間を減少
		introTimer_ -= 1.0f / 60.0f;
		introTimer_ = std::max(introTimer_, 0.0f); // 0未満にならないようにする
	}
}
