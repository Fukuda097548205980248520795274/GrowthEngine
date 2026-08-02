#pragma once
#include "../Action.h"
#include "AppCollider/AppCollider.h"
#include "MotionManager/MotionManager.h"

/// @brief 攻撃入力の種類
enum class AttackInputType
{
	None,
	InputX,
	InputY,
	InputB
};

// 攻撃の種類（コンボ、スピン投げ、掴みなど）を表す列挙型
enum class AttackType
{
	None,
	Combo,
	Grab,
	GrabStrike
};

enum class DamageReaction
{
	// 怯みなし
	None,

	// 小怯み
	LightStagger,

	// 大怯み
	HeavyStagger,
	
	// ダウン
	Down
};

class Attack : public Action
{
public:

	/// @brief コンストラクタ
	/// @param character 
	Attack(Character* character) : Action(character) {}

	/// @brief 実行
	virtual void Exec() override;

	/// @brief 中断・終了
	virtual void Exit() override;

	/// @brief リセット
	virtual void Reset() override;

	/// @brief 更新処理
	virtual void Update() override;

	/// @brief 攻撃中かどうか
	/// @return 
	bool IsUse() const override;

	/// @brief 攻撃の種類を取得する
	/// @return 
	AttackType GetType() const { return attackType_; }


	/// @brief 次の攻撃があるかどうか
	/// @param inputType 
	/// @return 
	virtual bool HasNextAttack(AttackInputType inputType) const;

	
	/// @brief 次の攻撃を設定する
	/// @param next 
	void SetNextInputXAttack(Attack* next) { nextInputXAttack_ = next; }

	/// @brief 次の攻撃を設定する
	/// @param next 
	void SetNextInputYAttack(Attack* next) { nextInputYAttack_ = next; }

	/// @brief 次の攻撃を設定する
	/// @param next 
	void SetNextInputBAttack(Attack* next) { nextInputBAttack_ = next; }

	
	/// @brief 次の攻撃を取得する
	/// @return 
	Attack* GetNextInputXAttack() const { return nextInputXAttack_; }

	/// @brief 次の攻撃を取得する
	/// @return 
	Attack* GetNextInputYAttack() const { return nextInputYAttack_; }

	/// @brief 次の攻撃を取得する
	/// @return 
	Attack* GetNextInputBAttack() const { return nextInputBAttack_; }

	/// @brief 攻撃キャンセル可能かどうか
	/// @return 
	virtual bool IsCancelable() const { return false; }

	/// @brief 時間が攻撃が終了したかどうか
	/// @return 
	bool IsFinishedTimer() const { return attackTimer_ >= attackTime_; }

	/// @brief 武器をつかむかどうか
	/// @return 
	virtual bool IsGrabWeapon()const { return false; }


protected:

	// 攻撃モーションのハンドル
	AnimationHandle hAttackMotion_ = 0;


protected:

	/// @brief 攻撃時間
	float attackTime_ = 0.0f;

	/// @brief 攻撃タイマー
	float attackTimer_ = 0.0f;


protected:

	/// @brief 移動速度
	float moveSpeed_ = 0.0f;

	/// @brief 現在の移動速度
	float currentMoveSpeed_ = 0.0f;

	/// @brief 移動開始時間（攻撃開始からの遅延時間）
	float moveStartTime_ = 0.0f;

	/// @brief 移動終了時間
	float moveEndTime_ = 0.0f;

	/// @brief 攻撃の種類
	AttackType attackType_ = AttackType::None;


protected:

	// 派生先のポインタを基底クラスで保持する
	Attack* nextInputXAttack_ = nullptr;
	Attack* nextInputYAttack_ = nullptr;
	Attack* nextInputBAttack_ = nullptr;
};



struct HitboxDefinition
{
	JointType jointType = JointType::HandR;
	float startTime = 0.0f;
	float endTime = 0.0f;
	int32_t damage = 10;
	DamageReaction damageReaction = DamageReaction::LightStagger;
	float knockback = 0.0f;
	Vector3 knockbackDirection = Vector3(0.0f, 0.0f, 1.0f);
	float radius = 0.25f;
};

struct HitboxState
{
	HitboxDefinition def;
	AppCollider hitbox;

	/// @brief ヒットしたキャラクターのリスト
	std::vector<Character*> hitCharacters;

	// この判定を削除するヘルパー関数
	void DeleteHitbox()
	{
		if (hitbox.collider_ != nullptr)
		{
			hitbox.collider_->Delete();
			hitbox.collider_ = nullptr;
		}
	}
};


struct HitDefinition
{
	// ダメージ量
	int32_t damage = 10;

	// ヒットする時間（攻撃開始からの遅延時間）
	float hitTime = 0.0f;

	// 相手に当たるジョイント
	JointType hitJoint = JointType::None;
};