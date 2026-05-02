#pragma once
#include "../Action.h"

class Guard : public Action
{
public:

	/// @brief コンストラクタ
	/// @param character 
	/// @param duration 
	Guard(Character* character , float duration) : Action(character), guardDuration_(duration) {}

	/// @brief 実行
	virtual void Exec() override;

	/// @brief 更新処理
	virtual void Update() override;

	/// @brief 終了、中断
	virtual void Exit() override;

	/// @brief 使用中かどうか
	/// @return 
	virtual bool IsUse() const override { return IsExec(); }


private:

	// ガードの持続時間
	float guardDuration_ = 0.5f;

	// ガードの経過時間
	float guardTimer_ = 0.0f;
};

