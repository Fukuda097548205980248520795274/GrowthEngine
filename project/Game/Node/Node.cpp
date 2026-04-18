#include "Node.h"

/// @brief コンストラクタ
Node::Node()
{
	// インスタンスを取得する
	engine_ = GrowthEngine::GetInstance();
}