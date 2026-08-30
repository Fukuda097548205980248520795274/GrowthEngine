#pragma once
#include "GrowthEngine.h"

class Player;

class PostEffectManager
{
public:

	/// @brief コンストラクタ
	PostEffectManager() = default;

	/// @brief デストラクタ
	~PostEffectManager() = default;

	/// @brief 初期化
	void Initialize();

	/// @brief 描画処理
	/// @param player 
	void Draw(Player* player);


private:

	/// @brief エンジンのインスタンス
	const GrowthEngine* engine_ = GrowthEngine::GetInstance();


	/*---------------
	    通常画面
	---------------*/

	/// @brief 通常画面内のヴィネッティング
	std::unique_ptr<PostEffectVignetting> normalVignetting_ = nullptr;

	/// @brief 通常画面内のブルーム
	std::unique_ptr<PostEffectBloom> normalBloom_ = nullptr;


	/*--------------
	    ダメージ
	--------------*/

	/// @brief ダメージ時のホワイトノイズ
	std::unique_ptr<PostEffectWhiteNoise> damageWhiteNoise_ = nullptr;

	/// @brief ダメージ時のガウシアンフィルター
	std::unique_ptr<PostEffectGaussianFilter> damageGaussianFilter_ = nullptr;

	// ダメージエフェクトの持続時間
	static constexpr float kDamageWhiteNoiseDuration = 0.5f;

	// ダメージエフェクトのタイマー
	float damageWhiteNoiseTimer_ = 0.0f;

	// ダメージエフェクトの持続時間
	static constexpr float kDamageGaussianFilterDuration = 2.0f;

	// ダメージエフェクトのタイマー
	float damageGaussianFilterTimer_ = 0.0f;


	/*----------------
	    チャージ攻撃
	----------------*/

	// チャージ攻撃エフェクトのグレースケール
	std::unique_ptr<PostEffectGrayscale> chargeAttackGrayscale_ = nullptr;

	// チャージ攻撃エフェクトのタイマー
	float chargeAttackTimer_ = 0.0f;

	// チャージ攻撃エフェクトの持続時間
	static constexpr float kChargeAttackDuration = 0.5f;


	/*--------------------
	    スタイルチェンジ
	---------------------*/


	/// @brief スタイルチェンジ中のグレースケール
	std::unique_ptr<PostEffectGrayscale> styleChangeGrayscale_ = nullptr;

	// スタイルチェンジ中のグレースケールエフェクトの強さ
	static constexpr float kStyleChangeGrayscaleIntensity = 0.15f;

	/// @brief 旋嵐スタイルのグレースケールカラー
	static constexpr Vector3 kTempestStyleGrayscaleColor = Vector3(0.7f, 0.7f, 1.0f);

	/// @brief 撃鉄スタイルのグレースケールカラー
	static constexpr Vector3 kHammerStyleGrayscaleColor = Vector3(1.0f, 0.7f, 0.2f);



	/// @brief スタイルチェンジ中の放射状ブラー
	std::unique_ptr<PostEffectRadialBlur> styleChangeRadialBlur_ = nullptr;

	// スタイルチェンジ中の放射状ブラーの強さ
	static constexpr float kStyleChangeRadialBlurPower = 0.1f;


	/// @brief スタイルチェンジ中のビネット
	std::unique_ptr<PostEffectVignetting> styleChangeVignetting_ = nullptr;

	// スタイルチェンジ中のビネットの強さ
	static constexpr float kStyleChangeVignettingIntensity = 0.1f;

	/// @brief 旋嵐スタイルのビネットカラー
	static constexpr Vector3 kTempestStyleVignettingColor = Vector3(0.0f, 0.0f, 0.5f);

	/// @brief 撃鉄スタイルのビネットカラー
	static constexpr Vector3 kHammerStyleVignettingColor = Vector3(0.3f, 0.3f, 0.0f);
};

