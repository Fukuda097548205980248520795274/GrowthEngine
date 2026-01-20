#include "AudioStore.h"
#include <cassert>
#include "Log/Log.h"
#include "Func/ConvertString/ConvertString.h"
#include "Func/RandomFunc/RandomFunc.h"


/// <summary>
/// デストラクタ
/// </summary>
Engine::AudioData::~AudioData()
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
	for (std::unique_ptr<PlayData>& playDatum : playData_)
	{
		if (playDatum->pSourceVoice)
		{
			playDatum->pSourceVoice->Stop(0);
			playDatum->pSourceVoice->DestroyVoice();
		}
	}
	// playData_ リストが unique_ptr であれば、ここで自動的に要素が解放される

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
	MFStartup(MF_VERSION, MFSTARTUP_NOSOCKET);
	if (log)log->Logging("MFStartup : MF_VERSION , MFSTARTUP_NOSOCKET");

	// XAudio2を初期化する
	HRESULT hr = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(hr));
	if (log)log->Logging("XAudio2Create : XAUDIO2_DEFAULT_PROCESSOR");

	// マスターボイスを生成する
	hr = xAudio2_->CreateMasteringVoice(&masterVoice_);
	assert(SUCCEEDED(hr));
	if (log)log->Logging("CreateMasteringVoice \n");
}

/// @brief ファイルを読む
/// @param filePath 
/// @return 
AudioHandle Engine::AudioStore::Load(const std::string& filePath, Log* log)
{
	// 同じファイルパスを見つけたら、そのハンドルを返す
	for (std::unique_ptr<AudioData>& data : audioData_)
	{
		if (strcmp(filePath.c_str(), data->filePath.c_str()) == 0)
			return data->handle;
	}

	// wStringに変換する
	const std::wstring filePathW = ConvertString(filePath);

	// ソースレーダを作成する
	ComPtr<IMFSourceReader> pMFSourceReader{ nullptr };
	HRESULT hr = MFCreateSourceReaderFromURL(filePathW.c_str(), NULL, &pMFSourceReader);
	assert(SUCCEEDED(hr));


	// メディアタイプの作成
	ComPtr<IMFMediaType> pReader{ nullptr };
	hr = MFCreateMediaType(&pReader);
	assert(SUCCEEDED(hr));

	// ソースレーダとメディアタイプの設定
	pReader->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
	pReader->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
	hr = pMFSourceReader->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, pReader.Get());
	assert(SUCCEEDED(hr));

	// メディアタイプを解放し、再度作成する
	ComPtr<IMFMediaType> pOutType;
	hr = pMFSourceReader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, &pOutType);
	assert(SUCCEEDED(hr));


	// オーディオデータを作成する
	std::unique_ptr<AudioData> audioDatum = std::make_unique<AudioData>();
	audioDatum->filePath = filePath;

	// サウンドハンドルを取得する
	AudioHandle soundHandle;
	soundHandle = static_cast<uint32_t>(audioData_.size());
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
	audioData_.push_back(std::move(audioDatum));

	return soundHandle;
}


/// <summary>
/// オーディオを流す
/// </summary>
/// <param name="soundHandle"></param>
/// <param name="volume"></param>
/// <returns></returns>
PlayHandle Engine::AudioStore::PlayAudio(AudioHandle handle, float volume)
{
	// プレイデータを生成する
	std::unique_ptr<PlayData> playDatum = std::make_unique<PlayData>();

	// プレイハンドルを作成する
	PlayHandle playHandle{};
	while (playHandle == 0)
	{
		playHandle = GetRandomRange(1, 10000000);

		for (std::unique_ptr<PlayData>& data : playData_)
		{
			if (playHandle == data->handle)
			{
				playHandle = 0;
				break;
			}
		}
	}
	playDatum->handle = playHandle;


	// ソースボイスを生成する
	HRESULT hr = xAudio2_->CreateSourceVoice(&playDatum->pSourceVoice, audioData_[handle]->waveFormat);
	assert(SUCCEEDED(hr));

	XAUDIO2_BUFFER buffer{ 0 };
	buffer.pAudioData = audioData_[handle]->mediaData.data();
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.AudioBytes = sizeof(BYTE) * static_cast<UINT32>(audioData_[handle]->mediaData.size());
	playDatum->pSourceVoice->SubmitSourceBuffer(&buffer);

	const float kMaxSoundVolume = 1.0f;
	const float kMinSoundVolume = 0.0f;

	// 規格外の音にならぬようにする
	volume = std::max(kMinSoundVolume, volume);
	volume = std::min(kMaxSoundVolume, volume);

	playDatum->pSourceVoice->SetVolume(volume);

	playDatum->pSourceVoice->Start(0);

	// リストに登録する
	playData_.push_back(std::move(playDatum));

	return playHandle;
}


/// <summary>
/// 音声を停止する
/// </summary>
/// <param name="playHandle"></param>
void Engine::AudioStore::StopAudio(PlayHandle handle)
{
	for (std::unique_ptr<PlayData>& playDatum : playData_)
	{
		if (handle == playDatum->handle)
		{
			playDatum->pSourceVoice->Stop(0);
			playDatum->pSourceVoice->DestroyVoice();
			playDatum->pSourceVoice = nullptr;

			return;
		}
	}

	return;
}

/// <summary>
/// 音楽が再生されているかどうか
/// </summary>
/// <param name="playHandle"></param>
bool Engine::AudioStore::IsAudioPlay(PlayHandle handle)
{
	for (std::unique_ptr<PlayData>& playDatum : playData_)
	{
		if (handle == playDatum->handle)
		{
			return true;
		}
	}

	return false;
}


/// <summary>
/// 音量を設定する
/// </summary>
/// <param name="playHandle"></param>
/// <param name="setVolume"></param>
void Engine::AudioStore::SetVolume(PlayHandle handle, float volume)
{
	const float kMaxSoundVolume = 1.0f;
	const float kMinSoundVolume = 0.0f;

	// 規格外の音にならぬようにする
	volume = std::max(kMinSoundVolume, volume);
	volume = std::min(kMaxSoundVolume, volume);

	// ハンドルが一致する構造体を探す
	for (std::unique_ptr<PlayData>& playDatum : playData_)
	{
		if (handle == playDatum->handle)
		{
			playDatum->pSourceVoice->SetVolume(volume);

			return;
		}
	}

	return;
}

/// <summary>
/// ピッチを設定する
/// </summary>
/// <param name="playHandle"></param>
/// <param name="pitch"></param>
void Engine::AudioStore::SetPitch(PlayHandle handle, float pitch)
{
	// ハンドルが一致する構造体を探す
	for (std::unique_ptr<PlayData>& playDatum : playData_)
	{
		if (handle == playDatum->handle)
		{
			playDatum->pSourceVoice->SetFrequencyRatio(pitch);

			return;
		}
	}

	return;
}


/// <summary>
/// 流れているオーディオを削除する
/// </summary>
void Engine::AudioStore::DeletePlayAudio()
{
	playData_.remove_if([](std::unique_ptr<PlayData>& playDatum)
		{
			if (playDatum->pSourceVoice)
			{
				XAUDIO2_VOICE_STATE state;
				playDatum->pSourceVoice->GetState(&state);

				if (state.BuffersQueued <= 0)
				{
					playDatum->pSourceVoice->DestroyVoice();
					playDatum->pSourceVoice = nullptr;
					return true;
				}
			} else
			{
				return true;
			}
			return false;
		}
	);
}