#pragma once
#include "../CompositeNode.h"

class PersistentSelectorNode : public CompositeNode
{
public:

	/// @brief コンストラクタ
	PersistentSelectorNode() : CompositeNode() {}

	/// @brief 実行
	/// @return 
	State Exec() override;


private:

	/// @brief 今のインデックス
	int32_t currentIndex_ = 0;
};

