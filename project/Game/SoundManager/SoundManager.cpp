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
	bgmStyleSenran_ = std::make_unique<Sound>(engine_->LoadSound("Style_Senran", engine_->LoadAudio("./Assets/Sounds/bgm/Style_Senran.mp3")));

	// 旋嵐スタイルチェンジのSE
	seStyleSenran0_ = std::make_unique<Sound>(engine_->LoadSound("Style_Senran0", engine_->LoadAudio("./Assets/Sounds/se/styleChange/Senran/Senran0.mp3")));
	seStyleSenran1_ = std::make_unique<Sound>(engine_->LoadSound("Style_Senran1", engine_->LoadAudio("./Assets/Sounds/se/styleChange/Senran/Senran1.mp3")));
	seStyleSenran2_ = std::make_unique<Sound>(engine_->LoadSound("Style_Senran2", engine_->LoadAudio("./Assets/Sounds/se/styleChange/Senran/Senran2.mp3")));
	seStyleSenran3_ = std::make_unique<Sound>(engine_->LoadSound("Style_Senran3", engine_->LoadAudio("./Assets/Sounds/se/styleChange/Senran/Senran3.mp3")));
	seStyleSenran4_ = std::make_unique<Sound>(engine_->LoadSound("Style_Senran4", engine_->LoadAudio("./Assets/Sounds/se/styleChange/Senran/Senran4.mp3")));

	// 撃鉄スタイルのBGM
	bgmStyleGekitetu_ = std::make_unique<Sound>(engine_->LoadSound("Style_Gekitetu", engine_->LoadAudio("./Assets/Sounds/bgm/Style_Gekitetu.mp3")));

	// 撃鉄スタイルチェンジのSE
	seStyleGekitetu0_ = std::make_unique<Sound>(engine_->LoadSound("Style_Gekitetu0", engine_->LoadAudio("./Assets/Sounds/se/styleChange/Gekitetu/Gekitetu0.mp3")));
	seStyleGekitetu1_ = std::make_unique<Sound>(engine_->LoadSound("Style_Gekitetu1", engine_->LoadAudio("./Assets/Sounds/se/styleChange/Gekitetu/Gekitetu1.mp3")));
	seStyleGekitetu2_ = std::make_unique<Sound>(engine_->LoadSound("Style_Gekitetu2", engine_->LoadAudio("./Assets/Sounds/se/styleChange/Gekitetu/Gekitetu2.mp3")));
	seStyleGekitetu3_ = std::make_unique<Sound>(engine_->LoadSound("Style_Gekitetu3", engine_->LoadAudio("./Assets/Sounds/se/styleChange/Gekitetu/Gekitetu3.mp3")));
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