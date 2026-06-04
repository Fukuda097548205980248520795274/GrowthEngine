#pragma once
#include "../Action.h"

class Move : public Action
{
public:

	/// @brief コンストラクタ
	/// @param character 
	Move(Character* character, bool isDash = false) : Action(character), isDash_(isDash) {}

	/// @brief 実行
	virtual void Exec() override;

	/// @brief 更新処理
	virtual void Update() override;

	/// @brief 中断・終了
	virtual void Exit() override;

	/// @brief 使用中かどうか
	/// @return 
	bool IsUse() const override;

	/// @brief 走るかどうか
	/// @return 
	bool IsDash() const { return isDash_; }


protected:

	/// @brief ダッシュするかどうか
	bool isDash_ = false;
};

