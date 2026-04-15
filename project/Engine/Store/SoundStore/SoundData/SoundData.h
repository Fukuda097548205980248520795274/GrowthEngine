#include "Handle/Handle.h"
#include <string>
#include <memory>

namespace Engine
{
	class AudioStore;
	class SoundParameter;

	// サウンドパラメータ
	struct SoundParam
	{
		// 音量
		float volume = 0.5f;

		// ピッチ
		float pitch = 1.0f;

		// ループの有効化
		bool enableLoop = false;
	};

	// サウンドデータ
	class SoundData
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		/// @param hSound 
		/// @param hAudio 
		SoundData(const std::string& name, SoundHandle hSound, AudioHandle hAudio)
			: name_(name), hSound_(hSound), hAudio_(hAudio) {
		}

		/// @brief 初期化
		/// @param audioStore 
		/// @param parameter 
		void Initialize(AudioStore* audioStore, SoundParameter* parameter);

		/// @brief リセット
		void Reset();

		/// @brief 更新処理
		void Update();

		/// @brief 再生する
		void Play();

		/// @brief 停止する
		void Stop();

		/// @brief パラメータを取得する
		/// @return 
		SoundParam* GetParam() { return param_.get(); }

		/// @brief 名前を取得する
		/// @return 名前
		std::string GetName()const { return name_; }

		/// @brief ハンドルを取得する
		/// @return 
		SoundHandle GetHandle() const { return hSound_; }

		/// @brief 再生されているかどうか
		/// @return 
		bool IsPlay() const;


	public:

		/// @brief デバッグ用パラメータ
		void DebugParameter();


	private:

		/// @brief 名前
		std::string name_{};

		/// @brief ファイルパス
		std::string filePath_{};

		/// @brief サウンドハンドル
		SoundHandle hSound_ = 0;

		/// @brief オーディオハンドル
		AudioHandle hAudio_ = 0;

		/// @brief プレイハンドル
		PlayHandle hPlay_ = 0;

		/// @brief パラメータ
		std::unique_ptr<SoundParam> param_ = std::make_unique<SoundParam>();


	private:

		// 前回の音量
		float preVolume_ = 0.5f;

		// 前回のピッチ
		float prePitch_ = 1.0f;

		// サウンド内での再生フラグ
		bool isPlay_ = false;


	private:

		/// @brief オーディオストア
		AudioStore* audioStore_ = nullptr;

		/// @brief パラメータ
		SoundParameter* parameter_ = nullptr;
	};
}