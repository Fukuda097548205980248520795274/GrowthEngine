#include "AudioObject.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param filePath 
/// @param volume 
/// @param isLoop 
AudioObject::AudioObject(const std::string& filePath, float volume, bool isLoop)
	: volume_(volume) , isLoop_(isLoop)
{
	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

	// サウンドハンドルを取得する
	ah_ = engine_->LoadAudio(filePath);

	// プレイハンドル初期化
	ph_ = 0;
}

/// <summary>
/// 更新処理
/// </summary>
void AudioObject::Update()
{
	if (isPlay_ == false)
		return;

	engine_->SetVolume(ph_, volume_);
	engine_->SetPitch(ph_, pitch_);

	// ループ再生
	if (isLoop_)
	{
		if (!engine_->IsPlayAudio(ph_) || ph_ == 0)
		{
			ph_ = engine_->PlayAudio(ah_, volume_);
		}

		return;
	} else
	{
		if (!engine_->IsPlayAudio(ph_) || ph_ == 0)
		{
			isPlay_ = false;
		}
	}
}

/// <summary>
/// サウンドを流す
/// </summary>
void AudioObject::PlayAudio()
{
	engine_->StopAudio(ph_);
	ph_ = engine_->PlayAudio(ah_, volume_);
	isPlay_ = true;
}

/// <summary>
/// オーディオを止める
/// </summary>
void AudioObject::StopAudio()
{
	engine_->StopAudio(ph_);
	isPlay_ = false;
}

/// <summary>
/// ピッチを設定する
/// </summary>
void AudioObject::SetPitch(float pitch)
{
	pitch_ = pitch;
	engine_->SetPitch(ph_, pitch_);
}

/// <summary>
/// ボリュームを設定する
/// </summary>
/// <param name="volume"></param>
void AudioObject::SetVolume(float volume)
{
	volume_ = volume;
	engine_->SetVolume(ph_, volume_);
}

/// <summary>
/// ループの設定
/// </summary>
/// <param name="isLoop"></param>
void AudioObject::SetLoop(bool isLoop)
{
	isLoop_ = isLoop;
}