#include "AudioStore.h"
#include <cassert>
#include <algorithm>
#include <limits>
#include "Log/Log.h"
#include "Func/ConvertString/ConvertString.h"
#include "Func/RandomFunc/RandomFunc.h"
#include <format>

namespace
{
	// 音声を停止して破棄する関数
	void StopAndDestroyVoice(IXAudio2SourceVoice*& sourceVoice)
	{
		if (sourceVoice)
		{
			sourceVoice->Stop(0);
			sourceVoice->DestroyVoice();
			sourceVoice = nullptr;
		}
	}

	// ピッチを制御する関数
	float ClampPitch(float pitch)
	{
		return std::clamp(pitch, XAUDIO2_MIN_FREQ_RATIO, XAUDIO2_MAX_FREQ_RATIO);
	}
}

/// @brief デストラクタ
Engine::AudioStore::AudioData::~AudioData()
{
	if (waveFormat) {
		CoTaskMemFree(waveFormat);
		waveFormat = nullptr;
	}
	mediaData.clear();
}



/// @brief デストラクタ
Engine::AudioStore::~AudioStore()
{
	// 全ての再生中の音声を停止・破棄する
	for (std::unique_ptr<PlayData>& playDatum : playTable_)
		StopAndDestroyVoice(playDatum->pSourceVoice);

	// MFの終了処理
	HRESULT hr = MFShutdown();
	assert(SUCCEEDED(hr));

	// XAudio2インスタンスを破棄する
	xAudio2_.Reset();
}

/// @brief 初期化
/// @param log 
void Engine::AudioStore::Initialize(Log* log)
{
	// MFの初期化（ローカル版）
	HRESULT hr = MFStartup(MF_VERSION, MFSTARTUP_NOSOCKET);
	assert(SUCCEEDED(hr));
	if (FAILED(hr))
	{
		if (log)log->Logging("Error : MFStartup");
		return;
	}
	if (log)log->Logging("MFStartup : MF_VERSION , MFSTARTUP_NOSOCKET");

	// XAudio2を初期化する
    hr = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(hr));
	if (FAILED(hr))
	{
		if (log)log->Logging("Error : XAudio2Create");
		return;
	}
	if (log)log->Logging("XAudio2Create : XAUDIO2_DEFAULT_PROCESSOR");

	// マスターボイスを生成する
	hr = xAudio2_->CreateMasteringVoice(&masterVoice_);
	assert(SUCCEEDED(hr));
    if (FAILED(hr))
	{
		if (log)log->Logging("Error : CreateMasteringVoice");
		return;
	}
	if (log)log->Logging("CreateMasteringVoice \n");
}

/// @brief 更新処理
void Engine::AudioStore::Update()
{
	// 再生中の音声を確認し、再生が終了しているものは破棄する
	playTable_.remove_if([](std::unique_ptr<PlayData>& playDatum)
		{
			// ソースボイスが存在する場合、再生状態を確認する
			if (playDatum->pSourceVoice)
			{
				// 再生状態を取得する
				XAUDIO2_VOICE_STATE state;
				playDatum->pSourceVoice->GetState(&state);

				// 再生が終了している場合、ソースボイスを破棄する
				if (state.BuffersQueued <= 0)
				{
                    StopAndDestroyVoice(playDatum->pSourceVoice);
					return true;
				}
			} 
			else
			{
				// ソースボイスが存在しない場合は、すでに再生が終了しているとみなして破棄する
				return true;
			}

			return false;
		}
	);
}

/// @brief ファイルを読む
/// @param filePath 
/// @return 
AudioHandle Engine::AudioStore::Load(const std::string& filePath, Log* log)
{
	// 同じファイルパスを見つけたら、そのハンドルを返す
	for (std::unique_ptr<AudioData>& data : audioTable_)
	{
		if (filePath == data->filePath)
			return data->handle;
	}

	// wStringに変換する
	const std::wstring filePathW = ConvertString(filePath);

	// ソースレーダを作成する
	ComPtr<IMFSourceReader> pMFSourceReader{ nullptr };
	HRESULT hr = MFCreateSourceReaderFromURL(filePathW.c_str(), NULL, &pMFSourceReader);
	assert(SUCCEEDED(hr));
	if (log)log->Logging(std::format("MFCreateSourceReaderFromURL : {}", filePath.c_str()));


	// メディアタイプの作成
	ComPtr<IMFMediaType> pReader{ nullptr };
	hr = MFCreateMediaType(&pReader);
	assert(SUCCEEDED(hr));
	if (log)log->Logging("MFCreateMediaType");

	// ソースレーダとメディアタイプの設定
	pReader->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
	pReader->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
	hr = pMFSourceReader->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, pReader.Get());
	assert(SUCCEEDED(hr));
	if (log)
	{
		log->Logging("Set GUID : MF_MT_MAJOR_TYPE , MFMediaType_Audio");
		log->Logging("Set GUID : MF_MT_SUBTYPE , MFAudioFormat_PCM");
		log->Logging("SetCurrentMediaType : MF_SOURCE_READER_FIRST_AUDIO_STREAM");
	}

	// メディアタイプを解放し、再度作成する
	ComPtr<IMFMediaType> pOutType;
	hr = pMFSourceReader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, &pOutType);
	assert(SUCCEEDED(hr));
	if (log)log->Logging("GetCurrentMediaType : MF_SOURCE_READER_FIRST_AUDIO_STREAM");


	// オーディオデータを作成する
	std::unique_ptr<AudioData> audioDatum = std::make_unique<AudioData>();
	audioDatum->filePath = filePath;

	// サウンドハンドルを取得する
	AudioHandle soundHandle;
	soundHandle = static_cast<uint32_t>(audioTable_.size());
	audioDatum->handle = soundHandle;

	// ウェーブフォーマットを作成する
	MFCreateWaveFormatExFromMFMediaType(pOutType.Get(), &audioDatum->waveFormat, nullptr);

	while (true)
	{
		ComPtr<IMFSample> pMFSample{ nullptr };

		DWORD streamIndex = 0;
		DWORD flags = 0;
		LONGLONG llTimeStamp = 0;

		hr = pMFSourceReader->ReadSample(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, &streamIndex, &flags, &llTimeStamp, &pMFSample);
		assert(SUCCEEDED(hr));

		if (flags & MF_SOURCE_READERF_ENDOFSTREAM)break;

		if (pMFSample)
		{
			ComPtr<IMFMediaBuffer> pBuffer{ nullptr };
			hr = pMFSample->ConvertToContiguousBuffer(&pBuffer);
			assert(SUCCEEDED(hr));

			BYTE* pData{ nullptr };
			DWORD currentLength = 0;
			hr = pBuffer->Lock(&pData, nullptr, &currentLength);
			assert(SUCCEEDED(hr));

			audioDatum->mediaData.resize(audioDatum->mediaData.size() + currentLength);
			memcpy(audioDatum->mediaData.data() + audioDatum->mediaData.size() - currentLength, pData, currentLength);

			pBuffer->Unlock();
		}
	}

	// 配列に登録する
	audioTable_.push_back(std::move(audioDatum));
	if (log)log->Logging("Success : Load Audio \n");

	return soundHandle;
}


/// @brief オーディオを流す
/// @param handle 
/// @param volume 
/// @return 
PlayHandle Engine::AudioStore::PlayAudio(AudioHandle handle, float volume)
{
	if (!xAudio2_)
		return 0;

	const AudioData* audioData = FindAudioData(handle);
	if (!audioData)
		return 0;

	if (!audioData->waveFormat || audioData->mediaData.empty())
		return 0;

	// プレイデータを生成する
	std::unique_ptr<PlayData> playDatum = std::make_unique<PlayData>();

	// プレイハンドルを作成する
    PlayHandle playHandle = GenerateUniquePlayHandle();
	playDatum->handle = playHandle;


	// ソースボイスを生成する
	HRESULT hr = xAudio2_->CreateSourceVoice(&playDatum->pSourceVoice, audioData->waveFormat);
	assert(SUCCEEDED(hr));
	if (FAILED(hr) || !playDatum->pSourceVoice)
		return 0;

	XAUDIO2_BUFFER buffer{ 0 };
	buffer.pAudioData = audioData->mediaData.data();
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.AudioBytes = sizeof(BYTE) * static_cast<UINT32>(audioData->mediaData.size());
	hr = playDatum->pSourceVoice->SubmitSourceBuffer(&buffer);
	assert(SUCCEEDED(hr));
	if (FAILED(hr))
	{
		StopAndDestroyVoice(playDatum->pSourceVoice);
		return 0;
	}

	// 規格外の音にならぬようにする
	volume = ClampVolume(volume);

	hr = playDatum->pSourceVoice->SetVolume(volume);
	assert(SUCCEEDED(hr));
	if (FAILED(hr))
	{
		StopAndDestroyVoice(playDatum->pSourceVoice);
		return 0;
	}

	hr = playDatum->pSourceVoice->Start(0);
	assert(SUCCEEDED(hr));
	if (FAILED(hr))
	{
		StopAndDestroyVoice(playDatum->pSourceVoice);
		return 0;
	}

	// リストに登録する
	playTable_.push_back(std::move(playDatum));

	return playHandle;
}


/// @brief オーディオを停止する
/// @param handle 
void Engine::AudioStore::StopAudio(PlayHandle handle)
{
	for (auto it = playTable_.begin(); it != playTable_.end(); ++it)
	{
		PlayData* playData = it->get();
		if (handle != playData->handle)
		{
			continue;
		}

        StopAndDestroyVoice(playData->pSourceVoice);
		playTable_.erase(it);
		return;
	}
}

/// @brief オーディオを再生されているかどうか
/// @param handle 
/// @return 
bool Engine::AudioStore::IsAudioPlay(PlayHandle handle)
{
	PlayData* playData = FindPlayData(handle);
	if (!playData || !playData->pSourceVoice)
	{
		return false;
	}

	XAUDIO2_VOICE_STATE state{};
	playData->pSourceVoice->GetState(&state);

	return state.BuffersQueued > 0;
}


/// @brief ボリュームを設定する
/// @param handle 
/// @param volume 
void Engine::AudioStore::SetVolume(PlayHandle handle, float volume)
{
	// 規格外の音にならぬようにする
	volume = ClampVolume(volume);

	// ハンドルが一致する構造体を探す
	PlayData* playData = FindPlayData(handle);
	if (!playData || !playData->pSourceVoice)
		return;

	HRESULT hr = playData->pSourceVoice->SetVolume(volume);
	assert(SUCCEEDED(hr));
}

/// @brief ピッチを設定する
/// @param handle 
/// @param pitch 
void Engine::AudioStore::SetPitch(PlayHandle handle, float pitch)
{
	pitch = ClampPitch(pitch);

	// ハンドルが一致する構造体を探す
	PlayData* playData = FindPlayData(handle);
	if (!playData || !playData->pSourceVoice)
		return;

	HRESULT hr = playData->pSourceVoice->SetFrequencyRatio(pitch);
	assert(SUCCEEDED(hr));
}

/// @brief ハンドルに対応するプレイデータを探す
/// @param handle 
/// @return 
Engine::AudioStore::PlayData* Engine::AudioStore::FindPlayData(PlayHandle handle)
{
	for (std::unique_ptr<PlayData>& playDatum : playTable_)
	{
		if (handle == playDatum->handle)
		{
			return playDatum.get();
		}
	}

	return nullptr;
}

/// @brief ハンドルに対応するオーディオデータを探す
/// @param handle 
/// @return 
const Engine::AudioStore::AudioData* Engine::AudioStore::FindAudioData(AudioHandle handle) const
{
	if (handle >= audioTable_.size())
		return nullptr;

	return audioTable_[handle].get();
}

/// @brief ユニークなプレイハンドルを生成する
/// @return 
PlayHandle Engine::AudioStore::GenerateUniquePlayHandle() const
{
    constexpr uint32_t kMaxRetryCount = 64;
	for (uint32_t retry = 0; retry < kMaxRetryCount; ++retry)
	{
		const PlayHandle playHandle = GetRandomRange(1, 10000000);
		const bool isDuplicate = std::any_of(playTable_.begin(), playTable_.end(), [playHandle](const std::unique_ptr<PlayData>& data)
			{
				return playHandle == data->handle;
			});

        if (!isDuplicate)
		{
         return playHandle;
		}
	}

	for (PlayHandle playHandle = 1; playHandle < std::numeric_limits<PlayHandle>::max(); ++playHandle)
	{
		const bool isDuplicate = std::any_of(playTable_.begin(), playTable_.end(), [playHandle](const std::unique_ptr<PlayData>& data)
			{
				return playHandle == data->handle;
			});

		if (!isDuplicate)
		{
			return playHandle;
		}
	}

	return 0;
}

/// @brief 音量を制御する
/// @param volume 
/// @return 
float Engine::AudioStore::ClampVolume(float volume)
{
	const float kMaxSoundVolume = 1.0f;
	const float kMinSoundVolume = 0.0f;
	return std::clamp(volume, kMinSoundVolume, kMaxSoundVolume);
}
