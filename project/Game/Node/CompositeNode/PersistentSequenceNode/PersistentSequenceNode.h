#pragma once
#include "../CompositeNode.h"

class PersistentSequenceNode : public CompositeNode
{
public:

	/// @brief コンストラクタ
	PersistentSequenceNode() : CompositeNode() {}

	/// @brief 実行
	/// @return 
	State Exec() override;


private:

	/// @brief 今のインデックス
	int32_t currentIndex_ = 0;
};

