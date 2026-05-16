#pragma once
#include "../Action.h"
#include "AppCollider/AppCollider.h"
#include "MotionManager/MotionManager.h"

/// @brief 攻撃入力の種類
enum class AttackInputType
{
	None,
	Light,
	Heavy
};

// 攻撃の種類（コンボ、スピン投げ、掴みなど）を表す列挙型
enum class AttackType
{
	None,
	Combo,
	Grab
};

enum class DamageReaction
{
	None, // ダメージなし
	LightStagger, // 軽い怯み
	HeavyStagger, // 重い怯み
	
	// ダウン
	DownFalling,

	// ダウン中
	DownLying,

	// 立ち上がり
	DownGettingUp
};

class Attack : public Action
{
public:

	/// @brief コンストラクタ
	/// @param character 
	Attack(Character* character) : Action(character) {}

	/// @brief 実行
	virtual void Exec() override;

	/// @brief 更新処理
	virtual void Update() override;

	/// @brief 次の攻撃があるかどうか
	/// @return 
	virtual bool HasNextAttack(AttackInputType inputType) const { return false; }

	/// @brief 攻撃中かどうか
	/// @return 
	bool IsUse() const override;

	/// @brief 攻撃の種類を取得する
	/// @return 
	AttackType GetType() const { return attackType_; }


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
	bool hasHit = false;

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