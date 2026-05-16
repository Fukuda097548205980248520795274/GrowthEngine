#pragma once
#include "../Attack.h"

struct SpinThrowAttackInitData
{
	AnimationHandle hAttackMotion = 0;
	float attackTime = 0.0f;
	float moveSpeed = 0.0f;
	float moveStartTime = 0.0f;
	float moveEndTime = 0.0f;
	JointType jointType = JointType::HandR;
	float hitboxStartTime = 0.0f;
	float hitboxEndTime = 0.0f;
	int32_t damage = 10;
	DamageReaction damageReaction = DamageReaction::LightStagger;
	float knockback = 0.0f;
	Vector3 knockbackDirection = Vector3(0.0f, 0.0f, 1.0f);
};

/// @brief スピン投げ攻撃
class SpinThrowAttack : public Attack
{
public:

	/// @brief コンストラクタ
	/// @param character 
	/// @param initData 
	SpinThrowAttack(Character* character, const SpinThrowAttackInitData& initData);

	/// @brief 実行
	virtual void Exec() override;

	/// @brief 更新処理
	virtual void Update() override;

	/// @brief リセット
	virtual void Reset() override;

	/// @brief 終了、中断
	virtual void Exit() override;

	/// @brief 外部から移動・投擲用の入力方向を設定する（PlayerやBehaviorTreeから毎フレーム呼ぶ想定）
	/// @param inputDir 入力方向ベクトル
	void SetInputDirection(const Vector3& inputDir)
	{
		// 入力がある（ゼロベクトルではない）場合のみ方向を更新
		// 入力がない場合は、最後に設定された方向（今向いている方向）を維持する
		if (inputDir.Length() > 0.001f)
		{
			throwDirection_ = inputDir.Normalize();
		}
	}


private:

	// 状態フラグ
	bool isSpinning_ = false; // 回転中かどうか（掴みが成功したか）
	bool hasThrown_ = false;  // 投げ終わったかどうか

	// 掴んでいるターゲット
	Character* grabbedTarget_ = nullptr;

	// 攻撃用の方向変数（移動方向 兼 投げる方向）
	Vector3 throwDirection_ = Vector3(0.0f, 0.0f, 1.0f);
};

