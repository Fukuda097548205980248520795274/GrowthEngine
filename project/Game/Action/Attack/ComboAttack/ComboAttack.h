#pragma once
#include "../Attack.h"

class ComboAttack : public Attack
{
public:

	/// @brief コンストラクタ
	/// @param character 
	/// @param hAttackMotion 
	/// @param cancelStartTime 
	/// @param cancelEndTime 
	ComboAttack(Character* character, AnimationHandle hAttackMotion, float cancelStartTime, float cancelEndTime) 
		: Attack(character), hAttackMotion_(hAttackMotion), cancelStartTime_(cancelStartTime), cancelEndTime_(cancelEndTime) {}

	/// @brief 実行
	virtual void Exec() override;

	/// @brief 更新処理
	virtual void Update() override;

	/// @brief 次の攻撃があるかどうか
	/// @return 
	bool HasNextAttack(AttackInputType inputType) const override;

	/// @brief 次のライト攻撃を設定する
	/// @param nextLightAttack 
	void SetNextLightAttack(ComboAttack* nextLightAttack) { nextLightAttack_ = nextLightAttack; }

	/// @brief 次のヘビー攻撃を設定する
	/// @param nextHeavyAttack 
	void SetNextHeavyAttack(ComboAttack* nextHeavyAttack) { nextHeavyAttack_ = nextHeavyAttack; }


private:

	/// @brief 攻撃モーション
	AnimationHandle hAttackMotion_ = 0;

	// コンボキャンセル受付時間
	float cancelStartTime_ = 0.0f;

	// コンボキャンセル終了時間
	float cancelEndTime_ = 0.0f;

	/// @brief 次のライト攻撃
	ComboAttack* nextLightAttack_ = nullptr;

	/// @brief 次のヘビー攻撃
	ComboAttack* nextHeavyAttack_ = nullptr;
};

