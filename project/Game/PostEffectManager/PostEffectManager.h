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


	/*--------------------
	    スタイルチェンジ
	---------------------*/

	/// @brief スタイルチェンジ中のグレースケールエフェクト
	std::unique_ptr<PostEffectGrayscale> styleChangeGrayscaleEffect_ = nullptr;

	// スタイルチェンジ中のグレースケールエフェクトの強さ
	static constexpr float kStyleChangeGrayscaleIntensity = 0.3f;

	/// @brief 旋嵐スタイルのエフェクトカラー
	static constexpr Vector3 kTempestStyleColor = Vector3(0.7f, 0.7f, 1.0f);

	/// @brief 撃鉄スタイルのエフェクトカラー
	static constexpr Vector3 kHammerStyleColor = Vector3(1.0f, 0.7f, 0.2f);
};

