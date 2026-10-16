#pragma once
#include "../CompositeNode.h"

/// @brief 再起動シーケンスノード
class RestartingSequenceNode : public CompositeNode
{
public:

	/// @brief コンストラクタ
	RestartingSequenceNode() : CompositeNode() {}

	/// @brief 実行
	/// @return 
	State Exec() override;
};

