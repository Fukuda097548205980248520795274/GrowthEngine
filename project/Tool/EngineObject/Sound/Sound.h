#pragma once
#include "Handle/Handle.h"
#include <string>

class GrowthEngine;

namespace Engine
{
	class Sound
	{
	public:

		/// @brief コンストラクタ
		Sound() = default;

		/// @brief デストラクタ
		virtual ~Sound() = default;

		/// @brief 再生
		virtual void Play()const = 0;


	private:

		/// @brief エンジンのインスタンス
		const GrowthEngine* engine_ = nullptr;

		/// @brief サウンドハンドル
		SoundHandle hSound_ = 0;
	};
}