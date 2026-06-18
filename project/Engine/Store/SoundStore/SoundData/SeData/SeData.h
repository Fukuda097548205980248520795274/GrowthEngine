#pragma once
#include "../SoundData.h"

namespace Engine
{
	// SEパラメータ
	struct SeParam
	{
		// 音量
		float volume = 0.5f;

		// ピッチ
		float pitch = 1.0f;

		// オーディオハンドル
		AudioHandle hAudio = 0;
	};

	class SeData : public SoundData
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		/// @param hSound 
		/// @param hAudio 
		SeData(const std::string& name, SoundHandle hSound)
			: SoundData(name, hSound)
		{
			type_ = SoundType::Se;
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
		void Stop() override {};

		/// @brief パラメータを取得する
		/// @return 
		void* GetParam() const override { return param_.get(); }

		/// @brief 再生されているかどうか
		/// @return 
		bool IsPlay() const override { return false; };


	public:

		/// @brief デバッグ用パラメータ
		void DebugParameter() override;


	protected:

		/// @brief パラメータ
		std::unique_ptr<SeParam> param_ = std::make_unique<SeParam>();
	};
}