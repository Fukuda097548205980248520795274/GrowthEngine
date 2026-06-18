#pragma once
#include "Handle/Handle.h"
#include <string>
#include <memory>

namespace Engine
{
	class AudioStore;
	class SoundParameter;

	enum class SoundType
	{
		Bgm,
		Se
	};

	// サウンドデータ
	class SoundData
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		/// @param hSound 
		/// @param hAudio 
		SoundData(const std::string& name, SoundHandle hSound)
			: name_(name), hSound_(hSound){
		}

		/// @brief デストラクタ
		virtual ~SoundData() = default;

		/// @brief 初期化
		/// @param audioStore 
		/// @param parameter 
		virtual void Initialize(AudioStore* audioStore, SoundParameter* parameter, AudioHandle hAudio) = 0;

		/// @brief リセット
		virtual void Reset() = 0;

		/// @brief 更新処理
		virtual void Update() = 0;

		/// @brief 再生する
		virtual void Play() = 0;

		/// @brief 停止する
		virtual void Stop() = 0;

		/// @brief パラメータを取得する
		/// @return 
		virtual void* GetParam() const = 0;

		/// @brief 再生されているかどうか
		/// @return 
		virtual bool IsPlay() const = 0;

		/// @brief 名前を取得する
		/// @return 名前
		std::string GetName()const { return name_; }

		/// @brief ハンドルを取得する
		/// @return 
		SoundHandle GetHandle() const { return hSound_; }

		/// @brief サウンドタイプを取得する
		/// @return 
		SoundType GetType() const { return type_; }


	public:

		/// @brief デバッグ用パラメータ
		virtual void DebugParameter() = 0;


	protected:

		/// @brief 名前
		std::string name_{};

		/// @brief ファイルパス
		std::string filePath_{};

		/// @brief サウンドハンドル
		SoundHandle hSound_ = 0;


	protected:

		/// @brief オーディオストア
		AudioStore* audioStore_ = nullptr;

		/// @brief パラメータ
		SoundParameter* parameter_ = nullptr;

		// サウンドタイプ
		SoundType type_;
	};
}