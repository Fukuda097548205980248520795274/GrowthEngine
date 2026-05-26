#include "ReleaseTokenNode.h"
#include "BattleDirector/BattleDirector.h"

Node::State ReleaseTokenNode::Exec()
{
	// アクションがない場合は失敗
	ReleaseToken* token = GetReleaseToken();
	if (!token)return State::Failure;

	// 実行と更新
	token->Exec();

	return State::Success;
}