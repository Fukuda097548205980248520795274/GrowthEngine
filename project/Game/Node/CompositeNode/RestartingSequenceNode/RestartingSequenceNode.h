#pragma once
#include "../CompositeNode.h"

class RestartingSequenceNode : public CompositeNode
{
public:

	/// @brief コンストラクタ
	RestartingSequenceNode() : CompositeNode() {}

	/// @brief 実行
	/// @return 
	State Exec() override;
};

