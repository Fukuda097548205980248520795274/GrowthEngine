#include "CompositeNode.h"

/// @brief 中断処理
void CompositeNode::Abort()
{
	for (auto& child : children_)
		child->Abort();
}