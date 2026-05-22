#include "Sound.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
Sound::Sound(SoundHandle hSound) : hSound_(hSound)
{
	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

	// サウンドパラメータを取得する
	param_ = engine_->GetSoundParam(hSound_);
}

/// @brief 再生
void Sound::Play()const
{
	engine_->SoundPlay(hSound_);
}

/// @brief 停止
void Sound::Stop()const
{
	engine_->SoundStop(hSound_);
}

/// @brief 再生されているかどうか
/// @return 
bool Sound::IsPlaying()const
{
	return engine_->IsSoundPlay(hSound_);
}