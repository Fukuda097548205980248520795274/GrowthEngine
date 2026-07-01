#pragma once
#include "../Entity.h"
#include "BlackBoard/BlackBoard.h"
#include "MotionManager/MotionManager.h"
#include "SoundManager/SoundManager.h"
#include "Action/Attack/ComboAttack/ComboAttack.h"
#include "Action/Attack/GrabAttack/GrabAttack.h"
#include "AppCollider/AppCollider.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Shake/Shake.h"

class Attack;
class Move;
class Avoid;
class Weapon;
class NavMesh;
class HP;

enum class FightStyle
{
	None,
	Tempest, // 旋嵐
	Hammer, // 撃鉄
};

class Character : public Entity
{
public:

	/// @brief キャラクターのタグ
	enum class CharacterTag
	{
		None,
		Player,
		Ally,
		Vip,
		EnemyNormal,
		EnemyBoss,
	};

	/// @brief 初期化用データ
	struct InitData
	{
		/// @brief 位置
		Vector3 position = Vector3(0.0f, 0.0f, 0.0f);

		/// @brief 回転
		float rotateY = 0.0f;

		/// @brief 体力
		int hp = 0;
		
		/// @brief モデル
		Render3DSkinningModel* model_ = nullptr;

		/// @brief 武器
		Weapon* weapon = nullptr;

		/// @brief 攻撃用トレイル
		Trail3D* attackTrail = nullptr;

		/// @brief 体力HUD
		HP* hpHUD = nullptr;


		/// @brief 立ちモーション
		AnimationHandle hStandMotion = 0;

		/// @brief 構えモーション
		AnimationHandle hStanceMotion = 0;

		/// @brief 歩きモーション
		AnimationHandle hWalkMotion = 0;

		/// @brief ダッシュモーション
		AnimationHandle hDashMotion = 0;

		/// @brief 前回避モーション
		AnimationHandle hAvoidFrontMotion = 0;

		/// @brief 後ろ回避モーション
		AnimationHandle hAvoidBackMotion = 0;

		/// @brief 左回避モーション
		AnimationHandle hAvoidLeftMotion = 0;

		/// @brief 右回避モーション
		AnimationHandle hAvoidRightMotion = 0;

		/// @brief 防御モーション
		AnimationHandle hGuardMotion = 0;

		/// @brief 防御成功モーション
		AnimationHandle hGuardHitMotion = 0;


		/// @brief 行動パターンを定義したスクリプトファイル名 
		std::string behaviorTreeName = "";


		/// @brief 当たり判定グループ
		Collision3DBaseSphere* hurtboxGroup = nullptr;

		/// @brief 攻撃判定グループ
		Collision3DBaseSphere* hitboxGroup = nullptr;

		/// @brief 着地判定グループ
		Collision3DInstanceCapsule* landingCollision = nullptr;

		/// @brief 壁接触の当たり判定グループ
		Collision3DInstanceCapsule* wallTouchCollision = nullptr;

		// / @brief イベントトリガーの当たり判定グループ
		Collision3DInstanceCapsule* eventTriggerCollision = nullptr;
	};

	/// @brief アニメーションのハンドルをまとめた構造体
	struct AnimationHandleData
	{
		AnimationHandle hStandMotion = 0;
		AnimationHandle hStanceMotion = 0;
		AnimationHandle hWalkMotion = 0;
		AnimationHandle hDashMotion = 0;
		AnimationHandle hAvoidFrontMotion = 0;
		AnimationHandle hAvoidBackMotion = 0;
		AnimationHandle hAvoidLeftMotion = 0;
		AnimationHandle hAvoidRightMotion = 0;
		AnimationHandle hGuardMotion = 0;
		AnimationHandle hGuardHitMotion = 0;
	};


public:

	/// @brief コンストラクタ
	/// @param initData 
	Character();

	/// @brief デストラクタ
	virtual ~Character() override;

	/// @brief アニメーションの初期化
	/// @param animData 
	void SetAnimationHandle(const AnimationHandleData& animData);

	/// @brief ビヘイビアツリーの設定
	/// @param behaviorTreeName 
	virtual void SetBehaviorTree(std::unique_ptr<BehaviorTree> behaviorTree) { (void)behaviorTree; }

	/// @brief ビヘイビアツリーの取得
	/// @return 
	virtual BehaviorTree* GetBehaviorTree() const { return nullptr; }

	/// @brief 更新処理
	virtual void Update() override;

	/// @brief 更新処理開始前のリセット
	virtual void StartUpdate();

	/// @brief ダメージを受けたときの処理
	/// @param damage 
	/// @param damageReaction 
	/// @param knockback 
	/// @param knockDirection 
	/// @param enemyPosition 
	/// @param attacker 
	/// @param hitPosition 
	virtual bool OnDamage(int damage, DamageReaction damageReaction, float knockback,
		const Vector3& knockDirection, const Vector3& enemyPosition, Character* attacker = nullptr, std::optional<Vector3> hitPosition = std::nullopt);

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
	virtual void OnGrabDamage(int damage);

	/// @brief 全キャラクターのリストを取得
	static const std::vector<Character*>& GetCharacters() { return characters_; }

	/// @brief 体力を取得する
	/// @return 
	int GetHp()const { return hp_; }

	/// @brief 回避を開始する
	/// @param direction 
	/// @param distance 
	/// @param time 
	void StartAvoid(const Vector3& direction, float distance, float time);

	/// @brief キャラクターのタグを取得する
	/// @return 
	CharacterTag GetCharacterTag()const { return characterTag_; }

	/// @brief 移動を停止させる
	void MoveStop();

	/// @brief XZ平面の移動入力を設定する
	/// @param direction
	/// @param maxSpeed
	void SetMoveInputXZ(const Vector2& direction, float maxSpeed);

	/// @brief 現在向いている方向ベクトルを取得する
	/// @return
	Vector3 GetDirection() const { return direction_; }

	/// @brief 向きを設定する
	/// @param direction 
	void SetDirection(const Vector3& direction) { direction_ = direction.Normalize(); }

	/// @brief ロックオンしているターゲットを取得する
	/// @return
	Character* GetLockOnTarget() const { return lockOnTarget_; }

	/// @brief ターゲットの方向を向く
	virtual void TargetDirection();

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
	bool IsAvoid() const { return isAvoid_; }

	/// @brief 回避した瞬間かどうか
	/// @return 
	bool IsJustAvoided() const { return isJustAvoided_ || isJustAvoidedPrev_; }

	/// @brief 構えているかどうか
	/// @return 
	bool IsStance() const { return isStance_; }

	/// @brief 死亡しているかどうか
	/// @return 
	bool IsDead() const { return isDead_; }

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
	bool IsDamageReaction() const { return currentDamageReaction_ != DamageReactionState::None; }

	/// @brief 地面に倒れているかどうか
	/// @return 
	bool IsGrondedDown() const;

	/// @brief 吹き飛ばされてダウンしているかどうか
	/// @return 
	bool IsBlownDown() const;

	/// @brief ダウン中かどうか
	/// @return 
	bool IsDown()const { return IsDownFalling() || IsDownLying() || IsGettingUp(); }

	/// @brief 倒れこみ中かどうか
	/// @return 
	bool IsDownFalling() const;

	/// @brief ダウン中かどうか
	/// @return 
	bool IsDownLying() const { return currentDamageReaction_ == DamageReactionState::DownLyingFront || currentDamageReaction_ == DamageReactionState::DownLyingBack; }

	/// @brief 起き上がり中かどうか
	/// @return 
	bool IsGettingUp() const { return currentDamageReaction_ == DamageReactionState::DownGettingUpFront || currentDamageReaction_ == DamageReactionState::DownGettingUpBack; }

	/// @brief 吹き飛び中かどうか
	/// @return 
	bool IsBlownAway() const { return currentDamageReaction_ == DamageReactionState::BlownAwayFront || currentDamageReaction_ == DamageReactionState::BlownAwayBack; }

	/// @brief 吹き飛び落下中かどうか
	/// @return 
	bool IsBlownFalling() const { return currentDamageReaction_ == DamageReactionState::BlownFallingFront || currentDamageReaction_ == DamageReactionState::BlownFallingBack; }

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
	bool IsGrabbing() const { return grabbedTarget_ != nullptr; }

	/// @brief 自分をつかんでいる相手がいるかどうか
	/// @return 
	bool IsGrabbed() const { return grabber_ != nullptr; }

	/// @brief 相手をつかむ
	/// @param target 
	/// @param duration つかむ時間
	void ExecuteGrab(Character* target, float duration);

	/// @brief 掴まれた相手の処理
	void OnGrabbed(Character* grabber);

	/// @brief 掴んだ相手を離す
	void ReleaseGrab();

	/// @brief 防御しているかどうか
	/// @return 
	bool IsGuard() const { return isGuard_; }

	/// @brief 防御を設定する
	/// @param isGuard 
	void SetGuard(bool isGuard);

	/// @brief 着地しているかどうか
	/// @return 
	bool IsGrounded() const { return isGrounded_; }

	/// @brief 掴んでいる相手を取得する
	/// @return 
	Character* GetGrabTarget() const { return grabbedTarget_; }
	
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

	/// @brief 受け流されているかどうか
	/// @return 
	bool IsDeflected() const { return currentDamageReaction_ == DamageReactionState::Deflected; }

	/// @brief 弾かれたかどうか
	/// @return 
	bool IsRepelled() const { return currentDamageReaction_ == DamageReactionState::Repelled; }

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

	/// @brief プレイヤーかどうかを取得する
	bool IsPlayer() const { return characterTag_ == CharacterTag::Player; }

	/// @brief 無力化されているかどうか（スタイルチェンジ中、地面にいない、ダメージリアクション中、掴まれているのいずれか）
	/// @return 
	bool IsIncapacitated() const { return IsStyleChanging() || !IsGrounded() || IsDamageReaction() || IsGrabbed() || IsDead() || IsFinished(); }

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

	/// @brief シェイクを開始する
	/// @param duration 
	/// @param magnitude 
	/// @param direction 
	void StartShake(float duration, float magnitude, const Vector3& direction) { if (shake_) shake_->StartShake(duration, magnitude, direction); }

	/// @brief モデルを持っているかどうか
	/// @return 
	bool HasModel() const { return model_ != nullptr; }


protected:

	/// @brief 初期化用データを設定する
	/// @param initData 
	void SetInitData(const InitData& initData);

	/// @brief 当たり判定の更新
	/// @param hurtbox 
	/// @param jointType 
	void UpdateHurtbox(AppCollider& hurtbox, JointType jointType);

	/// @brief 当たり判定の位置を更新する
	/// @param collision 
	void UpdateCollisionPosition(Collision3DInstanceCapsule* collision);

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

	// キャラクターのタグ
	CharacterTag characterTag_;

	/// @brief ブラックボード
	std::unique_ptr<Blackboard> blackboard_ = nullptr;

	/// @brief シェイク
	std::unique_ptr<Shake> shake_ = nullptr;


	/// @brief 体力
	int hp_ = 0;

	/// @brief 攻撃力
	float attackPower_ = 1.0f;

	// やられたかどうか
	bool isDead_ = false;


	// 死亡してからの経過時間
	const float kDeadDuration = 5.0f;

	// 死亡してからの経過時間
	float deadTimer_ = kDeadDuration;


protected:

	/// @brief 武器所持の更新
	void UpdateWeapon();

	/// @brief 所持している武器
	Weapon* weapon_ = nullptr;


protected:

	/// @brief 方向
	Vector3 direction_ = Vector3(0.0f, 0.0f, 1.0f);


protected:

   // 速度補間の応答速度(1秒あたり)
	float velocityLerpSpeed_ = 6.0f;

	/// @brief 目標速度
	Vector3 targetVelocity_ = Vector3(0.0f, 0.0f, 0.0f);

	/// @brief 現在の速度
	Vector3 currentVelocity_ = Vector3(0.0f, 0.0f, 0.0f);

protected:

	/// @brief 現在の目標Y軸回転角度（ラジアン）
	float targetRotationY_ = 0.0f;

	/// @brief 回転の補間スピード（数値が大きいほど素早く振り向く）
	float rotationSpeed_ = 10.0f;


protected:

	/// @brief ダッシュフラグ
	bool isDash_ = false;

	float dashTimer_ = 0.1f;


protected:

	/// @brief 回避フラグ
	bool isAvoid_ = false;

	/// @brief 回避した瞬間かどうか
	bool isJustAvoided_ = false;
	bool isJustAvoidedPrev_ = false;

	/// @brief 回避開始位置
	Vector3 avoidStartPosition_ = Vector3(0.0f, 0.0f, 0.0f);

	/// @brief 回避終了位置
	Vector3 avoidEndPosition_ = Vector3(0.0f, 0.0f, 0.0f);

	/// @brief 回避経過時間
	float avoidElapsedTime_ = 0.0f;

	/// @brief 回避時間
	float avoidDuration_ = 0.3f;

	/// @brief 連続回避の最大回数
	int maxConsecutiveAvoidCount_ = 3;

	/// @brief 現在の連続回避回数
	int currentAvoidCount_ = 0;

	/// @brief 回避方向を取得する
	/// @param moveInputDirection
	/// @param hasMoveInput
	/// @param cameraYaw
	/// @return
	Vector2 GetAvoidDirection(const Vector2& moveInputDirection, bool hasMoveInput, float cameraYaw) const;

	/// @brief 回避中の更新処理
	/// @param deltaTime
	void UpdateAvoid(float deltaTime);


protected:

	/// @brief 目標Y回転
	float targetYaw_ = 0.0f;

	/// @brief 目標回転が有効かどうか
	bool hasTargetYaw_ = false;

	// 入力に応じて目標速度と目標回転を更新する
	static constexpr float kRotateThreshold = 0.0001f;


protected:

	// 構え中かどうか
	bool isStance_ = false;

	// 構え状態でなくてもロックオン候補を更新するかどうか
	bool canLockOnWithoutStance_ = false;

	/// @brief ロックオンしているターゲット
	Character* lockOnTarget_ = nullptr;

	// 構え中のロックオン候補を更新する
	void UpdateLockOnTargets();

	// Characterインスタンスのリスト
	static std::vector<Character*> characters_;


protected:

	/// @brief 防御中かどうか
	bool isGuard_ = false;

	/// @brief 防御のリアクション中かどうか
	bool isGuardReaction_ = false;

	/// @brief 防御のリアクションの経過時間
	float guardReactionTimer_ = 0.0f;


	// ガードに成功したかどうか
	bool isGuardHit_ = false;

	// 前フレームでガードに成功したかどうか
	bool isPrevGuardHit_ = false;


protected:

	// ガードしてからの経過時間
	float guardActiveTimer_ = 0.0f;

	// ジャストガード（受け流し）の受付時間
	const float kJustGuardTime = 0.2f;


private:

	/// @brief 受け流しが可能かどうか
	bool canDeflect_ = false;

private:

	/// @brief 弾きが可能かどうか
	bool canRepel_ = false;

	/// @brief 弾きが成功したかどうか
	bool isHitRepel_ = false;

	/// @brief 前フレームで弾きが成功したかどうか
	bool isPrevHitRepel_ = false;


protected:

	// バッファされた攻撃入力
	AttackInputType bufferedAttackInput_ = AttackInputType::None;


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

	/// @brief ダウンからの起き上がり条件を満たしているかどうか
	/// @return 
	virtual bool CheckGetUpCondition();

	/// @brief ノックバックの速度
	Vector3 knockbackVelocity_ = Vector3(0.0f, 0.0f, 0.0f);

	/// @brief 現在のダメージリアクション
	DamageReactionState currentDamageReaction_ = DamageReactionState::None;

	/// @brief ダメージリアクションの経過時間
	float damageReactionTimer_ = 0.0f;


protected:

	/// @brief 自分がつかんでいるターゲット
	Character* grabbedTarget_ = nullptr;

	/// @brief 自分をつかんでいる相手
	Character* grabber_ = nullptr;


	/// @brief 掴まれ時間
	float grabbedTimer_ = 0.0f;

	/// @brief つかみ時間の上限
	float escapeTimeLimit_ = 3.0f;


protected:

	/// @brief 立ちモーション
	AnimationHandle hStandMotion_ = 0;

	/// @brief 構えモーション
	AnimationHandle hStanceMotion_ = 0;

	/// @brief 歩きモーション
	AnimationHandle hWalkMotion_ = 0;

	/// @brief ダッシュモーション
	AnimationHandle hDashMotion_ = 0;

	/// @brief スタイルチェンジモーション
	AnimationHandle hStyleChangeMotion_ = 0;


	/// @brief 前回避モーション
	AnimationHandle hAvoidFrontMotion_ = 0;

	/// @brief 後ろ回避モーション
	AnimationHandle hAvoidBackMotion_ = 0;

	/// @brief 左回避モーション
	AnimationHandle hAvoidLeftMotion_ = 0;

	/// @brief 右回避モーション
	AnimationHandle hAvoidRightMotion_ = 0;


	/// @brief 軽い怯みモーション
	AnimationHandle hDamageLightMotion_ = 0;

	/// @brief 重い怯みモーション
	AnimationHandle hDamageHeavyMotion_ = 0;

	/// @brief ダウン怯みモーション
	AnimationHandle hDownStaggerMotion_ = 0;


	/// @brief ダウンモーション
	AnimationHandle hDownFallMotion_ = 0;

	/// @brief ダウン中モーション
	AnimationHandle hDownLyingMotion_ = 0;

	/// @brief 立ち上がりモーション
	AnimationHandle hDownGetUpMotion_ = 0;


	/// @brief つかみモーション
	AnimationHandle hGrabMotion_ = 0;

	/// @brief つかまれているモーション
	AnimationHandle hGrabbedMotion_ = 0;


	// 防御モーション
	AnimationHandle hGuardMotion_ = 0;

	// 防御成功モーション
	AnimationHandle hGuardHitMotion_ = 0;


protected:

	/// @brief 当たり判定
	AppCollider hurtboxHead_;
	AppCollider hurtboxChest_;
	AppCollider hurtboxRoot_;

	/// @brief 攻撃判定グループ
	Collision3DBaseSphere* hitboxGroup_ = nullptr;


protected:

	/// @brief 落下の更新
	/// @param deltaTime 
	void FallUpdate(float deltaTime);

	/// @brief 着地判定の更新
	void LandingCheck();

	/// @brief 着地判定
	Collision3DInstanceCapsule* landingCollision_ = nullptr;

	// 現在の落下速度
	float velocityY_ = 0.0f;

	// 地面に接地しているかどうか
	bool isGrounded_ = false;

	// 重力加速度
	const float kGravity = -9.8f * 0.75f;

	// 最大落下速度
	const float kMaxFallSpeed = -20.0f;


protected:

	/// @brief 壁接触の判定
	void WallTouchCheck();

	/// @brief 壁接触の更新
	void WallTouchUpdate();

	/// @brief 壁に接触しているかどうか
	bool isWallTouch_ = false;

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

	/// @brief 体力HUD
	HP* hpHUD_ = nullptr;

	/// @brief 体力HUDの初期化
	void HpHudInit();

	/// @brief 体力HUDの位置を更新する
	void HpHudUpdate();


public:

	/// @brief デバッグUIを描画する
	/// @param placementData 
	/// @param placementList 
	/// @param history 
	/// @param isDirty 
	void DrawDebugUI(PlacementData* placementData, std::vector<PlacementData>& placementList, StageEditorHistory* history, bool* isDirty) override;
};

