#pragma once
#include "Handle/Handle.h"

namespace Engine
{
	struct SoundParam;
}

class GrowthEngine;

class Sound
{
public:

	/// @brief コンストラクタ
	Sound(SoundHandle hSound);

	/// @brief 再生
	void Play()const;

	/// @brief 停止
	void Stop()const;

	/// @brief パラメータ
	Engine::SoundParam* param_ = nullptr;


private:

	/// @brief エンジンのインスタンス
	const GrowthEngine* engine_ = nullptr;

	/// @brief サウンドハンドル
	SoundHandle hSound_ = 0;
};

