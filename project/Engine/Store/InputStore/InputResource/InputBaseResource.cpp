#include "InputBaseResource.h"
#include <cassert>

/// @brief 初期化
/// @param input 
void Engine::InputBaseResource::Initialize(Input* input)
{
	// nullptrチェック
	assert(input);

	// 引数を受け取る
	input_ = input;
}