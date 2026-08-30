#pragma once
#include "../Entity.h"
#include "CharacterTypes.h"
#include "CharacterComponent/CharacterMovement/CharacterMovement.h"

#include "BlackBoard/BlackBoard.h"
#include "MotionManager/MotionManager.h"
#include "SoundManager/SoundManager.h"
#include "Action/Attack/ComboAttack/ComboAttack.h"
#include "Action/Attack/GrabAttack/GrabAttack.h"
#include "Action/Telegraph/Telegraph.h"
#include "AppCollider/AppCollider.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Shake/Shake.h"

#include "CharacterStateMachine/CharacterStateMachine.h"

class Attack;
class Move;
class Avoid;
class NavMesh;
class EffectManager;
class Gage;

class Character : public Entity
{
public:

	/// @brief コンストラクタ
	/// @param initData 
	Character();

	/// @brief デストラクタ
	virtual ~Character() override;

	/// @brief アニメーションの初期化
	/// @param animationData 
	void SetAnimationHandle(const AnimationHandleData& animationData);

	/// @brief ビヘイビアツリーの設定
	/// @param behaviorTreeName 
	virtual void SetBehaviorTree(BehaviorTree* behaviorTree) { (void)behaviorTree; }

	/// @brief ビヘイビアツリーの取得
	/// @return 
	virtual BehaviorTree* GetBehaviorTree() const { return nullptr; }

	/// @brief 更新処理
	virtual void Update() override;

	/// @brief 更新処理開始前のリセット
	virtual void StartUpdate();

	/// @brief 更新処理終了後のリセット
	/// @return 
	CharacterStateMachine* GetStateMachine() const { return stateMachine_.get(); }

	/// @brief ダメージを受けたときの処理
	/// @param damage 
	/// @param damageReaction 
	/// @param knockback 
	/// @param knockDirection 
	/// @param enemyPosition 
	/// @param attacker 
	/// @param hitPosition 
	virtual bool OnDamage(int damage, DamageReaction damageReaction, float knockback,
		const Vector3& knockDirection, const Vector3& enemyPosition, Character* attacker = nullptr, std::optional<Vector3> hitPosition = std::nullopt,
		bool isGuardBreak = false, bool isThrow = false, Weapon* weapon = nullptr, bool isChargeAttack = false);

	/// @brief ガードダメージを受けたときの処理
	/// @param damage 
	/// @param damageReaction 
	/// @param knockback 
	/// @param knockDirection 
	/// @param enemyPosition 
	/// @param attacker 
	/// @param hitPosition 
	/// @param isGuardBreak 
	/// @param isThrow 
	/// @param weapon 
	/// @param isChargeAttack 
	/// @return 
	virtual bool OnGuardDamage(int damage, DamageReaction damageReaction, float knockback,
		const Vector3& knockDirection, const Vector3& enemyPosition, Character* attacker = nullptr, std::optional<Vector3> hitPosition = std::nullopt,
		bool isGuardBreak = false, bool isThrow = false, Weapon* weapon = nullptr, bool isChargeAttack = false);

	/// @brief 受け流されたときの処理
	/// @param pullPosition 
	/// @param pushDirection 
	/// @return 
	virtual void OnDeflected(const Vector3& pullPosition, const Vector3& pushDirection, float knockBackPower);

	/// @brief 弾かれた時の処理
	/// @param pushDirection 
	virtual void OnRepelled(const Vector3& pushDirection, float knockBackPower);

	/// @brief 掴みダメージを受けた時の処理
	/// @param damage 
	virtual void OnGrabDamage(int damage, DamageReaction damageReaction, Character* attacker = nullptr, std::optional<Vector3> hitPosition = std::nullopt);

	/// @brief 全キャラクターのリストを取得
	static const std::vector<Character*>& GetCharacters() { return characters_; }

	/// @brief 体力を設定する
	/// @param hp 
	void SetHp(int hp) { hp_ = std::clamp(hp, 0, maxHp_); }

	/// @brief 最大体力を設定する
	/// @param maxHp 
	void SetMaxHp(int maxHp) { maxHp_ = std::max(maxHp, 1); }

	/// @brief 体力を取得する
	/// @return 
	int GetHp()const { return hp_; }

	/// @brief 最大体力を取得する
	/// @return 
	int GetMaxHp()const { return maxHp_; }

	/// @brief 回避を開始する
	/// @param direction 
	/// @param distance 
	/// @param time 
	void StartAvoid(const Vector3& direction, float distance, float time);

	/// @brief キャラクターのタグを取得する
	/// @return 
	CharacterTag GetCharacterTag()const { return characterTag_; }

	/// @brief 移動を停止させる
	void MoveStop() { movement_->Stop(); }

	/// @brief XZ平面の移動入力を設定する
	/// @param direction
	/// @param maxSpeed
	void SetMoveInputXZ(const Vector2& direction, float maxSpeed) { movement_->SetMoveInputXZ(direction, maxSpeed); }

	/// @brief 現在向いている方向ベクトルを取得する
	/// @return
	Vector3 GetDirection() const { return movement_->GetDirection(); }

	/// @brief 向きを設定する
	/// @param direction 
	void SetDirection(const Vector3& direction) { movement_->SetDirection(direction.Normalize()); }

	/// @brief 自分を攻撃してきた相手を取得する
	/// @return 
	Character* GetAttacker() const;

	/// @brief ロックオンしているターゲットを取得する
	/// @return
	Character* GetLockOnTarget() const { return lockOnTarget_; }

	/// @brief ロックオンした相手をクリアする
	void ClearLockOnTarget() { lockOnTarget_ = nullptr; }

	/// @brief ナビゲーションメッシュを取得する
	/// @return 
	virtual const NavMesh* GetNavMesh() const { return nullptr; }

	/// @brief ターゲットがいるかどうか
	/// @return 
	bool HasTarget() const { return lockOnTarget_ != nullptr; }

	/// @brief 攻撃しているかどうか
	/// @return 
	bool IsAttack()const { return currentAttack_ != nullptr; }

	/// @brief 攻撃のシーケンスに入っているかどうか（攻撃動作全体の中かどうか）
	/// @return 
	bool IsInAttackSequence() const { return isInAttackSequence_; }

	/// @brief 攻撃のシーケンスに入るかどうかを設定する
	/// @param isInSequence 
	void SetInAttackSequence(bool isInSequence) { isInAttackSequence_ = isInSequence; }

	/// @brief 回避しているかどうか
	/// @return 
	bool IsAvoid() const { return stateMachine_->GetCurrentStateName() == "Avoid"; }

	/// @brief 回避した瞬間かどうか
	/// @return 
	bool IsJustAvoided() const { return isJustAvoided_ || isJustAvoidedPrev_; }

	/// @brief 構えているかどうか
	/// @return 
	bool IsStance() const { return isStance_; }

	/// @brief ダッシュしているかどうか
	/// @return 
	bool IsDash()const { return stateMachine_->GetCurrentStateName() == "Dash"; }

	/// @brief 死亡しているかどうか
	/// @return 
	bool IsDead() const { return stateMachine_->GetCurrentStateName() == "Dead"; }

	/// @brief 予備動作中かどうか
	/// @return 
	bool IsTelegraph() const { return currentTelegraph_ != nullptr; }

	/// @brief 現在の攻撃を設定する
	/// @param attack 
	virtual void SetCurrentAttack(Attack* attack) { currentAttack_ = attack; }

	/// @brief 現在の攻撃のクールタイムを取得する
	/// @return 
	virtual float GetAttackCooltime()const { return 0.0f; }

	/// @brief 攻撃のクールタイムを設定する
	/// @param cooltime 
	virtual void SetAttackCooltime(float cooltime) { (void)cooltime; }

	/// @brief 現在の攻撃を取得する
	/// @return 
	Attack* GetCurrentAttack() const { return currentAttack_; }

	/// @brief 現在の移動を設定する
	/// @param move 
	void SetCurrentMove(Move* move) { currentMove_ = move; }

	/// @brief 現在の移動を取得する
	/// @return 
	Move* GetCurrentMove() const { return currentMove_; }

	/// @brief 現在の回避を設定する
	/// @param avoid 
	void SetCurrentAvoid(Avoid* avoid) { currentAvoid_ = avoid; }

	/// @brief 現在の回避を取得する
	/// @return 
	Avoid* GetCurrentAvoid() const { return currentAvoid_; }

	/// @brief 現在の予備動作を設定する
	/// @param telegraph 
	void SetCurrentTelegraph(Telegraph* telegraph) { currentTelegraph_ = telegraph; }

	/// @brief 現在の予備動作を取得する
	/// @return 
	Telegraph* GetCurrentTelegraph() const { return currentTelegraph_; }

	/// @brief アニメーションを設定する
	/// @param hAnimation 
	/// @param isReset
	void SetAnimation(AnimationHandle hAnimation, bool isReset, bool isLoop);

	/// @brief 現在のアニメーションの再生時間を取得する
	/// @return 
	float GetAnimationTimer()const { return model_->param_->animation.timer; }

	/// @brief バッファされた攻撃入力を取得する
	/// @return 
	AttackInputType GetBufferedAttackInput() const { return bufferedAttackInput_; }

	/// @brief バッファされたチャージ攻撃入力を取得する
	/// @return 
	AttackInputType GetBufferedChargeAttackInput() const { return bufferedChargeAttackInput_; }

	/// @brief バッファされた攻撃入力を消化する
	void ConsumeBufferedAttackInput() { bufferedAttackInput_ = AttackInputType::None; }

	/// @brief バッファされた攻撃入力を設定する
	/// @param input 
	void SetBufferedAttackInput(AttackInputType input) { bufferedAttackInput_ = input; }

	/// @brief 当たり判定を取得する
	/// @return 
	Collision3DBaseSphere* GetHitboxGroup() const { return hitboxGroup_; }

	/// @brief ボーン行列を取得する
	/// @param partName 
	/// @return 
	Matrix4x4 GetBoneMatrix(const JointType& jointType) const;

	/// @brief ボーンの位置を取得する
	/// @param jointType 
	/// @return 
	Vector3 GetBonePosition(const JointType& jointType) const;

	/// @brief ダメージリアクション中かどうか
	/// @return 
	bool IsDamageReaction() const { return IsLightDamage() || IsHeavyDamage() || IsGroundedDown() || IsBlownDown() || IsDeflected() || IsRepelled(); }

	/// @brief 地面に倒れているかどうか
	/// @return 
	bool IsGroundedDown() const { return IsDownFalling() || IsDownLying() || IsGettingUp() || IsDownStagger(); }

	/// @brief 吹き飛ばされてダウンしているかどうか
	/// @return 
	bool IsBlownDown() const { return IsBlownAway() || IsBlownFalling(); }

	/// @brief ダウン中かどうか
	/// @return 
	bool IsDown()const { return IsDownFalling() || IsDownLying() || IsGettingUp(); }

	/// @brief 軽ダメージ中かどうか
	/// @return 
	bool IsLightDamage() const { return stateMachine_->GetCurrentStateName() == "LightDamage"; }

	/// @brief 重ダメージ中かどうか
	/// @return 
	bool IsHeavyDamage() const { return stateMachine_->GetCurrentStateName() == "HeavyDamage"; }

	/// @brief 倒れこみ中かどうか
	/// @return 
	bool IsDownFalling() const { return stateMachine_->GetCurrentStateName() == "DownFalling"; }

	/// @brief ダウン中かどうか
	/// @return 
	bool IsDownLying() const { return stateMachine_->GetCurrentStateName() == "DownLying"; }

	/// @brief 起き上がり中かどうか
	/// @return 
	bool IsGettingUp() const { return stateMachine_->GetCurrentStateName() == "DownGettingUp"; }

	/// @brief ダウン中に怯んでいるかどうか
	/// @return 
	bool IsDownStagger() const { return stateMachine_->GetCurrentStateName() == "DownStagger"; }

	/// @brief 吹き飛び中かどうか
	/// @return 
	bool IsBlownAway() const { return stateMachine_->GetCurrentStateName() == "BlownAway"; }

	/// @brief 吹き飛び落下中かどうか
	/// @return 
	bool IsBlownFalling() const { return stateMachine_->GetCurrentStateName() == "BlownFalling"; }

	/// @brief 頭の当たり判定を取得する
	/// @return 
	AppCollider& GetHurtboxHead() { return hurtboxHead_; }

	/// @brief 胴の当たり判定を取得する
	/// @return 
	AppCollider& GetHurtboxChest() { return hurtboxChest_; }

	/// @brief 腰の当たり判定を取得する
	/// @return 
	AppCollider& GetHurtboxRoot() { return hurtboxRoot_; }

	/// @brief 相手をつかんでいるかどうか
	/// @return 
	bool IsGrabbing() const { return stateMachine_->GetCurrentStateName() == "Grabbing"; }

	/// @brief 自分をつかんでいる相手がいるかどうか
	/// @return 
	bool IsGrabbed() const { return stateMachine_->GetCurrentStateName() == "Grabbed"; }

	/// @brief 相手をつかむ
	/// @param target 
	/// @param duration つかむ時間
	void ExecuteGrab(Character* target, float duration, const std::optional<Vector3>& hitPosition = std::nullopt);

	/// @brief 掴まれた相手の処理
	void OnGrabbed(Character* grabber);

	/// @brief 防御しているかどうか
	/// @return 
	bool IsGuard() const { return stateMachine_->GetCurrentStateName() == "Guard"; }

	/// @brief 防御を実行する
	void ExecuteGuard();

	/// @brief 着地しているかどうか
	/// @return 
	bool IsGrounded() const { return movement_->IsGrounded(); }

	/// @brief 掴んでいる相手を取得する
	/// @return 
	Character* GetGrabTarget() const;

	/// @brief 掴まれている相手を取得する
	/// @param target 
	void SetGrabTarget(Character* target);
	
	/// @brief 掴んだ状態の攻撃をしているかどうか
	/// @return 
	bool IsGrabStrikeAttack() const;

	/// @brief 掴まれた状態で攻撃されているかどうか
	/// @return 
	bool IsGrabbedDamage()const;

	/// @brief 受け流しを実行する
	/// @param attacker 
	void ExecuteDeflect(Character* attacker);

	/// @brief 弾きを実行する
	/// @param attacker 
	/// @param hitPosition 
	void ExecuteRepel(Character* attacker, std::optional<Vector3> hitPosition = std::nullopt);

	/// @brief 受け流し中かどうか
	/// @return 
	bool IsDeflecting() const { return stateMachine_->GetCurrentStateName() == "Deflect"; }

	/// @brief 受け流されているかどうか
	/// @return 
	bool IsDeflected() const { return stateMachine_->GetCurrentStateName() == "Deflected"; }

	/// @brief 弾き中かどうか
	/// @return 
	bool IsRepelling() const { return stateMachine_->GetCurrentStateName() == "Repel"; }

	/// @brief 弾かれたかどうか
	/// @return 
	bool IsRepelled() const { return stateMachine_->GetCurrentStateName() == "Repelled"; }

	/// @brief スタイルチェンジを開始する
	/// @param style 
	void StartStyleChange(FightStyle style);

	/// @brief 現在のスタイルを取得する
	/// @return 
	FightStyle GetCurrentStyle() const { return currentStyle_; }

	/// @brief 次のスタイルを取得する
	/// @return 
	FightStyle GetNextStyle() const { return nextStyle_; }

	/// @brief スタイルチェンジの経過時間を取得する
	/// @return 
	float GetStyleChangeTimer() const { return styleChangeTimer_; }

	/// @brief スタイルチェンジの時間を取得する
	/// @return 
	float GetStyleChangeDuration() const { return kStyleChangeDuration; }

	/// @brief スタイルチェンジ中かどうか
	/// @return 
	bool IsStyleChanging() const { return isStyleChanging_; }

	/// @brief 武器を持っているかどうか
	/// @return 
	bool HasWeapon() const { return weapon_ != nullptr; }

	/// @brief 所持している武器を取得する
	/// @return 
	Weapon* GetWeapon() const { return weapon_; }

	/// @brief 武器を掴む
	/// @param weapon 
	void GrabWeapon(Weapon* weapon);

	/// @brief 武器を離す
	/// @param blowVelocity 
	void ReleaseWeapon(const Vector3& blowVelocity = Vector3(0.0f, 0.0f, 0.0f));

	/// @brief 受け流しが可能かどうかを取得する
	/// @return 
	bool CanDeflect() const { return canDeflect_; }

	/// @brief 弾きが可能かどうかを取得する
	/// @return 
	bool CanRepel() const { return canRepel_; }

	/// @brief 受け流しが可能かどうかを設定する
	/// @param canDeflect 
	void SetCanDeflect(bool canDeflect) { canDeflect_ = canDeflect; }

	/// @brief 弾きが可能かどうかを設定する
	/// @param canRepel 
	void SetCanRepel(bool canRepel) { canRepel_ = canRepel; }

	/// @brief プレイヤーかどうかを取得する
	bool IsPlayer() const { return characterTag_ == CharacterTag::Player; }

	/// @brief ボスかどうかを取得する
	/// @return 
	bool IsBoss() const { return characterTag_ == CharacterTag::EnemyBoss; }

	/// @brief 無力化されているかどうか（スタイルチェンジ中、地面にいない、ダメージリアクション中、掴まれているのいずれか）
	/// @return 
	bool IsIncapacitated() const { return IsStyleChanging() || !IsGrounded() || IsDamageReaction() || 
		IsGrabbed() || IsDead() || IsFinished() || IsRageModeStart() || IsDeflecting() || IsRepelling(); }

	/// @brief プレイヤー側かどうか
	/// @return 
	bool IsPlayerSide() const { return characterTag_ == CharacterTag::Player || characterTag_ == CharacterTag::Ally || characterTag_ == CharacterTag::Vip; }

	/// @brief 敵側かどうか
	/// @return 
	bool IsEnemySide() const { return characterTag_ == CharacterTag::EnemyNormal || characterTag_ == CharacterTag::EnemyBoss; }

	/// @brief 攻撃がヒットしているかどうか
	/// @return 
	bool IsHitAttack() const { return isHitAttack_ || isPrevHitAttack_; }

	/// @brief ダメージを受けたかどうか
	/// @return 
	bool IsHitDamage() const { return isHitDamage_ || isPrevHitDamage_; }

	/// @brief ガードが成功したかどうか
	/// @return 
	bool IsGuardHit() const { return isGuardHit_ || isPrevGuardHit_; }

	/// @brief 攻撃がヒットしているかどうかを設定する
	/// @param isHit 
	void SetHitAttack(bool isHit) { isHitAttack_ = isHit; }

	/// @brief 弾きが成功したかどうか
	/// @return 
	bool IsHitRepel() const { return isHitRepel_ || isPrevHitRepel_; }

	/// @brief トレイルの位置を設定する
	/// @param basePosition
	/// @param tipPosition 
	void SetTrailPos(const Vector3& basePosition, const Vector3& tipPosition);

	/// @brief トレイルをクリアする
	void TrailClear() { if (attackTrail_)attackTrail_->Clear(); }

	/// @brief 死亡処理
	virtual void Dead();

	/// @brief 状態が変更されたかどうか
	/// @return 
	bool IsChangeState() const { return stateMachine_->IsChangeState(); }

	/// @brief シェイクを開始する
	/// @param duration 
	/// @param magnitude 
	/// @param direction 
	void StartShake(float duration, float magnitude, const Vector3& direction) { if (shake_) shake_->StartShake(duration, magnitude, direction); }

	/// @brief モデルを持っているかどうか
	/// @return 
	bool HasModel() const { return model_ != nullptr; }

	/// @brief モデルを取得する
	/// @return 
	Render3DSkinningModel* GetModel() const { return model_; }

	/// @brief 攻撃用トレイルを取得する
	/// @return 
	Trail3D* GetAttackTrail() const { return attackTrail_; }

	/// @brief レイジゲージが満タンかどうか
	/// @return 
	bool IsRageGageFull() const { if (rageGageThresholds_.empty())return false; return rageGage_ >= rageGageThresholds_.back(); }

	/// @brief レイジゲージの閾値を超えているかどうか
	/// @return 
	bool IsRageGageThresholdExceeded() const { return rageGageThresholdIndex_ > 0; }

	/// @brief レイジゲージの最大値を取得する
	/// @return 
	float GetMaxRageGage() const { if (rageGageThresholds_.empty())return 0.0f; return rageGageThresholds_.back(); }

	/// @brief レイジモード中かどうか
	/// @return 
	bool IsRageMode() const { return isRageMode_; }

	/// @brief レイジモード開始中かどうか
	/// @return 
	bool IsRageModeStart() const { return isRageMode_ && rageModeStartTimer_ > 0.0f; }

	/// @brief レイジモード中の攻撃速度を取得する
	/// @return 
	float RageModeAttackSpeed()const { return IsRageMode() && IsAttack() ? 2.0f : 1.0f; }

	/// @brief レイジモード中の攻撃力を取得する
	/// @return 
	float RageModeAttackPower()const { return IsRageMode() && IsAttack() ? 1.5f : 1.0f; }

	/// @brief レイジモード開始に成功したかどうか
	/// @return 
	bool IsSuccessRageModeStart() const { return isSuccessRageModeStart_ || isPrevSuccessRageModeStart_; }

	/// @brief レイジゲージを取得する
	/// @param rageGage 
	void SetRageGage(float rageGage) { rageGage_ = rageGage; }

	/// @brief 移動コンポーネントを取得する
	/// @return 
	CharacterMovement* GetMovement() const { return movement_.get(); }


	/// @brief 回避方向を取得する
	/// @param moveInputDirection
	/// @param hasMoveInput
	/// @param cameraYaw
	/// @return
	Vector2 GetAvoidDirection(const Vector2& moveInputDirection, bool hasMoveInput, float cameraYaw) const;

	/// @brief カットシーンの有効化を設定する
	/// @param isCutscene 
	static void SetIsCutsceneActive(bool isCutscene) { isCutsceneActive_ = isCutscene; }

	/// @brief カットシーンが有効かどうかを取得する
	/// @return 
	static bool IsCutsceneActive() { return isCutsceneActive_; }

	/// @brief ゲームが終了したかどうかを設定する
	/// @param isFinished 
	static void SetIsGameFinished(bool isFinished) { isGameFinished_ = isFinished; }

	/// @brief ゲームが終了したかどうかを取得する
	/// @return 
	static bool IsGameFinished() { return isGameFinished_; }

	/// @brief ゲーム開始時のイントロ中かどうかを設定する
	/// @param isIntro 
	static void SetIsGameIntro(bool isIntro) { isGameIntro_ = isIntro; }

	/// @brief ゲーム開始時のイントロ中かどうかを取得する
	/// @return 
	static bool IsGameIntro() { return isGameIntro_; }

	/// @brief 攻撃性の値を取得する
	/// @return 
	virtual float GetAggressiveness() const { return 1.0f; }

	/// @brief 攻撃性の値を設定する
	/// @param aggressiveness 
	virtual void SetAggressiveness(float aggressiveness) {}

	/// @brief スロットシステムによる距離を取得する
	/// @return 
	float GetSlotDistance() const { return slotDistance_; }

	/// @brief スロットの角度オフセットを取得する
	/// @return 
	float GetPersonalSlotDegreeOffset() const { return personalSlotDegreeOffset_; }

	/// @brief 間合いを取っているかどうかを取得する
	/// @return 
	virtual bool IsTakingDistance() const { return false; }

	/// @brief 現在の速度を取得する
	/// @return 
	Vector3 GetVelocity() const { return movement_->GetCurrentVelocity(); }

	/// @brief 壁に接触しているかどうかを取得する
	/// @return 
	bool IsWallTouch() const { return isWallTouching_; }

	/// @brief ガードブレイクが発生したかどうかを取得する
	/// @return 
	bool IsGuardBreak() const { return isGuardBreak_; }

	/// @brief ガードブレイクさせたかどうか
	/// @return 
	bool IsGuardBreaking() const { return isGuardBreaking_ || isPrevGuardBreaking_; }

	/// @brief ガードブレイクされたかどうか
	/// @return 
	bool IsGuardBroke() const { return isGuardBroke_ || isPrevGuardBroke_; }

	/// @brief ガードブレイクさせたかどうかの設定
	/// @param isGuardBreaking 
	void SetIsGuardBreaking(bool isGuardBreaking) { isGuardBreaking_ = isGuardBreaking; }

	/// @brief ガードゲージ量の設定
	/// @param guardGage 
	void SetGuardGage(float guardGage) { guardGage_ = guardGage; maxGuardGage_ = std::max(maxGuardGage_, guardGage); }

	/// @brief ガードゲージ量を取得する
	/// @return 
	float GetGuardGage()const { return guardGage_; }

	/// @brief 最大ガードゲージ量を取得する
	/// @return 
	float GetMaxGuardGage()const { return maxGuardGage_; }

	/// @brief ガード復活時間の設定
	/// @param guardRecoveryTime 
	void SetGuardRecoveryTime(float guardRecoveryTime) { guardRecoveryTimer_ = guardRecoveryTime; }

	/// @brief ほどき時間を取得する
	/// @return 
	float GetUnravelingTime() const { return unravelingTime_; }

	/// @brief ほどき時間を設定する
	/// @param unravelingTime 
	void SetUnravelingTime(float unravelingTime) { unravelingTime_ = unravelingTime; }

	/// @brief チャージ攻撃のチャージ時間の割合を取得する
	/// @return 
	float GetChargeTimeRate() const { return currentAttack_ ? currentAttack_->GetChargeTimeRate() : 0.0f; }


protected:

	/// @brief 初期化用データを設定する
	/// @param initData 
	void SetInitData(const CharacterInitData& initData);

	/// @brief 当たり判定の更新
	/// @param hurtbox 
	/// @param jointType 
	void UpdateHurtbox(AppCollider& hurtbox, JointType jointType);

	/// @brief 当たり判定の位置を更新する
	/// @param collision 
	/// @param newPosition 
	void UpdateCollisionPosition(Collision3DInstanceCapsule* collision, const Vector3& newPosition = Vector3(0.0f, 0.0f, 0.0f));

	/// @brief カメラのローカル方向をワールド座標系の移動方向に変換する
	/// @param cameraLocalDirection 
	/// @param cameraYaw 
	/// @return 
	static Vector2 ToWorldMoveDirectionFromCamera(const Vector2& cameraLocalDirection, float cameraYaw);


protected:

	/// @brief エンジン
	const GrowthEngine* engine_ = nullptr;

	/// @brief モーションマネージャ
	MotionManager* motionManager_ = nullptr;

	/// @brief サウンドマネージャ
	SoundManager* soundManager_ = nullptr;

	/// @brief エフェクトマネージャ
	EffectManager* effectManager_ = nullptr;

	// キャラクターのタグ
	CharacterTag characterTag_;

	/// @brief ステートマシン
	std::unique_ptr<CharacterStateMachine> stateMachine_ = nullptr;

	/// @brief 移動コンポーネント
	std::unique_ptr<CharacterMovement> movement_ = nullptr;

	/// @brief ブラックボード
	std::unique_ptr<Blackboard> blackboard_ = nullptr;

	/// @brief シェイク
	std::unique_ptr<Shake> shake_ = nullptr;


	/// @brief 最大の体力
	int maxHp_ = 0;

	/// @brief 体力
	int hp_ = 0;

	/// @brief 攻撃力
	float attackPower_ = 1.0f;


protected:

	/// @brief 武器所持の更新
	void UpdateWeapon();

	/// @brief 所持している武器
	Weapon* weapon_ = nullptr;

	/// @brief 壊れた武器を破壊するための参照
	Weapon* brokenWeaponToDestroy_ = nullptr;

	/// @brief 武器を放した際のツリーを変更するかどうかのフラグ
	bool isReleaseWeaponTree_ = false;


protected:

	/// @brief 回避した瞬間かどうか
	bool isJustAvoided_ = false;
	bool isJustAvoidedPrev_ = false;


protected:

	// 構え中かどうか
	bool isStance_ = false;

	// 構え状態でなくてもロックオン候補を更新するかどうか
	bool canLockOnWithoutStance_ = false;

	/// @brief ロックオンしているターゲット
	Character* lockOnTarget_ = nullptr;

	// 構え中のロックオン候補を更新する
	void UpdateLockOnTargets();

	/// @brief ロックオンしているターゲットを検索する
	virtual void SearchLockOnTarget() = 0;

	// Characterインスタンスのリスト
	static std::vector<Character*> characters_;

	/// @brief スロットシステムによる距離
	float slotDistance_ = 6.0f;


protected:

	/// @brief ガードゲージの更新
	void UpdateGuardGage();

	// ガードに成功したかどうか
	bool isGuardHit_ = false;

	// 前フレームでガードに成功したかどうか
	bool isPrevGuardHit_ = false;


	/// @brief ガードゲージ
	float guardGage_ = 0.0f;

	/// @brief ガードゲージの最大値
	float maxGuardGage_ = 0.0f;

	// ガードゲージ復活量
	float guardGageRecoveryAmount_ = 0.1f;

	/// @brief ガードブレイクが発生したかどうか
	bool isGuardBreak_ = false;


	/// @brief ガードブレイクした瞬間
	bool isGuardBreaking_ = false;

	/// @brief 前フレームのガードブレイクした瞬間
	bool isPrevGuardBreaking_ = false;

	/// @brief ガードブレイクされたかどうか
	bool isGuardBroke_ = false;

	/// @brief 前のフレームでガードブレイクされたかどうか
	bool isPrevGuardBroke_ = false;


	// ガード復活タイマー
	float guardRecoveryTimer_ = 0.0f;

	/// @brief ガード復活までの時間
	float guardRecoveryTime_ = 5.0f;


protected:

	/// @brief 受け流しが可能かどうか
	bool canDeflect_ = false;

	/// @brief 弾きが可能かどうか
	bool canRepel_ = false;


	/// @brief 弾きが成功したかどうか
	bool isHitRepel_ = false;

	/// @brief 前フレームで弾きが成功したかどうか
	bool isPrevHitRepel_ = false;


protected:

	// バッファされた攻撃入力
	AttackInputType bufferedAttackInput_ = AttackInputType::None;

	/// @brief バッファされたチャージ攻撃入力
	AttackInputType bufferedChargeAttackInput_ = AttackInputType::None;


protected:

	/// @brief スタイルチェンジ開始時の処理
	virtual void StyleChangeStart();

	/// @brief スタイルが変化したときの処理
	/// @param newStyle 
	virtual void OnStyleChanged(FightStyle newStyle);

	/// @brief スタイルチェンジの更新処理
	/// @param dt 
	void UpdateStyleChange(float dt);

	// 現在のスタイル
	FightStyle currentStyle_ = FightStyle::None;

	// 次のスタイル（スタイルチェンジ予約用）
	FightStyle nextStyle_ = FightStyle::None;

	// スタイルチェンジのタイマー
	float styleChangeTimer_ = 0.0f;

	// スタイルチェンジ中かどうか
	bool isStyleChanging_ = false;

	// スタイルチェンジの継続時間
	constexpr static float kStyleChangeDuration = 0.5f;


protected:

	/// @brief アクションの更新処理
	void ActionUpdate();

	/// @brief 今の攻撃
	Attack* currentAttack_ = nullptr;
	
	// 攻撃シーケンス中かどうか
	bool isInAttackSequence_ = false;

	/// @brief 今の移動
	Move* currentMove_ = nullptr;

	/// @brief 今の回避
	Avoid* currentAvoid_ = nullptr;

	/// @brief 今の予備動作
	Telegraph* currentTelegraph_ = nullptr;


protected:

	/// @brief アニメーションの更新
	void UpdateAnimation();

	/// @brief モデル
	Render3DSkinningModel* model_ = nullptr;

	// アニメーションループ中かどうか
	bool isAnimationLoop_ = true;

	// アニメーションの経過時間
	float animationTime_ = 0.0f;

	/// @brief 攻撃用トレイル
	Trail3D* attackTrail_ = nullptr;


protected:

	// 攻撃がヒットしたかどうか
	bool isHitAttack_ = false;

	// 前フレームで攻撃がヒットしたかどうか
	bool isPrevHitAttack_ = false;


	/// @brief ダメージを受けたかどうか
	bool isHitDamage_ = false;

	/// @brief 前フレームでダメージを受けたかどうか
	bool isPrevHitDamage_ = false;


protected:

	/// @brief レイジゲージの更新
	void RageGageUpdate(float dt);

	/// @brief レイジゲージをチャージする
	/// @param damageReaction 
	void ChargeRageGage(DamageReaction damageReaction);

	/// @brief レイジモードの入力処理
	virtual void RageModeInput();


	/// @brief レイジゲージ
	float rageGage_ = 0;

	/// @brief レイジゲージの閾値
	std::vector<float> rageGageThresholds_;

	/// @brief レイジゲージの閾値を超えたかどうか
	int32_t rageGageThresholdIndex_ = 0;

	/// @brief レイジゲージの減少量
	static constexpr float kRageGageDecrease = 1.0f;

	/// @brief レイジモード中かどうか
	bool isRageMode_ = false;

	/// @brief レイジモード開始時の経過時間
	static constexpr float kRageModeStartDuration = 0.5f;

	/// @brief レイジモード開始時の経過時間
	float rageModeStartTimer_ = kRageModeStartDuration;


	/// @brief レイジモード開始に成功したかどうか
	bool isSuccessRageModeStart_ = false;

	/// @brief 前フレームでレイジモード開始に成功したかどうか
	bool isPrevSuccessRageModeStart_ = false;


protected:

	/// @brief ほどきの時間
	float unravelingTime_ = 3.0f;


protected:

	/// @brief スロットシステムによる度数のオフセット
	float personalSlotDegreeOffset_ = 0.0f;

	// スロットシステムのシフトタイマー
	float slotShiftTimer_ = 0.0f;

	// スロットシステムのシフト間隔
	float currentShiftInterval_ = 3.0f;


protected:

	/// @brief 立ちモーション
	AnimationHandle hStandMotion_ = 0;

	/// @brief 構えモーション
	AnimationHandle hStanceMotion_ = 0;

	/// @brief 歩きモーション
	AnimationHandle hWalkMotion_ = 0;

	/// @brief スタイルチェンジモーション
	AnimationHandle hStyleChangeMotion_ = 0;


	/// @brief つかみモーション
	AnimationHandle hGrabMotion_ = 0;

	/// @brief つかまれているモーション
	AnimationHandle hGrabbedMotion_ = 0;


protected:

	/// @brief 当たり判定
	AppCollider hurtboxHead_;
	AppCollider hurtboxChest_;
	AppCollider hurtboxRoot_;

	/// @brief 攻撃判定グループ
	Collision3DBaseSphere* hitboxGroup_ = nullptr;


protected:

	/// @brief 着地判定の更新
	void LandingCheck();

	/// @brief 着地判定
	Collision3DInstanceCapsule* landingCollision_ = nullptr;


protected:

	/// @brief 壁接触の判定
	void WallTouchCheck();

	/// @brief 壁接触の更新
	void WallTouchUpdate();

	/// @brief 壁に接触しているかどうか
	bool isWallTouch_ = false;

	/// @brief 壁に接触していたかどうか
	bool isWallTouching_ = false;

	/// @brief 壁接触判定
	Collision3DInstanceCapsule* wallTouchCollision_ = nullptr;


protected:

	/// @brief イベントトリガーのコリジョン
	Collision3DInstanceCapsule* eventTriggerCollision_ = nullptr;


protected:

	/// @brief 押し出し判定処理
	void UpdatePushOut();

	// 押し出し判定の半径
	static constexpr float kPushOutRadius = 0.25f;


protected:

	/// @brief カットシーン中かどうか
	static bool isCutsceneActive_;

	/// @brief ゲーム開始時のイントロ中かどうか
	static bool isGameIntro_;

	/// @brief ゲーム終了フラグ
	static bool isGameFinished_;


protected:

	/// @brief 体力HUD
	HP* hpHUD_ = nullptr;

	/// @brief 体力HUDの位置を更新する
	void HpHudUpdate();


protected:

	/// @brief ガードゲージHUD
	Gage* guardGageHUD_ = nullptr;

	/// @brief ガードゲージHUDの位置を更新する
	void GuardGageHudUpdate();
};

