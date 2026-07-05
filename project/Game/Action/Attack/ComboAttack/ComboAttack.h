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

	/// @brief 次の攻撃があるかどうか
	/// @return 
	bool HasNextAttack(AttackInputType inputType) const override;


	/// @brief 次のX入力攻撃を設定する
	/// @param nextInputXAttack 
	void SetNextInputXAttack(Attack* nextInputXAttack) { nextInputXAttack_ = nextInputXAttack; }

	/// @brief 次のY入力攻撃を設定する
	/// @param nextHeavyAttack 
	void SetNextInputYAttack(Attack* nextInputYAttack) { nextInputYAttack_ = nextInputYAttack; }

	/// @brief 次のB入力攻撃を設定する
	/// @param nextInputBAttack 
	void SetNextInputBAttack(Attack* nextInputBAttack) { nextInputBAttack_ = nextInputBAttack; }

	/// @brief 次のX入力攻撃を取得する
	/// @return 
	Attack* GetNextInputXAttack() const { return nextInputXAttack_; }

	/// @brief 次のY入力攻撃を取得する
	/// @return 
	Attack* GetNextInputYAttack() const { return nextInputYAttack_; }

	/// @brief 次のB攻撃を取得する
	/// @return 
	Attack* GetNextInputBAttack() const { return nextInputBAttack_; }


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

	/// @brief 次のX入力攻撃
	Attack* nextInputXAttack_ = nullptr;

	/// @brief 次のY入力攻撃
	Attack* nextInputYAttack_ = nullptr;

	/// @brief 次のB入力攻撃
	Attack* nextInputBAttack_ = nullptr;


private:

	std::vector<HitboxState> hitStates_;
};

