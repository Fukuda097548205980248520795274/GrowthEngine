#pragma once
#include "../CompositeNode.h"

/// @brief 再帰度セレクターノード
class RestartingSelectorNode : public CompositeNode
{
public:

	/// @brief コンストラクタ
	RestartingSelectorNode() : CompositeNode() {}

	/// @brief 実行
	/// @return 
	State Exec() override;
};

