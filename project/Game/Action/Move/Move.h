#pragma once
#include "../Action.h"

class Move : public Action
{
public:

	/// @brief コンストラクタ
	/// @param character 
	Move(Character* character) : Action(character) {}

	/// @brief 実行
	virtual void Exec() override;

	/// @brief 更新処理
	virtual void Update() override;

	/// @brief 使用中かどうか
	/// @return 
	bool IsUse() const override;
};

