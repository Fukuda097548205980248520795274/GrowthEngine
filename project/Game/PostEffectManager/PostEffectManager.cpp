#include "PostEffectManager.h"
#include "Entity/Character/Player/Player.h"

/// @brief 初期化
void PostEffectManager::Initialize()
{
	// スタイルチェンジ中のグレースケールの生成と初期化
	styleChangeGrayscale_ = std::make_unique<PostEffectGrayscale>("StyleChange_Grayscale");

	// スタイルチェンジ中の放射状ブラーの生成と初期化
	styleChangeRadialBlur_ = std::make_unique<PostEffectRadialBlur>("StyleChange_RadialBlur");
	styleChangeRadialBlur_->param_->samples = 8;

	// スタイルチェンジ中のビネットの生成と初期化
	styleChangeVignetting_ = std::make_unique<PostEffectVignetting>("StyleChange_Vignetting");
}

/// @brief 描画処理
/// @param player 
void PostEffectManager::Draw(Player* player)
{
	// プレイヤーによる描画処理の後にポストエフェクトを描画する
	if (player)
	{
		// スタイルチェンジ中のポストエフェクト
		if (player->IsStyleChanging())
		{
			float timer = player->GetStyleChangeDuration() - player->GetStyleChangeTimer();
			float duration = player->GetStyleChangeDuration();
			float t = timer / duration;

			if (t < 0.25f)
			{
				float tNormalized = t / 0.25f; // 0.0fから1.0fの範囲に正規化
				float easing = 1.0f - std::powf(1.0f - tNormalized, 3.0f); // イージング関数を適用して強さを調整

				styleChangeGrayscale_->param_->intensity = Lerp(0.0f, kStyleChangeGrayscaleIntensity, easing);
				styleChangeVignetting_->param_->intensity = Lerp(0.0f, kStyleChangeVignettingIntensity, easing);
			}
			else if (t >= 0.25f && t < 0.95f)
			{
				float tNormalized = (t - 0.25f) / 0.5f; // 0.0fから1.0fの範囲に正規化
				float easing = 1.0f - std::powf(1.0f - tNormalized, 2.0f); // イージング関数を適用して強さを調整

				styleChangeGrayscale_->param_->intensity = Lerp(kStyleChangeGrayscaleIntensity, 0.0f, easing);
			}

			if (t < 0.75f)
			{
				float tNormalized = t / 0.75f; // 0.0fから1.0fの範囲に正規化
				float easing = 1.0f - std::powf(1.0f - tNormalized, 4.0f); // イージング関数を適用して強さを調整

				styleChangeRadialBlur_->param_->power = Lerp(kStyleChangeRadialBlurPower, 0.0f, easing);
			}

			if (t < 0.3f)
			{
				float tNormalized = t / 0.3f; // 0.0fから1.0fの範囲に正規化
				float easing = 1.0f - std::powf(1.0f - tNormalized, 4.0f); // イージング関数を適用して強さを調整

				styleChangeVignetting_->param_->intensity = Lerp(kStyleChangeVignettingIntensity, 0.0f, easing);
			}

			// スタイルに応じたエフェクトの色を設定
			switch (player->GetNextStyle())
			{
			// 旋嵐スタイル
			case FightStyle::Tempest:
				styleChangeGrayscale_->param_->tint = kTempestStyleGrayscaleColor;
				styleChangeVignetting_->param_->color = kTempestStyleVignettingColor;
				break;

			// 撃鉄スタイル
			case FightStyle::Hammer:
				styleChangeGrayscale_->param_->tint = kHammerStyleGrayscaleColor;
				styleChangeVignetting_->param_->color = kHammerStyleVignettingColor;
				break;
			}


			// 放射状ブラーの描画
			if (t < 0.75f)
				styleChangeRadialBlur_->Draw();

			// グレースケールの描画
			styleChangeGrayscale_->Draw();

			// ビネットの描画
			styleChangeVignetting_->Draw();
		}
	}
}