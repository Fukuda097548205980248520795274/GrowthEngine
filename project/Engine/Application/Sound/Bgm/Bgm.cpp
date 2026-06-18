#include "Bgm.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
Bgm::Bgm(const std::string& name, AudioHandle hAudio) : Sound()
{
	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

	hSound_ = engine_->LoadSound(name, hAudio, Engine::SoundType::Bgm);

	// サウンドパラメータを取得する
	param_ = engine_->GetSoundParam<Engine::BgmParam>(hSound_);
}

/// @brief 再生
void Bgm::Play()const
{
	engine_->SoundPlay(hSound_);
}

/// @brief 停止
void Bgm::Stop()const
{
	engine_->SoundStop(hSound_);
}

/// @brief 再生されているかどうか
/// @return 
bool Bgm::IsPlaying()const
{
	return engine_->IsSoundPlay(hSound_);
}