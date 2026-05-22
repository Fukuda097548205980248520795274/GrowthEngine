#pragma once
#include "GrowthEngine.h"

class SoundManager
{
public:




private:

	/// @brief 旋嵐スタイルのBGM
	std::unique_ptr<Sound> bgmStyleSenran_ = nullptr;

	/// @brief 撃鉄スタイルのBGM
	std::unique_ptr<Sound> bgmStyleGekitetu_ = nullptr;
};

