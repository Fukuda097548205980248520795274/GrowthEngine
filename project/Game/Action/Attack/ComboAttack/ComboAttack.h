#pragma once
#include "../Attack.h"
#include "AppCollider/AppCollider.h"

class ComboAttack : public Attack
{
public:

	/// @brief コンストラクタ
	/// @param character 
	/// @param hAttackMotion 
	/// @param attackTime 
	/// @param moveSpeed 
	/// @param moveAcceleration 
	/// @param moveDuration 
	/// @param moveDelay 
	/// @param cancelStartTime 
	/// @param cancelEndTime 
	ComboAttack(Character* character, AnimationHandle hAttackMotion,float attackTime,
		float moveSpeed,float moveStartTime, float moveEndTime, float cancelStartTime, float cancelEndTime , float hitboxStartTime, float hitboxEndTime)
		: Attack(character), hAttackMotion_(hAttackMotion), attackTime_(attackTime), moveSpeed_(moveSpeed),
		moveStartTime_(moveStartTime), moveEndTime_(moveEndTime), cancelStartTime_(cancelStartTime), cancelEndTime_(cancelEndTime),
		hitboxStartTime_(hitboxStartTime), hitboxEndTime_(hitboxEndTime) {}

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

	/// @brief 攻撃モーション
	AnimationHandle hAttackMotion_ = 0;


private:

	// コンボキャンセル受付時間
	float cancelStartTime_ = 0.0f;

	// コンボキャンセル終了時間
	float cancelEndTime_ = 0.0f;


private:

	/// @brief 攻撃時間
	float attackTime_ = 0.0f;

	/// @brief 攻撃タイマー
	float attackTimer_ = 0.0f;


private:

	/// @brief 移動速度
	float moveSpeed_ = 0.0f;

	/// @brief 移動開始時間（攻撃開始からの遅延時間）
	float moveStartTime_ = 0.0f;

	/// @brief 移動終了時間
	float moveEndTime_ = 0.0f;


private:

	/// @brief 次のライト攻撃
	ComboAttack* nextLightAttack_ = nullptr;

	/// @brief 次のヘビー攻撃
	ComboAttack* nextHeavyAttack_ = nullptr;


private:

	/// @brief 攻撃判定を削除する
	void DeleteHitbox();

	/// @brief 攻撃判定
	AppCollider hitbox_;

	/// @brief 攻撃判定開始時間
	float hitboxStartTime_ = 0.0f;

	/// @brief 攻撃判定終了時間
	float hitboxEndTime_ = 0.0f;

	/// @brief 攻撃判定がヒットしたかどうか
	bool hasHit_ = false;
};

