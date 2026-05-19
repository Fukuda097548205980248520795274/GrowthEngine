#include "PostEffectManager.h"
#include "Entity/Character/Player/Player.h"

/// @brief 初期化
void PostEffectManager::Initialize()
{
	// スタイルチェンジ中のグレースケールエフェクトの生成と初期化
	styleChangeGrayscaleEffect_ = std::make_unique<PostEffectGrayscale>("StyleChange_Grayscale");
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

				styleChangeGrayscaleEffect_->param_->intensity = Lerp(0.0f, kStyleChangeGrayscaleIntensity, easing);
			}
			else if (t >= 0.25f && t < 0.75f)
			{
				float tNormalized = (t - 0.25f) / 0.5f; // 0.0fから1.0fの範囲に正規化
				float easing = 1.0f - std::powf(1.0f - tNormalized, 3.0f); // イージング関数を適用して強さを調整
				styleChangeGrayscaleEffect_->param_->intensity = Lerp(kStyleChangeGrayscaleIntensity, 0.0f, easing);
			} 

			switch (player->GetNextStyle())
			{
			// 旋嵐スタイル
			case FightStyle::Tempest:
				styleChangeGrayscaleEffect_->param_->tint = kTempestStyleColor;
				break;

			// 撃鉄スタイル
			case FightStyle::Hammer:
				styleChangeGrayscaleEffect_->param_->tint = kHammerStyleColor;
				break;
			}

			// スタイルチェンジ中のグレースケールエフェクトを描画する
			styleChangeGrayscaleEffect_->Draw();
		}
	}
}