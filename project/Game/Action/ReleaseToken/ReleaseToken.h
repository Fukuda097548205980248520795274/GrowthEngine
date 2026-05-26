#pragma once
#include "../Action.h"

class ReleaseToken : public Action
{
public:

	/// @brief コンストラクタ
	/// @param character 
	/// @param avoidDirection 
	ReleaseToken(Character* character) : Action(character) {}

	/// @brief 実行
	void Exec() override;

	/// @brief 更新処理
	void Update() override;

	/// @brief 使用中かどうか
	/// @return 
	bool IsUse() const override;
};

