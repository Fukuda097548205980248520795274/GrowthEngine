#pragma once
#include "GrowthEngine.h"

class Context
{
public:

	/// @brief コンストラクタ
	Context();

	/// @brief キー入力 : プレイヤー左移動
	std::unique_ptr<InputKey> keyInputPlayerLeftMove_ = nullptr;

	/// @brief キー入力 : プレイヤー右移動
	std::unique_ptr<InputKey> keyInputPlayerRightMove_ = nullptr;

private:

	/// @brief エンジン
	const GrowthEngine* engine_ = nullptr;
};

