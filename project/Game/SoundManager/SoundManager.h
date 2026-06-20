#pragma once
#include "GrowthEngine.h"

class SoundManager
{
public:

	/// @brief デストラクタ
	~SoundManager() = default;

	/// @brief インスタンスを取得する
	/// @return 
	static SoundManager* GetInstance();


private:

	/// @brief エンジンのインスタンス
	const GrowthEngine* engine_ = nullptr;


public:

	/// @brief 旋嵐スタイルチェンジのSE
	void SeStyleChangeSenran()const;

	/// @brief 撃鉄スタイルチェンジのSE
	void SeStyleChangeGekitetu()const;

	/// @brief 受け流しのSE
	void SeParried() { if (seParried_)seParried_->Play(); }

	/// @brief 弾きのSE
	void SeDeflect() { if (seDeflect_)seDeflect_->Play(); }

	/// @brief ガードのSE
	void SeGuard() { if (seGuard_)seGuard_->Play(); }

	/// @brief 回避のSE
	void SeAvoid() { if (seAvoid_)seAvoid_->Play(); }

	/// @brief 軽ダメージのSE
	void SeLightDamage() { if (seLightDamage_)seLightDamage_->Play(); }

	/// @brief 重ダメージのSE
	void SeHeavyDamage() { if (seHeavyDamage_)seHeavyDamage_->Play(); }

	/// @brief ガードブレイクのSE
	void SeGuardBreak() { if (seGuardBreak_)seGuardBreak_->Play(); }

	/// @brief 着地のSE
	void SeLanding() { if (seLanding_)seLanding_->Play(); }

	/// @brief ダウン中着地のSE
	void SeDownLanding() { if (seDownLanding_)seDownLanding_->Play(); }


	/// @brief 旋嵐スタイルのBGM
	std::unique_ptr<Bgm> bgmStyleSenran_ = nullptr;

	/// @brief 撃鉄スタイルのBGM
	std::unique_ptr<Bgm> bgmStyleGekitetu_ = nullptr;


private:


	/// @brief 旋嵐スタイルチェンジのSE
	std::unique_ptr<Se> seStyleSenran0_ = nullptr;
	std::unique_ptr<Se> seStyleSenran1_ = nullptr;
	std::unique_ptr<Se> seStyleSenran2_ = nullptr;
	std::unique_ptr<Se> seStyleSenran3_ = nullptr;
	std::unique_ptr<Se> seStyleSenran4_ = nullptr;

	/// @brief 撃鉄スタイルチェンジのSE
	std::unique_ptr<Se> seStyleGekitetu0_ = nullptr;
	std::unique_ptr<Se> seStyleGekitetu1_ = nullptr;
	std::unique_ptr<Se> seStyleGekitetu2_ = nullptr;
	std::unique_ptr<Se> seStyleGekitetu3_ = nullptr;

	/// @brief 受け流しのSE
	std::unique_ptr<Se> seParried_ = nullptr;

	/// @brief 弾きのSE
	std::unique_ptr<Se> seDeflect_ = nullptr;

	/// @brief ガードのSE
	std::unique_ptr<Se> seGuard_ = nullptr;

	/// @brief 回避のSE
	std::unique_ptr<Se> seAvoid_ = nullptr;

	/// @brief 軽ダメージのSE
	std::unique_ptr<Se> seLightDamage_ = nullptr;

	/// @brief 重ダメージのSE
	std::unique_ptr<Se> seHeavyDamage_ = nullptr;

	/// @brief ガードブレイクのSE
	std::unique_ptr<Se> seGuardBreak_ = nullptr;

	/// @brief 着地のSE
	std::unique_ptr<Se> seLanding_ = nullptr;

	/// @brief ダウン中着地のSE
	std::unique_ptr<Se> seDownLanding_ = nullptr;


private:

	/// @brief インスタンス
	static std::unique_ptr<SoundManager> instance_;

	/// @brief 初期化
	void Initialize();

	// コピー禁止
	SoundManager() = default;
	SoundManager(const SoundManager&) = delete;
	SoundManager& operator=(const SoundManager&) = delete;
};

