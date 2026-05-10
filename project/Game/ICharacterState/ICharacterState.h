#pragma once

class Character;

class ICharacterState
{
public:

	/// @brief 仮想デストラクタ
	virtual ~ICharacterState() = default;

	/// @brief 状態に入るときの処理
	/// @param character 
	virtual void Enter(Character* character) = 0;

	/// @brief 更新処理
	/// @param character 
	virtual void Update(Character* character) = 0;

	/// @brief 状態から出るときの処理
	/// @param character 
	virtual void Exit(Character* character) = 0;
};