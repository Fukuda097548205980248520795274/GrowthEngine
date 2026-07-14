#pragma once
#include "../Action.h"

/// @brief パリィの種類
enum class GuardType
{
	Guard, // ガード
	Deflect, // 受け流し
	Repel, // 弾き
};

class Defense : public Action
{
public:

	/// @brief コンストラクタ
	/// @param character 
	/// @param duration 
	Defense(Character* character , float duration) : Action(character), guardDuration_(duration) {}

	/// @brief 実行
	virtual void Exec() override;

	/// @brief 終了、中断
	virtual void Exit() override;

	/// @brief 使用中かどうか
	/// @return 
	virtual bool IsUse() const override { return IsExec(); }


private:

	// ガードの持続時間
	float guardDuration_ = 0.5f;
};

