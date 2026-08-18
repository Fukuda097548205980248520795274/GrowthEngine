#pragma once
#include "../Attack.h"

struct CombAttackInitData
{
	// 攻撃モーションのハンドル
	AnimationHandle hAttackMotion = 0;

	// 攻撃時間
	float attackTime = 0.0f;

	// 移動速度
	float moveSpeed = 0.0f;

	// 移動開始時間
	float moveStartTime = 0.0f;

	// 移動終了時間
	float moveEndTime = 0.0f;

	// コンボキャンセル開始時間
	float cancelStartTime = 0.0f;

	// コンボキャンセル終了時間
	float cancelEndTime = 0.0f;

	// 武器をつかむかどうか
	bool isGrabWeapon = false;

	/// @brief 武器をつかむ開始時間
	float grabWeaponStartTime = 0.0f;

	/// @brief 武器をつかむ終了時間
	float grabWeaponEndTime = 0.0f;

	// @brief 投げる武器かどうか
	bool isThrowWeapon = false;

	// @brief 投げる武器の開始時間
	float throwWeaponTime = 0.0f;

	// @brief 投げる武器のパワー
	float throwWeaponPower = 3.0f;

	// @brief 投げる方向（正規化されたベクトル）
	Vector3 throwDirection = Vector3(0.0f, 0.0f, 1.0f);

	// ヒット判定のグループ
	std::vector<HitGroupDefinition> groups;

	// ヒット判定
	std::vector<HitboxDefinition> hitboxes;
};

class ComboAttack : public Attack
{
public:

	/// @brief コンストラクタ
	/// @param character 
	/// @param initData 
	ComboAttack(Character* character, const CombAttackInitData& initData);

	/// @brief デストラクタ
	~ComboAttack();

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

	/// @brief 攻撃中かどうか
	/// @return 
	bool IsGrabWeapon() const override;

	/// @brief 投げる武器かどうか
	/// @return 
	bool IsThrowWeapon() const override { return isThrowWeapon_; }


private:

	// コンボキャンセル受付時間
	float cancelStartTime_ = 0.0f;

	// コンボキャンセル終了時間
	float cancelEndTime_ = 0.0f;

	/// @brief 武器をつかむかどうか
	bool isGrabWeapon_ = false;

	/// @brief 掴み開始時間
	float grabWeaponStartTime_ = 0.0f;

	/// @brief 掴み終了時間
	float grabWeaponEndTime_ = 0.0f;


private:

	// @brief 投げる武器かどうか
	bool isThrowWeapon_ = false;

	// @brief 投げる武器の開始時間
	float throwWeaponTime_ = 0.0f;

	// @brief 投げる武器のパワー
	float throwWeaponPower_ = 3.0f;

	// @brief 投げる方向（正規化されたベクトル）
	Vector3 throwDirection_ = Vector3(0.0f, 0.0f, 1.0f);

	/// @brief 投げた武器かどうか
	bool hasThrownWeapon_ = false;


private:

	/// @brief ヒット判定の状態を保持するリスト
	std::vector<HitboxState> hitStates_;

	// グループの定義を保持するリスト
	std::vector<HitGroupDefinition> groups_;

	/// @brief グループIDごとにヒットしたキャラクターのリストを保持するマップ
	std::unordered_map<int32_t, std::vector<Character*>> hitCharactersByGroup_;
};

