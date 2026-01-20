#include "AudioObject.h"
#include "GrowthEngine.h"

/// @brief デストラクタ
AudioObject::~AudioObject()
{
	// 流れている曲を停止する
	if (engine_->IsPlayAudio(ph_))engine_->StopAudio(ph_);
}

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

/// @brief 更新処理
void AudioObject::Update()
{
	if (isPlay_ == false)
		return;

	// ループ再生
	if (isLoop_)
	{
		if (!engine_->IsPlayAudio(ph_) || ph_ == 0)
		{
			ph_ = engine_->PlayAudio(ah_, volume_);
		}

		return;
	}
	else
	{
		if (!engine_->IsPlayAudio(ph_) || ph_ == 0)
		{
			isPlay_ = false;
		}
	}
}

/// @brief オーディオを流す
void AudioObject::PlayAudio()
{
	engine_->StopAudio(ph_);
	ph_ = engine_->PlayAudio(ah_, volume_);
	isPlay_ = true;
}

/// @brief オーディオを止める
void AudioObject::StopAudio()
{
	engine_->StopAudio(ph_);
	isPlay_ = false;
}

/// @brief ピッチを設定する
/// @param pitch 
void AudioObject::SetPitch(float pitch)
{
	pitch_ = pitch;
	engine_->SetPitch(ph_, pitch_);
}

/// @brief ボリュームを設定する
/// @param volume 
void AudioObject::SetVolume(float volume)
{
	volume_ = volume;
	engine_->SetVolume(ph_, volume_);
}

/// @brief ループの設定
/// @param isLoop 
void AudioObject::SetLoop(bool isLoop)
{
	isLoop_ = isLoop;
}