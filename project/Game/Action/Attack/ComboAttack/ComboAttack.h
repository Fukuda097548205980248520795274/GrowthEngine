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
	std::string partName{};
	float hitboxStartTime = 0.0f;
	float hitboxEndTime = 0.0f;
	int32_t damage = 10;
	float staggerTime = 0.3f;
	float knockback = 0.0f;
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

	/// @brief 次の攻撃があるかどうか
	/// @return 
	bool HasNextAttack(AttackInputType inputType) const override;

	/// @brief 次のライト攻撃を設定する
	/// @param nextLightAttack 
	void SetNextLightAttack(ComboAttack* nextLightAttack) { nextLightAttack_ = nextLightAttack; }

	/// @brief 次のヘビー攻撃を設定する
	/// @param nextHeavyAttack 
	void SetNextHeavyAttack(ComboAttack* nextHeavyAttack) { nextHeavyAttack_ = nextHeavyAttack; }

	/// @brief コンボキャンセル可能かどうか
	/// @return 
	bool IsCancelable() const { return (attackTimer_ >= cancelStartTime_ && attackTimer_ <= cancelEndTime_); }

	/// @brief 次の攻撃に移行できるかどうか
	/// @return 
	bool IsCanNextCombo()const { return !IsExec() || (attackTimer_ >= cancelStartTime_ && attackTimer_ <= cancelEndTime_); }


private:

	// コンボキャンセル受付時間
	float cancelStartTime_ = 0.0f;

	// コンボキャンセル終了時間
	float cancelEndTime_ = 0.0f;


private:

	/// @brief 次のライト攻撃
	ComboAttack* nextLightAttack_ = nullptr;

	/// @brief 次のヘビー攻撃
	ComboAttack* nextHeavyAttack_ = nullptr;
};

