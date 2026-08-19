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
	void SeDeflect() { if (seDeflect_)seDeflect_->Play(); }

	/// @brief 弾きのSE
	void SeRepel() { if (seRepel_)seRepel_->Play(); }

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

	/// @brief 武器の着地のSE
	void SeLandingWeapon() { if (seLandingWeapon_)seLandingWeapon_->Play(); }

	/// @brief ダウン中着地のSE
	void SeDownLanding() { if (seDownLanding_)seDownLanding_->Play(); }

	/// @brief レイジモード開始のSE
	void SeRageModeStart() { if (seRageModeStart_)seRageModeStart_->Play(); }

	/// @brief レイジモード終了のSE
	void SeRageModeEnd() { if (seRageModeEnd_)seRageModeEnd_->Play(); }

	/// @brief 軽攻撃のSE
	void SeLightAttack() { if (seLightAttack_)seLightAttack_->Play(); }

	/// @brief 重攻撃のSE
	void SeHeavyAttack() { if (seHeavyAttack_)seHeavyAttack_->Play(); }

	/// @brief 掴みのSE
	void SeGrab() { if (seGrab_)seGrab_->Play(); }

	/// @brief クリティカルダメージのSE
	void SeCriticalDamage() { if (seCriticalDamage_)seCriticalDamage_->Play(); }

	/// @brief 武器ダメージのSE
	void SeWeaponDamage() { if (seWeaponDamage_)seWeaponDamage_->Play(); }

	/// @brief 武器破壊のSE
	void SeWeaponBreak() { if (seWeaponBreak_)seWeaponBreak_->Play(); }

	/// @brief ロックオンのSE
	void SeLockOn() { if (seLockOn_)seLockOn_->Play(); }

	/// @brief 勝利のSE
	void SeWin() { if (seWin_)seWin_->Play(); }

	/// @brief 敗北のSE
	void SeLose() { if (seLose_)seLose_->Play(); }

	/// @brief チュートリアルの道中のBGMを再生する
	/// @param isPlay 
	void BgmTutorialRoadPlay(bool isPlay) { tutorialRoadBgm_->param_->volume = 0.25f; if (tutorialRoadBgm_) isPlay ? tutorialRoadBgm_->Play() : tutorialRoadBgm_->Stop(); }

	/// @brief チュートリアルのボス戦のBGMを再生する
	/// @param isPlay 
	void BgmTutorialBossPlay(bool isPlay) { tutorialBossBgm_->param_->volume = 0.25f; if (tutorialBossBgm_) isPlay ? tutorialBossBgm_->Play() : tutorialBossBgm_->Stop(); }

	/// @brief チュートリアルの道中のBGMを取得する
	/// @return 
	Bgm* GetTutorialRoadBgm() { return tutorialRoadBgm_.get(); }

	/// @brief チュートリアルのボス戦のBGMを取得する
	/// @return 
	Bgm* GetTutorialBossBgm() { return tutorialBossBgm_.get(); }


private:

	/// @brief チュートリアルの道中のBGM
	std::unique_ptr<Bgm> tutorialRoadBgm_ = nullptr;

	/// @brief チュートリアルのボス戦のBGM
	std::unique_ptr<Bgm> tutorialBossBgm_ = nullptr;


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
	std::unique_ptr<Se> seDeflect_ = nullptr;

	/// @brief 弾きのSE
	std::unique_ptr<Se> seRepel_ = nullptr;

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

	/// @brief 武器の着地のSE
	std::unique_ptr<Se> seLandingWeapon_ = nullptr;

	/// @brief ダウン中着地のSE
	std::unique_ptr<Se> seDownLanding_ = nullptr;

	/// @brief レイジモード開始のSE
	std::unique_ptr<Se> seRageModeStart_ = nullptr;

	/// @brief レイジモード終了のSE
	std::unique_ptr<Se> seRageModeEnd_ = nullptr;

	/// @brief 軽攻撃のSE
	std::unique_ptr<Se> seLightAttack_ = nullptr;

	/// @brief 重攻撃のSE
	std::unique_ptr<Se> seHeavyAttack_ = nullptr;

	/// @brief 掴み攻撃のSE
	std::unique_ptr<Se> seGrab_ = nullptr;

	/// @brief クリティカルダメージのSE
	std::unique_ptr<Se> seCriticalDamage_ = nullptr;

	/// @brief 武器ダメージのSE
	std::unique_ptr<Se> seWeaponDamage_ = nullptr;

	/// @brief 武器破壊のSE
	std::unique_ptr<Se> seWeaponBreak_ = nullptr;

	/// @brief ロックオンのSE
	std::unique_ptr<Se> seLockOn_ = nullptr;

	/// @brief 勝利のSE
	std::unique_ptr<Se> seWin_ = nullptr;

	/// @brief 敗北のSE
	std::unique_ptr<Se> seLose_ = nullptr;


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

