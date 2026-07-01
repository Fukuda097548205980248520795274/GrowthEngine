#include "SoundManager.h"

// インスタンスの初期化
std::unique_ptr<SoundManager> SoundManager::instance_ = nullptr;

/// @brief インスタンスを取得する
/// @return 
SoundManager* SoundManager::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_.reset(new SoundManager());
		instance_->Initialize();
	}

	return instance_.get();
}

/// @brief 初期化
void SoundManager::Initialize()
{
	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

	// 旋嵐スタイルのBGM
	bgmStyleSenran_ = std::make_unique<Bgm>("Style_Senran", engine_->LoadAudio("./Assets/Sounds/bgm/Style_Senran.mp3"));
	bgmStyleSenran_->param_->enableLoop = true; // ループ再生を有効にする
	bgmStyleSenran_->param_->volume = 0.01f; // 音量を調整する

	// 旋嵐スタイルチェンジのSE
	seStyleSenran0_ = std::make_unique<Se>("Style_Senran0", engine_->LoadAudio("./Assets/Sounds/se/styleChange/Senran/Senran0.mp3"));
	seStyleSenran1_ = std::make_unique<Se>("Style_Senran1", engine_->LoadAudio("./Assets/Sounds/se/styleChange/Senran/Senran1.mp3"));
	seStyleSenran2_ = std::make_unique<Se>("Style_Senran2", engine_->LoadAudio("./Assets/Sounds/se/styleChange/Senran/Senran2.mp3"));
	seStyleSenran3_ = std::make_unique<Se>("Style_Senran3", engine_->LoadAudio("./Assets/Sounds/se/styleChange/Senran/Senran3.mp3"));
	seStyleSenran4_ = std::make_unique<Se>("Style_Senran4", engine_->LoadAudio("./Assets/Sounds/se/styleChange/Senran/Senran4.mp3"));

	// 撃鉄スタイルのBGM
	bgmStyleGekitetu_ = std::make_unique<Bgm>("Style_Gekitetu", engine_->LoadAudio("./Assets/Sounds/bgm/Style_Gekitetu.mp3"));
	bgmStyleGekitetu_->param_->enableLoop = true; // ループ再生を有効にする
	bgmStyleGekitetu_->param_->volume = 0.01f; // 音量を調整する

	// 撃鉄スタイルチェンジのSE
	seStyleGekitetu0_ = std::make_unique<Se>("Style_Gekitetu0", engine_->LoadAudio("./Assets/Sounds/se/styleChange/Gekitetu/Gekitetu0.mp3"));
	seStyleGekitetu1_ = std::make_unique<Se>("Style_Gekitetu1", engine_->LoadAudio("./Assets/Sounds/se/styleChange/Gekitetu/Gekitetu1.mp3"));
	seStyleGekitetu2_ = std::make_unique<Se>("Style_Gekitetu2", engine_->LoadAudio("./Assets/Sounds/se/styleChange/Gekitetu/Gekitetu2.mp3"));
	seStyleGekitetu3_ = std::make_unique<Se>("Style_Gekitetu3", engine_->LoadAudio("./Assets/Sounds/se/styleChange/Gekitetu/Gekitetu3.mp3"));

	// 受け流しのSE
	seDeflect_ = std::make_unique<Se>("Deflect", engine_->LoadAudio("./Assets/Sounds/se/deflect.mp3"));

	// 弾きのSE
	seRepel_ = std::make_unique<Se>("Repel", engine_->LoadAudio("./Assets/Sounds/se/repel.mp3"));

	// ガードのSE
	seGuard_ = std::make_unique<Se>("Guard", engine_->LoadAudio("./Assets/Sounds/se/guard.mp3"));

	// 回避のSE
	seAvoid_ = std::make_unique<Se>("Avoid", engine_->LoadAudio("./Assets/Sounds/se/avoid.mp3"));

	// 軽ダメージのSE
	seLightDamage_ = std::make_unique<Se>("LightDamage", engine_->LoadAudio("./Assets/Sounds/se/lightDamage.mp3"));

	// 重ダメージのSE
	seHeavyDamage_ = std::make_unique<Se>("HeavyDamage", engine_->LoadAudio("./Assets/Sounds/se/heavyDamage.mp3"));

	// ガードブレイクのSE
	seGuardBreak_ = std::make_unique<Se>("GuardBreak", engine_->LoadAudio("./Assets/Sounds/se/guardBreak.mp3"));

	// 着地のSE
	seLanding_ = std::make_unique<Se>("Landing", engine_->LoadAudio("./Assets/Sounds/se/landing.mp3"));

	// ダウン中着地のSE
	seDownLanding_ = std::make_unique<Se>("DownLanding", engine_->LoadAudio("./Assets/Sounds/se/downLanding.mp3"));

	// レイジモード開始のSE
	seRageModeStart_ = std::make_unique<Se>("RageModeStart", engine_->LoadAudio("./Assets/Sounds/se/rageModeStart.mp3"));

	// レイジモード終了のSE
	seRageModeEnd_ = std::make_unique<Se>("RageModeEnd", engine_->LoadAudio("./Assets/Sounds/se/rageModeEnd.mp3"));
}

/// @brief 旋嵐スタイルチェンジのSE
void SoundManager::SeStyleChangeSenran()const
{
	seStyleSenran0_->Play();
	seStyleSenran1_->Play();
	seStyleSenran2_->Play();
	seStyleSenran3_->Play();
	seStyleSenran4_->Play();
}

/// @brief 撃鉄スタイルチェンジのSE
void SoundManager::SeStyleChangeGekitetu()const
{
	seStyleGekitetu0_->Play();
	seStyleGekitetu1_->Play();
	seStyleGekitetu2_->Play();
	seStyleGekitetu3_->Play();
}