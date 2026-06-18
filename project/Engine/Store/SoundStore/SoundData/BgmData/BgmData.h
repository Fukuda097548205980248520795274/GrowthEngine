#pragma once
#include "../SoundData.h"

namespace Engine
{
	// BGMパラメータ
	struct BgmParam
	{
		// 音量
		float volume = 0.5f;

		// ピッチ
		float pitch = 1.0f;

		// ループの有効化
		bool enableLoop = false;

		// オーディオハンドル
		AudioHandle hAudio = 0;
	};

	class BgmData : public SoundData
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		/// @param hSound 
		/// @param hAudio 
		BgmData(const std::string& name, SoundHandle hSound)
			: SoundData(name, hSound) 
		{
			type_ = SoundType::Bgm;
		}

		/// @brief 初期化
		/// @param audioStore 
		/// @param parameter 
		void Initialize(AudioStore* audioStore, SoundParameter* parameter, AudioHandle hAudio) override;

		/// @brief リセット
		void Reset() override;

		/// @brief 更新処理
		void Update() override;

		/// @brief 再生する
		void Play() override;

		/// @brief 停止する
		void Stop() override;

		/// @brief パラメータを取得する
		/// @return 
		void* GetParam() const override { return param_.get(); }

		/// @brief 再生されているかどうか
		/// @return 
		bool IsPlay() const override;


	public:

		/// @brief デバッグ用パラメータ
		void DebugParameter() override;


	protected:

		/// @brief プレイハンドル
		PlayHandle hPlay_ = 0;

		/// @brief パラメータ
		std::unique_ptr<BgmParam> param_ = std::make_unique<BgmParam>();


	protected:

		// 前回の音量
		float preVolume_ = 0.5f;

		// 前回のピッチ
		float prePitch_ = 1.0f;

		// サウンド内での再生フラグ
		bool isPlay_ = false;
	};
}