#pragma once
#include "../Action.h"

class RequestToken : public Action
{
public:

	/// @brief コンストラクタ
	/// @param character 
	/// @param avoidDirection 
	RequestToken(Character* character) : Action(character) {}

	/// @brief 実行
	void Exec() override;

	/// @brief 更新処理
	void Update() override;

	/// @brief 使用中かどうか
	/// @return 
	bool IsUse() const override;

	/// @brief トークンが要求されたかどうか
	/// @return 
	bool IsRequested() const { return isRequested_; }


private:

	/// @brief トークンが要求されたかどうか
	bool isRequested_ = false;
};

