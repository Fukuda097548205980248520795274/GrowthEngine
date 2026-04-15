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

	class AudioStore
	{
	public:

		// オーディオデータ
		class AudioData
		{
		public:

			/// @brief デストラクタ
			~AudioData();

			/// @brief ファイルパスを取得する
			/// @return ファイルパス
			std::string GetFilePath() const { return filePath; }

			/// @brief ハンドルを取得する
			/// @return ハンドル
			AudioHandle GetHandle() const { return handle; }

			// ファイルパス
			std::string filePath;

			// フォーマット
           WAVEFORMATEX* waveFormat = nullptr;

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
          IXAudio2SourceVoice* pSourceVoice = nullptr;
		};


	public:

		/// @brief デストラクタ
		~AudioStore();

		/// @brief 初期化
		/// @param log 
		void Initialize(Log* log);

		/// @brief 更新処理
		void Update();

		/// @brief ファイルを読む
		/// @param filePath 
		/// @return 
		AudioHandle Load(const std::string& filePath, Log* log);

		/// @brief 音声を流す
		/// @param handle 
		/// @param volume 
		/// @return 
		PlayHandle PlayAudio(AudioHandle handle, float volume);

		/// @brief ファイルパスを取得する
		/// @param handle 
		/// @return 
		std::string GetFilePath(AudioHandle handle) const { return audioTable_[handle]->GetFilePath(); }

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

		// Microsoft::WRL:: 省略
		template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;


	private:

		/// @brief ハンドルに対応するプレイデータを探す
		/// @param handle 
		/// @return 
		PlayData* FindPlayData(PlayHandle handle);

		/// @brief ハンドルに対応するオーディオデータを探す
		/// @param handle 
		/// @return 
		const AudioData* FindAudioData(AudioHandle handle) const;

		/// @brief ユニークなプレイハンドルを生成する 
		PlayHandle GenerateUniquePlayHandle() const;

		/// @brief 音量を制御する
		/// @param volume 
		/// @return 
		static float ClampVolume(float volume);



		// XAudio2
		ComPtr<IXAudio2> xAudio2_ = nullptr;

		// マスターボイス
		IXAudio2MasteringVoice* masterVoice_ = nullptr;


	private:

		// オーディオテーブル
		std::vector<std::unique_ptr<AudioData>> audioTable_;

		// プレイテーブル
		std::list<std::unique_ptr<PlayData>> playTable_;
	};
}