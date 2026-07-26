#pragma once
#include "../Sound.h"
#include "Store/SoundStore/SoundData/SeData/SeData.h"

class Se : public Engine::Sound
{
public:

	/// @brief コンストラクタ
	Se(const std::string& name, AudioHandle hAudio);

	/// @brief 再生
	void Play()const override;

	/// @brief 停止
	void Stop()const;

	/// @brief 再生されているかどうか
	/// @return 
	bool IsPlaying()const;

	/// @brief パラメータ
	Engine::SeParam* param_ = nullptr;


private:

	/// @brief エンジンのインスタンス
	const GrowthEngine* engine_ = nullptr;

	/// @brief サウンドハンドル
	SoundHandle hSound_ = 0;
};

