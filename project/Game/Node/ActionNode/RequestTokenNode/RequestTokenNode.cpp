#include "RequestTokenNode.h"
#include "BattleDirector/BattleDirector.h"

Node::State RequestTokenNode::Exec()
{
	// アクションがない場合は失敗
	RequestToken* token = GetRequestToken();
	if (!token)return State::Failure;

	// 実行と更新
	token->Exec();

	// アクションの状態に応じてノードの状態を返す
	if (token->IsRequested())
	{
		return State::Success;
	}
	else
	{
		return State::Failure;
	}
}