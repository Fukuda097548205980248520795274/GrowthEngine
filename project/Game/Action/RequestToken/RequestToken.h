#pragma once
#include "../Action.h"
#include "BattleDirector/BattleDirector.h"

class RequestToken : public Action
{
public:

	/// @brief コンストラクタ
	/// @param character 
	/// @param avoidDirection 
	RequestToken(Character* character, ActionTokenType tokenType) : Action(character), tokenType_(tokenType) {}

	/// @brief 実行
	void Exec() override;

	/// @brief 更新処理 ここでは特に何もしない
	void Update() override{}

	/// @brief 終了、中断
	void Exit() override;

	/// @brief リセット
	void Reset() override;

	/// @brief 使用中かどうか ここでは常に使用していないとみなす
	/// @return 
	bool IsUse() const override { return false; }


private:

	/// @brief トークンの種類
	ActionTokenType tokenType_ = ActionTokenType::Attack;
};

