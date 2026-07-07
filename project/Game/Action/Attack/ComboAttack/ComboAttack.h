#pragma once
#include "../Attack.h"

struct CombAttackInitData
{
	AnimationHandle hAttackMotion = 0;
	float attackTime = 0.0f;
	float moveSpeed = 0.0f;
	float moveStartTime = 0.0f;
	float moveEndTime = 0.0f;
	float cancelStartTime = 0.0f;
	float cancelEndTime = 0.0f;

	// 当たり判定
	std::vector<HitboxDefinition> hitDefinitions;
};

class ComboAttack : public Attack
{
public:

	/// @brief コンストラクタ
	/// @param character 
	/// @param initData 
	ComboAttack(Character* character, const CombAttackInitData& initData);

	/// @brief 実行
	virtual void Exec() override;

	/// @brief 更新処理
	virtual void Update() override;

	/// @brief リセット
	virtual void Reset() override;

	/// @brief 終了、中断
	virtual void Exit() override;


	/// @brief コンボキャンセル可能かどうか
	/// @return 
	bool IsCancelable() const { return (attackTimer_ >= cancelStartTime_ && attackTimer_ <= cancelEndTime_); }

	/// @brief 次の攻撃に移行できるかどうか
	/// @return 
	bool IsCanNextCombo()const;


private:

	// コンボキャンセル受付時間
	float cancelStartTime_ = 0.0f;

	// コンボキャンセル終了時間
	float cancelEndTime_ = 0.0f;


private:

	std::vector<HitboxState> hitStates_;
};

