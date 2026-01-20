#pragma once
#define NOMINMAX
#include <vector>
#include <string>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <xaudio2.h>
#include <mferror.h>
#include <list>
#include <wrl.h>
#include <memory>
#include "Handle/Handle.h"

namespace Engine
{
	class Log;

	// オーディオデータ
	class AudioData
	{
	public:

		/// @brief デストラクタ
		~AudioData();

		// ファイルパス
		std::string filePath;

		// フォーマット
		WAVEFORMATEX* waveFormat;

		// メディアデータ
		std::vector<BYTE> mediaData;

		// サウンドハンドル
		AudioHandle handle;
	};

	// プレイデータ
	class PlayData
	{
	public:

		// プレイハンドル
		PlayHandle handle;

		// ソースボイス
		IXAudio2SourceVoice* pSourceVoice;
	};

	class AudioStore
	{
	public:

		/// @brief デストラクタ
		~AudioStore();

		/// @brief 初期化
		/// @param log 
		void Initialize(Log* log);

		/// @brief ファイルを読む
		/// @param filePath 
		/// @return 
		AudioHandle Load(const std::string& filePath, Log* log);

		/// @brief 音声を流す
		/// @param handle 
		/// @param volume 
		/// @return 
		PlayHandle PlayAudio(AudioHandle handle, float volume);

		/// @brief 音声を停止する
		/// @param handle 
		void StopAudio(PlayHandle handle);

		/// @brief 音声が流れているかどうか
		/// @param handle 
		/// @return 
		bool IsAudioPlay(PlayHandle handle);

		/// @brief 音量を設定する
		/// @param handle 
		/// @param volume 
		void SetVolume(PlayHandle handle, float volume);

		/// @brief ピッチを設定する
		/// @param handle 
		/// @param pitch 
		void SetPitch(PlayHandle handle, float pitch);

		/// @brief 流れているオーディオを削除する
		void DeletePlayAudio();

		// Microsoft::WRL:: 省略
		template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;


	private:



		// XAudio2
		ComPtr<IXAudio2> xAudio2_ = nullptr;

		// マスターボイス
		IXAudio2MasteringVoice* masterVoice_ = nullptr;

		// オーディオデータ配列
		std::vector<std::unique_ptr<AudioData>> audioData_;

		// プレイデータ配列
		std::list<std::unique_ptr<PlayData>> playData_;
	};
}