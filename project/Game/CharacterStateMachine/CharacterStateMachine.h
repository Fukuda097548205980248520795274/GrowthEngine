#pragma once
#include <memory>
#include <unordered_map>
#include <string>
#include "CharacterState/CharacterState.h"

class CharacterStateMachine
{
public:

	/// @brief コンストラクタ
	CharacterStateMachine() = default;

	/// @brief デストラクタ
	~CharacterStateMachine() = default;

	/// @brief 状態のマップ
	/// @param name 
	/// @param state 
	void AddState(const std::string& name, std::unique_ptr<CharacterState> state) { states_[name] = std::move(state); }

	/// @brief 状態を変更する
	/// @param name 
	void ChangeState(const std::string& name);

	/// @brief 更新処理
	/// @param dt 
	void Update(float dt);

	/// @brief 現在の状態の名前を取得する
	/// @return 
	const std::string& GetCurrentStateName() const { return currentStateName_; }

	/// @brief 現在の状態を取得する
	/// @return 
	CharacterState* GetCurrentState() const { return currentState_; }

	/// @brief 状態を取得する
	/// @param name 
	/// @return 
	CharacterState* GetState(const std::string& name) const { return states_.at(name).get(); }

	/// @brief 状態が変更されたかどうかを取得する
	/// @return 
	bool IsChangeState() const { return isChangeState_ || isPrevChangeState_; }


private:

	/// @brief 状態のマップ
	std::unordered_map<std::string, std::unique_ptr<CharacterState>> states_;

	/// @brief 現在の状態
	CharacterState* currentState_ = nullptr;

	/// @brief 現在の状態の名前
	std::string currentStateName_{};

	/// @brief 状態が変更されたかどうか
	bool isChangeState_ = false;
	bool isPrevChangeState_ = false;
};

