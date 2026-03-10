#include "Context.h"

/// @brief コンストラクタ
Context::Context()
{
	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

	// キー入力 : プレイヤー左移動
	keyInputPlayerLeftMove_ = std::make_unique<InputKey>("PlayerLeftMove", InputState::Press, DIK_A);

	// キー入力 : プレイヤー右移動
	keyInputPlayerRightMove_ = std::make_unique<InputKey>("PlayerRightMove", InputState::Press, DIK_D);
}