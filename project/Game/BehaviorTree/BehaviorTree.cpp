#include "BehaviorTree.h"

/// @brief 実行
void BehaviorTree::Exec()
{
#ifdef _DEVELOPMENT
	root_->ResetStatusRecursive();
#endif

	if (root_) root_->UpdateNode();
}