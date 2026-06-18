#include "Se.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
Se::Se(const std::string& name, AudioHandle hAudio) : Sound()
{
	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

	hSound_ = engine_->LoadSound(name, hAudio, Engine::SoundType::Se);

	// サウンドパラメータを取得する
	param_ = engine_->GetSoundParam<Engine::SeParam>(hSound_);
}

/// @brief 再生
void Se::Play()const
{
	engine_->SoundPlay(hSound_);
}