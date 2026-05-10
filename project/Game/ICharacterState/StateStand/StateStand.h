#pragma once
#include "../ICharacterState.h"

class StateStand : public ICharacterState
{
public:

	/// @brief 状態に入るときの処理
	/// @param character 
	void Enter(Character* character) override;

	/// @brief 更新処理
	/// @param character 
	void Update(Character* character) override;

	/// @brief 状態から出るときの処理
	/// @param character 
	void Exit(Character* character) override;
};

