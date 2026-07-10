#pragma once

class Character;

class CharacterState
{
public:

	/// @brief 仮想デストラクタ
	virtual ~CharacterState() = default;

	/// @brief コンストラクタ
	/// @param owner 
	CharacterState(Character* owner) : owner_(owner) {}

	/// @brief この状態に入るときに呼ばれる処理
	virtual void Enter(){}

	/// @brief 更新処理
	/// @param dt 
	virtual void Update(float dt){}

	/// @brief この状態からでるときに呼ばれる処理
	virtual void Exit(){}


protected:

	// 所有者
	Character* owner_ = nullptr;
};

