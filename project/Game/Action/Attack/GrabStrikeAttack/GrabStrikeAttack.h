#pragma once
#include "../Attack.h"

/// @brief 掴み攻撃の初期化データ
struct GrabStrikeAttackInitData
{
	// 攻撃時間
	float attackTime = 0.0f;

	// 移動
	float moveSpeed = 0.0f;
	float moveStartTime = 0.0f;
	float moveEndTime = 0.0f;

	// ノックバックの強さ
	float knockback = 0.0f;

	// ノックバックの方向
	Vector3 knockbackDirection = Vector3(0.0f, 0.0f, 1.0f);

	// アニメーション
	AnimationHandle hAttackAnimation = 0;

	// 相手側のアニメーション
	AnimationHandle hTargetAnimation = 0;

	// 相手を離すかどうか
	bool isRelease = false;

	// ダメージリアクション
	DamageReaction damageReaction = DamageReaction::None;

	// ヒット
	std::vector<HitDefinition> hits;

	// チャージ時間
	float chargeTime = 0.0f;

	// チャージ完了の時間
	float chargeCompleteTime = 0.0f;

	// @brief チャージ攻撃終了の攻撃時間
	float chargeFinishAttackTime = 0.0f;

	// チャージ攻撃するかどうか
	bool isChargeAttack = false;
};

class GrabStrikeAttack : public Attack
{
public:

	/// @brief コンストラクタ
	/// @param character 
	/// @param initData 
	GrabStrikeAttack(Character* character, const GrabStrikeAttackInitData& initData);

	/// @brief デストラクタ
	~GrabStrikeAttack();

	/// @brief 実行
	virtual void Exec() override;

	/// @brief 更新処理
	virtual void Update() override;

	/// @brief リセット
	virtual void Reset() override;

	/// @brief 終了、中断
	virtual void Exit() override;

	/// @brief チャージ攻撃可能かどうか
	/// @param type 
	void SetChargeInputType(AttackInputType type) override { chargeInputType_ = type; }

	/// @brief チャージタイマーの割合を取得する
	/// @return 
	float GetChargeTimeRate()const override { return canChargeAttack_ && chargeTime_ > 0.0f ? std::clamp(chargeTimer_ / chargeTime_, 0.0f, 1.0f) : 0.0f; }


private:

	// 前回の攻撃タイマー
	float prevTimer_ = 0.0f;

	// ノックバックの強さ
	float knockback_ = 0.0f;

	// ノックバックの方向
	Vector3 knockbackDirection_ = Vector3(0.0f, 0.0f, 1.0f);

	/// @brief 掴んでいる相手
	Character* grabbedTarget_ = nullptr;

	// 相手側のアニメーション
	AnimationHandle hTargetAnimation_ = 0;

	// 離すかどうか
	bool isRelease_ = false;

	/// @brief ダメージリアクション
	DamageReaction damageReaction_ = DamageReaction::None;

	// ヒット
	std::vector<HitDefinition> hits_;

	// 既に相手を手放したかどうか
	bool isReleased_ = false;


private:

	/// @brief チャージ攻撃のタイマー
	float chargeTimer_ = 0.0f;

	/// @brief チャージする時間
	float chargeTime_ = 0.0f;

	// @brief チャージ攻撃終了の攻撃時間
	float chargeFinishAttackTime_ = 0.0f;

	// チャージ完了時間
	float chargeCompleteTime_ = 0.0f;

	// @brief チャージ攻撃の入力タイプ
	AttackInputType chargeInputType_ = AttackInputType::InputX;

	// チャージ攻撃するかどうか
	bool isChargeAttack_ = false;

	/// @brief チャージ攻撃可能かどうか
	bool canChargeAttack_ = false;

	// @brief チャージが完了したかどうか
	bool isChargeFinished_ = false;
};

