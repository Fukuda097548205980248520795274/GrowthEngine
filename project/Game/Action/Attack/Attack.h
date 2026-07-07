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
	Down,

	// 受け流され
	Deflected,

	// 弾かれ
	Repelled,
};

enum class DamageReactionState
{
	// 怯みなし
	None,

	// 小怯み
	LightStaggerFront,
	LightStaggerBack,
	LightStaggerLeft,
	LightStaggerRight,

	// 大怯み
	HeavyStaggerFront,
	HeavyStaggerBack,
	HeavyStaggerLeft,
	HeavyStaggerRight,

	// 倒れこみ
	DownFallingFront,
	DownFallingBack,
	DownFallingLeft,
	DownFallingRight,

	// ダウン中
	DownLyingFront,
	DownLyingBack,

	// 起き上がり
	DownGettingUpFront,
	DownGettingUpBack,

	// ダウン中の怯み
	DownStaggerFront,
	DownStaggerBack,

	// 吹き飛び
	BlownAwayFront,
	BlownAwayBack,

	// 落下
	BlownFallingFront,
	BlownFallingBack,

	// 受け流され
	Deflected,

	// 弾かれ
	Repelled,
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