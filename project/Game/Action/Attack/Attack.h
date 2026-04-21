#pragma once
#include "../Action.h"

class Attack : public Action
{
public:

	/// @brief コンストラクタ
	/// @param character 
	Attack(Character* character) : Action(character) {}

	/// @brief 実行
	virtual void Exec() override;

	/// @brief 更新処理
	virtual void Update() override;
};

