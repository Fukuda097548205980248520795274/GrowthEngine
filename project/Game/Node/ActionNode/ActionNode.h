#pragma once
#include "../Node.h"
#include "Action/Action.h"

class ActionNode : public Node
{
public:

	/// @brief コンストラクタ
	/// @param action
	ActionNode(std::unique_ptr<Action> action) : action_(std::move(action)), Node(){}

	/// @brief 実行
	/// @return 
	State Exec() override;

	/// @brief 中断処理
	virtual void Abort() override;

	/// @brief 所有者を設定する
	/// @param owner 
	void SetOwner(Character* owner) override { action_->SetOwner(owner); }


protected:

#ifdef DEVELOPMENT

	/// @brief カスタムノードUIを描画する
	/// @param zoom 
	void DrawCustomNodeUI(float zoom) override;

#endif


protected:

	/// @brief アクション
	std::unique_ptr<Action> action_ = nullptr;
};

