#pragma once
#define NOMINMAX
#include "../Entity.h"
#include "BlackBoard/BlackBoard.h"
#include "MotionManager/MotionManager.h"
#include "Action/Attack/ComboAttack/ComboAttack.h"
#include "Action/Attack/GrabAttack/GrabAttack.h"
#include "AppCollider/AppCollider.h"

class Attack;
class Move;
class Avoid;

class Character : public Entity
{
public:

	/// @brief キャラクターのタグ
	enum class CharacterTag
	{
		PlayerSide,
		EnemySide,
	};

	/// @brief 初期化用データ
	struct InitData
	{
		/// @brief 位置
		Vector3 position = Vector3(0.0f, 0.0f, 0.0f);

		/// @brief 体力
		int hp = 0;

		/// @brief 回避時間
		float avoidDuration = 0.3f;

		/// @brief 回避距離
		float avoidDistance = 1.5f;
		
		/// @brief モデル
		Render3DSkinningModel* model_ = nullptr;

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

		/// @brief 当たり判定グループ
		Collision3DBaseSphere* hurtboxGroup = nullptr;

		/// @brief 攻撃判定グループ
		Collision3DBaseSphere* hitboxGroup = nullptr;
	};


public:

	/// @brief コンストラクタ
	/// @param initData 
	Character(const InitData& initData);

	/// @brief デストラクタ
	virtual ~Character() override;

	/// @brief 更新処理
	virtual void Update() override;

	/// @brief ダメージを受けたときの処理
	/// @param damage 
	/// @param damageReaction 
	/// @param knockback 
	/// @param knockDirection 
	/// @param enemyPosition 
	virtual bool OnDamage(int damage, DamageReaction damageReaction, float knockback, const Vector3& knockDirection, const Vector3& enemyPosition);

	/// @brief 全キャラクターのリストを取得
	static const std::vector<Character*>& GetCharacters() { return characters_; }

	/// @brief 回避を開始する
	/// @param moveInputDirection
	/// @param hasMoveInput
	/// @param cameraYaw
	void StartAvoid(const Vector2& moveInputDirection, bool hasMoveInput, float cameraYaw);

    /// @brief 連続回避を試行する
	/// @param moveInputDirection
	/// @param hasMoveInput
	/// @param cameraYaw
	void ReserveNextAvoid(const Vector2& moveInputDirection, bool hasMoveInput, float cameraYaw);

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

	/// @brief 攻撃しているかどうか
	/// @return 
	bool IsAttack()const { return currentAttack_ != nullptr; }

	/// @brief 回避しているかどうか
	/// @return 
	bool IsAvoid() const { return isAvoid_; }

	/// @brief 構えているかどうか
	/// @return 
	bool IsStance() const { return isStance_; }

	/// @brief 死亡しているかどうか
	/// @return 
	bool IsDead() const { return isDead_; }

	/// @brief 現在の攻撃を設定する
	/// @param attack 
	void SetCurrentAttack(Attack* attack) { currentAttack_ = attack; }

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
	Matrix4x4 GetBoneMatrix(const std::string& partName) const;

	/// @brief ダメージリアクション中かどうか
	/// @return 
	bool IsDamageReaction() const { return currentDamageReaction_ != DamageReaction::None; }

	/// @brief ダウン中かどうか
	/// @return 
	bool IsDown()const { return currentDamageReaction_ == DamageReaction::DownFalling || currentDamageReaction_ == DamageReaction::DownLying || currentDamageReaction_ == DamageReaction::DownGettingUp; }

	/// @brief ダメージを受けているかどうか
	/// @return 
	AppCollider& GetHurtbox() { return hurtbox_; }

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

	/// @brief 掴んだ相手を離す
	void ReleaseGrab();

	/// @brief 防御しているかどうか
	/// @return 
	bool IsGuard() const { return isGuard_; }

	/// @brief 防御を設定する
	/// @param isGuard 
	void SetGuard(bool isGuard) { isGuard_ = isGuard; }


protected:

	/// @brief エンジン
	const GrowthEngine* engine_ = nullptr;

	/// @brief モーションマネージャ
	MotionManager* motionManager_;

	// キャラクターのタグ
	CharacterTag characterTag_;

	/// @brief ブラックボード
	std::unique_ptr<Blackboard> blackboard_ = nullptr;

	bool isDead_ = false;


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


protected:

	/// @brief 回避フラグ
	bool isAvoid_ = false;

	/// @brief 回避開始位置
	Vector3 avoidStartPosition_ = Vector3(0.0f, 0.0f, 0.0f);

	/// @brief 回避終了位置
	Vector3 avoidEndPosition_ = Vector3(0.0f, 0.0f, 0.0f);

	/// @brief 回避経過時間
	float avoidElapsedTime_ = 0.0f;

	/// @brief 回避時間
	float avoidDuration_ = 0.3f;

	/// @brief 回避距離
	float avoidDistance_ = 1.5f;

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


protected:

	// バッファされた攻撃入力
	AttackInputType bufferedAttackInput_ = AttackInputType::None;



protected:

	/// @brief アクションの更新処理
	void ActionUpdate();

	/// @brief 今の攻撃
	Attack* currentAttack_ = nullptr;

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


protected:

	/// @brief ダウンからの起き上がり条件を満たしているかどうか
	/// @return 
	virtual bool CheckGetUpCondition();

	/// @brief ノックバックの速度
	Vector3 knockbackVelocity_ = Vector3(0.0f, 0.0f, 0.0f);

	/// @brief 現在のダメージリアクション
	DamageReaction currentDamageReaction_ = DamageReaction::None;

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
	AppCollider hurtbox_;

	/// @brief 攻撃判定グループ
	Collision3DBaseSphere* hitboxGroup_ = nullptr;


protected:

	/// @brief 押し出し判定処理
	void UpdatePushOut();
};

