#include "CharacterStateNone.h"

/// @brief この状態に入るときに呼ばれる処理
void CharacterStateNone::Enter()
{
	// ツリーのリクエストを行う
	TreeRequest();
}