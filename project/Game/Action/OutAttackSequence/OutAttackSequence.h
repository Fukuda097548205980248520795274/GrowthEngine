#pragma once
#include "../Action.h"

class OutAttackSequence : public Action
{
public:

	/// @brief コンストラクタ
	/// @param character 
	OutAttackSequence(Character* character) : Action(character) {}

	/// @brief 実行
	void Exec() override;

	/// @brief 終了、中断
	void Exit() override;

	/// @brief 更新処理 ここでは特に何もしない
	void Update() override {}

	/// @brief リセット
	void Reset() override;

	/// @brief 使用中かどうか ここでは常に使用していないとみなす
	/// @return 
	bool IsUse() const override { return false; }
};