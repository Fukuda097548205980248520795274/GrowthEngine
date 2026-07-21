#include "CharacterStateMachine.h"

/// @brief 状態を変更する
/// @param name 
void CharacterStateMachine::ChangeState(const std::string& name)
{
	// すでに現在の状態と同じ場合は、ビヘイビアツリーのリクエストのみを行う
	if (currentStateName_ == name)
	{
		currentState_->TreeRequest();
		return;
	}

	// 現在の状態を終了する
	if (currentState_)currentState_->Exit();

	auto it = states_.find(name);
	if (it != states_.end())
	{
		// 新しい状態に変更する
		currentState_ = it->second.get();
		currentStateName_ = name;

		// 新しい状態に入る
		currentState_->Enter();

		// 状態が変更されたことを記録する
		isChangeState_ = true;
	}
}

/// @brief 更新処理
/// @param dt 
void CharacterStateMachine::Update(float dt)
{
	// 前フレームの状態変更フラグを更新する
	isPrevChangeState_ = isChangeState_;
	isChangeState_ = false;

	if (currentState_)currentState_->Update(dt);
}