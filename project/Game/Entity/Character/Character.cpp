#include "Character.h"
#include <algorithm>
#include <cmath>
#include <numbers>
#include "Entity/Weapon/Weapon.h"
#include "EffectManager/EffectManager.h"

#include "Action/Attack/Attack.h"
#include "Action/Move/Move.h"
#include "Action/Avoid/Avoid.h"

#include "StageEditor/StageData/StageData.h"
#include "StageEditor/StageEditorHistory/StageEditorHistory.h"

namespace
{
	/// @brief カメラ基準の入力方向をワールド方向へ変換する
	/// @param cameraLocalDirection
	/// @param cameraYaw
	/// @return
	Vector2 ToWorldMoveDirectionFromCamera(const Vector2& cameraLocalDirection, float cameraYaw)
	{
		// カメラ前方向(XZ平面)
		const Vector2 forward = Vector2(std::sin(cameraYaw), std::cos(cameraYaw));

		// カメラ右方向(XZ平面)
		const Vector2 right = Vector2(forward.y, -forward.x);

		// カメラ基準入力をワールド方向へ変換する
		return right * cameraLocalDirection.x + forward * cameraLocalDirection.y;
	}
}

// 静的メンバの定義
std::vector<Character*> Character::characters_{};

/// @brief 
/// @param position 
Character::Character(const InitData& initData) : Entity()
{
	// インスタンスリストに登録する
	characters_.push_back(this);

	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

	// マネージャのインスタンスを取得する
	motionManager_ = MotionManager::GetInstance();
	soundManager_ = SoundManager::GetInstance();

	// タグを指定する
	entityTag_ = EntityTag::Character;


	// 位置
	worldTransform_->translate_ = initData.position;

	// 回転
	worldTransform_->rotate_ = Vector3(0.0f, initData.rotateY, 0.0f);

	// 体力
	hp_ = initData.hp;

	// モデルデータ
	if (initData.model_)
	{
		// モデル
		model_ = initData.model_;

		// アニメーションの時間を取得する
		animationTime_ = engine_->GetAnimationTime(model_->param_->animation.hAnimation);
	}

	// 武器
	GrabWeapon(initData.weapon);

	// モーション
	hStandMotion_ = initData.hStandMotion;
	hStanceMotion_ = initData.hStanceMotion;
	hWalkMotion_ = initData.hWalkMotion;
	hDashMotion_ = initData.hDashMotion;
	hAvoidFrontMotion_ = initData.hAvoidFrontMotion;
	hAvoidBackMotion_ = initData.hAvoidBackMotion;
	hAvoidLeftMotion_ = initData.hAvoidLeftMotion;
	hAvoidRightMotion_ = initData.hAvoidRightMotion;
	hGuardMotion_ = initData.hGuardMotion;
	hGuardHitMotion_ = initData.hGuardHitMotion;

	hDamageLightMotion_ = motionManager_->GetMotion(MotionType::Stagger, "Front");
	hDamageHeavyMotion_ = motionManager_->GetMotion(MotionType::Stagger, "Front");
	hDownStaggerMotion_ = motionManager_->GetMotion(MotionType::Stagger, "Front_Down");

	hDownFallMotion_ = motionManager_->GetMotion(MotionType::DownFall, "Front");
	hDownLyingMotion_ = motionManager_->GetMotion(MotionType::DownLying, "Front");
	hDownGetUpMotion_ = motionManager_->GetMotion(MotionType::DowoGetUp, "Front");

	hGrabMotion_ = motionManager_->GetMotion(MotionType::Grab, "Front");
	hGrabbedMotion_ = motionManager_->GetMotion(MotionType::Grabbed, "Front");



	// 当たり判定グループ
	hurtboxHead_.collider_ = initData.hurtboxGroup->CreateInstance();
	hurtboxHead_.owner_ = this;
	hurtboxHead_.type_ = ColliderType::Hurtbox;

	hurtboxChest_.collider_ = initData.hurtboxGroup->CreateInstance();
	hurtboxChest_.owner_ = this;
	hurtboxChest_.type_ = ColliderType::Hurtbox;

	hurtboxRoot_.collider_ = initData.hurtboxGroup->CreateInstance();
	hurtboxRoot_.owner_ = this;
	hurtboxRoot_.type_ = ColliderType::Hurtbox;


	// 攻撃判定グループ
	hitboxGroup_ = initData.hitboxGroup;

	// 着地判定
	landingCollision_ = initData.landingCollision;
	landingCollision_->param_->center = GetWorldPosition();
	landingCollision_->param_->radius = Vector3(0.25f, 0.03f, 0.25f);
	

	// ブラックボードの生成
	blackboard_ = std::make_unique<Blackboard>();


	// ワールド座標を更新
	worldTransform_->Update();
}

/// @brief デストラクタ
Character::~Character()
{
	// 当たり判定の削除
	if (landingCollision_)landingCollision_->Delete();
	landingCollision_ = nullptr;

	if (hurtboxHead_.collider_) hurtboxHead_.collider_->Delete();
	hurtboxHead_.collider_ = nullptr;

	if (hurtboxChest_.collider_) hurtboxChest_.collider_->Delete();
	hurtboxChest_.collider_ = nullptr;

	if (hurtboxRoot_.collider_) hurtboxRoot_.collider_->Delete();
	hurtboxRoot_.collider_ = nullptr;

	// インスタンスリストから自分を除外する
	auto it = std::remove(characters_.begin(), characters_.end(), this);
	characters_.erase(it, characters_.end());
}

/// @brief 更新処理
void Character::Update()
{
	// 更新が無効なら何もしない
	if (!updateEnabled_)return;

	// デルタタイムを取得する
	const float dt = std::max(engine_->GetDeltaTime() * engine_->GetTimeScale(), 0.0f);
	const float unscaledDt = std::max(engine_->GetDeltaTime(), 0.0f);

	// 着地判定をチェックする
	LandingCheck();

	// 最後のまとめた処理
	auto FinalizeUpdate = [&]()
		{
			// 壊れたブキを持っている場合は、ブキを離す
			if (HasWeapon())
			{
				if (weapon_->IsBreak())
					ReleaseWeapon();
			}

			// スタイルチェンジの更新
			if (isStyleChanging_ && IsPlayer())
			{
				// プレイヤーはスローモーションの影響を受けない
				UpdateStyleChange(unscaledDt);
			}
			else
			{
				// プレイヤー以外はスローモーションの影響を受ける
				UpdateStyleChange(dt);
			}

			// 押し出し処理
			UpdatePushOut();

			// アニメーションの更新
			UpdateAnimation();

			// 基底クラスの更新処理
			Entity::Update();

			// 着地判定の位置を更新する
			if (landingCollision_)
				landingCollision_->param_->center = GetWorldPosition();

			if (hurtboxHead_.collider_)
			{
				auto collider = static_cast<Collision3DInstanceSphere*>(hurtboxHead_.collider_);
				collider->param_->radius = 0.25f;
				collider->param_->center = GetBonePosition(JointType::Head);
			}

			if (hurtboxChest_.collider_)
			{
				auto collider = static_cast<Collision3DInstanceSphere*>(hurtboxChest_.collider_);
				collider->param_->radius = 0.25f;
				collider->param_->center = GetBonePosition(JointType::Chest);
			}

			if (hurtboxRoot_.collider_)
			{
				auto collider = static_cast<Collision3DInstanceSphere*>(hurtboxRoot_.collider_);
				collider->param_->radius = 0.25f;
				collider->param_->center = GetBonePosition(JointType::Root);
			}
		};

	// 掴まれている場合の処理
	if (IsGrabbed())
	{
		// 掴まれタイマーが過ぎたら、掴まれ状態を解除する
		if (grabbedTimer_ >= escapeTimeLimit_)
		{
			// 振りほどかれた際の怯みを入れる
			grabber_->OnDamage(0, DamageReaction::LightStagger, 0.1f, Vector3(0.0f, 0.0f, -1.0f), GetWorldPosition());

			// 掴んでいる相手から離れる
			worldTransform_->rotate_ = Vector3(0.0f, grabber_->GetWorldTransform()->rotate_.y + std::numbers::pi_v<float>, 0.0f);
			worldTransform_->translate_.y = grabber_->GetWorldPosition().y;

			// 掴んでいる相手から離れる
			grabber_->grabbedTarget_ = nullptr;
			grabber_ = nullptr;
			grabbedTimer_ = 0.0f;

			// ダメージリアクションを解除する
			currentDamageReaction_ = DamageReactionState::None;
		}

		// まとめた後処理を呼んで終了
		FinalizeUpdate();
		return;
	}

	// 掴んでいる場合の処理
	if (IsGrabbing())
	{
		// 掴んでいる相手の位置を、掴んでいる自分の手の位置に合わせる
		Matrix4x4 handMatrix = GetBoneMatrix(JointType::HandR);
		Vector3 handPos(handMatrix.m[3][0], handMatrix.m[3][1], handMatrix.m[3][2]);
		grabbedTarget_->SetPosition(handPos + Vector3(0.0f, -1.2f, 0.1f));

		// 掴んでいる相手の向きを、掴んでいる自分の向きに合わせる (Y軸のみ180度反転させる)
		// Quaternionでの計算を避け、直接オイラー角を指定する
		grabbedTarget_->worldTransform_->rotate_ = Vector3(0.0f, worldTransform_->rotate_.y + std::numbers::pi_v<float>, 0.0f);
	}

	// ノックバックの更新
	if (knockbackVelocity_.Length() > 0.01f)
	{
		// ノックバックの移動
		SetPosition(GetPosition() + knockbackVelocity_ * dt);
		knockbackVelocity_ = knockbackVelocity_ * std::pow(0.1f, dt);
	}
	else
	{
		// ノックバックの速度が十分小さくなったら、ノックバックを終了する
		knockbackVelocity_ = Vector3(0.0f, 0.0f, 0.0f);
	}

	// ダメージリアクションの更新
	if (IsDamageReaction())
	{
		damageReactionTimer_ -= dt;

		// ダメージリアクションがダウン中の場合は、起き上がり条件をチェックし、それ以外の場合はダメージリアクションの時間が十分経過したかをチェックする
		bool shouldTransition = (currentDamageReaction_ == DamageReactionState::DownLyingFront || currentDamageReaction_ == DamageReactionState::DownLyingBack)
			? CheckGetUpCondition() : (damageReactionTimer_ <= 0.0f);

		if (shouldTransition)
		{
			switch (currentDamageReaction_)
			{
				// ダメージリアクションが終了したら、通常状態へ移行する
			case DamageReactionState::LightStaggerFront:
			case DamageReactionState::LightStaggerBack:
			case DamageReactionState::LightStaggerLeft:
			case DamageReactionState::LightStaggerRight:
			case DamageReactionState::HeavyStaggerFront:
			case DamageReactionState::HeavyStaggerBack:
			case DamageReactionState::HeavyStaggerLeft:
			case DamageReactionState::HeavyStaggerRight:
			case DamageReactionState::DownGettingUpFront:
			case DamageReactionState::DownGettingUpBack:
			case DamageReactionState::Parried:
			case DamageReactionState::Deflect:
				currentDamageReaction_ = DamageReactionState::None;
				break;

				// ダウン落下の時間が十分経過したら、ダウン中状態へ移行する
			case DamageReactionState::DownFallingFront:
				currentDamageReaction_ = DamageReactionState::DownLyingFront;
				damageReactionTimer_ = 2.0f;
				SetAnimation(hDownLyingMotion_, true, true);
				break;

				// ダウン落下の時間が十分経過したら、ダウン中状態へ移行する
			case DamageReactionState::DownFallingBack:
			case DamageReactionState::DownFallingLeft:
			case DamageReactionState::DownFallingRight:
				currentDamageReaction_ = DamageReactionState::DownLyingBack;
				damageReactionTimer_ = 2.0f;
				SetAnimation(hDownLyingMotion_, true, true);
				break;

				// ダウン中に攻撃を受けて怯んだ場合は、ダウン中状態へ移行する
			case DamageReactionState::DownStaggerFront:
				currentDamageReaction_ = DamageReactionState::DownLyingFront;
				damageReactionTimer_ = 2.0f;
				SetAnimation(hDownLyingMotion_, true, true);
				break;

				// ダウン中に攻撃を受けて怯んだ場合は、ダウン中状態へ移行する
			case DamageReactionState::DownStaggerBack:
				currentDamageReaction_ = DamageReactionState::DownLyingBack;
				damageReactionTimer_ = 2.0f;
				SetAnimation(hDownLyingMotion_, true, true);
				break;

				// ダウン中の時間が十分経過したら、起き上がりモーションへ移行する
			case DamageReactionState::DownLyingFront:
				currentDamageReaction_ = DamageReactionState::DownGettingUpFront;
				damageReactionTimer_ = 1.0f;
				SetAnimation(hDownGetUpMotion_, true, false);
				break;

				// ダウン中の時間が十分経過したら、起き上がりモーションへ移行する
			case DamageReactionState::DownLyingBack:
				currentDamageReaction_ = DamageReactionState::DownGettingUpBack;
				damageReactionTimer_ = 1.0f;
				SetAnimation(hDownGetUpMotion_, true, false);
				break;
			}
		}
	}

	// ガードタイマーの更新
	if (isGuard_)
		guardActiveTimer_ += dt;

	// 回避の更新
	if (isAvoid_)
		UpdateAvoid(dt);

	// ターゲットをロックオンする処理
	UpdateLockOnTargets();

	// ロックオンしているターゲットの方向を向く処理
	if (lockOnTarget_ && isStance_ && !IsGrabbing() && !IsIncapacitated())
	{
		Vector3 toTarget = lockOnTarget_->GetWorldPosition() - worldTransform_->GetWorldPosition();
		toTarget.y = 0.0f;

		// ターゲットの方向がある程度ある場合のみ、ターゲットの方向を向くようにする
		if ((toTarget.x * toTarget.x + toTarget.z * toTarget.z) > kRotateThreshold)
		{
			targetYaw_ = std::atan2(toTarget.x, toTarget.z);
			hasTargetYaw_ = true;
		}
	}

	// ターゲットの方向へ向く処理
	if (hasTargetYaw_)
	{
		float currentYaw = worldTransform_->rotate_.y;
		float deltaYaw = std::atan2(std::sin(targetYaw_ - currentYaw), std::cos(targetYaw_ - currentYaw));

		const float rotateLerpT = 1.0f - std::exp(-12.0f * dt);
		worldTransform_->rotate_.y = currentYaw + deltaYaw * rotateLerpT;

		// 角度が十分近くなったら、ターゲットの方向を向ききったとみなす
		if ((deltaYaw * deltaYaw) <= kRotateThreshold)
		{
			worldTransform_->rotate_.y = targetYaw_;
			hasTargetYaw_ = false;
		}
	}

	// ガードリアクション中は、攻撃してきた相手の方向を向くようにする
	if (isGuardReaction_)
	{
		float diff = targetRotationY_ - worldTransform_->rotate_.y;
		const float pi = std::numbers::pi_v<float>;

		// 角度の正規化
		while (diff > pi) diff -= 2.0f * pi;
		while (diff < -pi) diff += 2.0f * pi;

		worldTransform_->rotate_.y += diff * rotationSpeed_ * dt;
	}

	// 向きの更新
	direction_.x = std::sin(worldTransform_->rotate_.y);
	direction_.y = 0.0f;
	direction_.z = std::cos(worldTransform_->rotate_.y);

	// 速度の更新
	const float velocityLerpT = 1.0f - std::exp(-velocityLerpSpeed_ * dt);
	currentVelocity_ = Lerp(currentVelocity_, targetVelocity_, velocityLerpT);

	// 位置の更新
	worldTransform_->translate_ += currentVelocity_ * dt;

	// 落下処理
	FallUpdate(dt);

	// 最後のまとめた処理
	FinalizeUpdate();
}

/// @brief 更新処理開始前のリセット
void Character::StartUpdate()
{
	// 回避直後のフラグを更新する
	isJustAvoidedPrev_ = isJustAvoided_;
	isJustAvoided_ = false;
}

/// @brief ダメージを受ける
/// @param damage 
/// @param staggerTime
/// @param knockback
/// @param knockDirection
bool Character::OnDamage(int damage, DamageReaction damageReaction, float knockback, 
	const Vector3& knockDirection, const Vector3& enemyPosition, Character* attacker)
{
	// すでに死亡している場合は、ダメージを受けない
	if (IsDead())return false;

	// 攻撃をキャンセルする
	MoveStop();

	// 回避とダッシュのフラグをリセットする
	isAvoid_ = false;
	isDash_ = false;
	bufferedAttackInput_ = AttackInputType::None;

	// ガードしている場合は、ダメージを無効にして、ガードリアクションを行う
	if (IsGuard())
	{
		// 受け流し可能で、ガードが有効なタイミングで攻撃を受けた場合は、受け流し成功の処理を行う
		if (canParry_ && guardActiveTimer_ <= kJustGuardTime && attacker != nullptr)
		{
			// 受け流す
			ExecuteParry(attacker);

			// 必要であれば自分に専用の受け流し成功モーションを設定
			// SetAnimation(hParrySuccessMotion_, false, false);

			// ダメージ無効
			return false;
		}

		// 弾き可能で、ガードが有効なタイミングで攻撃を受けた場合は、弾き成功の処理を行う
		if (canDeflect_ && guardActiveTimer_ <= kJustGuardTime && attacker != nullptr)
		{
			// 受け流す
			ExecuteDeflect(attacker);

			// 必要であれば自分に専用の弾き成功モーションを設定
			// SetAnimation(hDeflectSuccessMotion_, false, false);
			
			// ダメージ無効
			return false;
		}


		Vector3 dirToAttacker = enemyPosition - GetWorldPosition();
		dirToAttacker.y = 0.0f; // 水平方向のみ

		if (dirToAttacker.Length() > 0.0f)
		{
			dirToAttacker = dirToAttacker.Normalize();
			targetRotationY_ = std::atan2(dirToAttacker.x, dirToAttacker.z);
		}

		isGuardReaction_ = true;
		guardReactionTimer_ = 0.0f;
		knockbackVelocity_ = -dirToAttacker * 2.0f;

		// ガードse
		soundManager_->SeGuard();

		// ガードエフェクト
		EffectManager::GetInstance()->CreateGuardEffect(GetWorldPosition() + Vector3(0.0f, 1.0f, 0.0f), worldTransform_->rotate_);

		SetAnimation(hGuardHitMotion_, false, true);

		return false; // ガード成功によりダメージ無効
	}


	// ダメージを受ける処理

	// ダウン中に攻撃を受けた場合は、ダウン怯み状態へ移行する
	if (currentDamageReaction_ == DamageReactionState::DownLyingFront || currentDamageReaction_ == DamageReactionState::DownStaggerFront)
	{
		currentDamageReaction_ = DamageReactionState::DownStaggerFront;
		SetAnimation(hDownStaggerMotion_, true, false);
		damageReactionTimer_ = 0.3f;

		// ダウン中はノックバックが入らない
		knockback = 0.0f;
	}
	else if (currentDamageReaction_ == DamageReactionState::DownLyingBack || currentDamageReaction_ == DamageReactionState::DownStaggerBack)
	{
		currentDamageReaction_ = DamageReactionState::DownStaggerBack;
		SetAnimation(hDownStaggerMotion_, true, false);
		damageReactionTimer_ = 0.3f;

		// ダウン中はノックバックが入らない
		knockback = 0.0f;
	}
	else
	{

		switch (damageReaction)
		{
			// 軽い怯みは、ノックバックも少なく、短い時間リアクションが続く
		case DamageReaction::LightStagger:
			currentDamageReaction_ = DamageReactionState::LightStaggerFront; // ここではとりあえず前方向の怯みを設定。
			SetAnimation(hDamageLightMotion_, true, false);
			damageReactionTimer_ = 0.3f;
			break;

			// 重い怯みは、軽い怯みよりも長い時間リアクションが続く
		case DamageReaction::HeavyStagger:
			currentDamageReaction_ = DamageReactionState::HeavyStaggerFront; // ここではとりあえず前方向の怯みを設定。
			SetAnimation(hDamageHeavyMotion_, true, false);
			damageReactionTimer_ = 1.0f;
			break;

			// ダウン落下は、落下モーションを再生してから、ダウン中状態へ移行する
		case DamageReaction::Down:
			currentDamageReaction_ = DamageReactionState::DownFallingFront; // ここではとりあえず前方向のダウンを設定。
			SetAnimation(hDownFallMotion_, true, false);
			damageReactionTimer_ = 0.3f;
			break;
		}
	}



	// 最終的な攻撃力を計算する
	int finalDamage = damage;

	// 武器の攻撃力を考慮する
	if (attacker != nullptr && attacker->HasWeapon())
	{
		finalDamage = static_cast<int>(static_cast<float>(damage) * attacker->GetWeapon()->GetAttackPower());

		// 武器の耐久力を1減らす
		attacker->GetWeapon()->TakeDamage(1);
	}

	// 体力を減らし、0未満にならないようにする
	hp_ = std::max(0, hp_ - finalDamage);

	// ノックバック処理
	if (knockback > 0.0f)
	{
		knockbackVelocity_ = knockDirection.Normalize() * (knockback * 10.0f);
	}

	// 死亡判定
	if (hp_ == 0)
	{
		isDead_ = true;
		SetAnimation(hDownFallMotion_, true, false);
	}

	return true; // ダメージが通った
}

/// @brief 受け流されたときの処理
/// @param pullPosition 
/// @param pushDirection 
/// @return 
void Character::OnParried(const Vector3& pullPosition, const Vector3& pushDirection)
{
	// すでに死亡している場合は、何もしない
	if (IsDead())return;

	// 攻撃や移動をキャンセルする
	MoveStop();

	// 回避とダッシュのフラグをリセットする
	isAvoid_ = false;
	isDash_ = false;
	bufferedAttackInput_ = AttackInputType::None;

	// 受け流し成功の位置を設定する
	SetPosition(pullPosition);

	// 受け流し成功のリアクションを設定する
	currentDamageReaction_ = DamageReactionState::Parried;
	damageReactionTimer_ = 1.0f; // 相手が無防備になる時間（調整可）

	// 受け流し成功のノックバックを設定する（相手を押し出す）
	knockbackVelocity_ = pushDirection * 4.0f;

	// 受け流し成功モーションを再生する
	SetAnimation(hDamageHeavyMotion_, true, false);
}

/// @brief 弾かれた時の処理
/// @param pushDirection 
void Character::OnDeflect(const Vector3& pushDirection, float knockBackPower)
{
	// すでに死亡している場合は、何もしない
	if (IsDead())return;

	// 攻撃や移動をキャンセルする
	MoveStop();

	// 回避とダッシュのフラグをリセットする
	isAvoid_ = false;
	isDash_ = false;
	bufferedAttackInput_ = AttackInputType::None;

	// 弾き成功のリアクションを設定する
	currentDamageReaction_ = DamageReactionState::Deflect;
	damageReactionTimer_ = 1.0f;

	// ノックバックを入れる
	knockbackVelocity_ = pushDirection * knockBackPower;

	// 弾き成功モーションを再生する
	SetAnimation(hGuardHitMotion_, true, false);
}

/// @brief 掴みダメージを受けた時の処理
/// @param damage 
void Character::OnGrabDamage(int damage)
{
	// 体力を減らし、0未満にならないようにする
	hp_ = std::max(0, hp_ - damage);

	// 死亡判定
	if (hp_ == 0)
		isDead_ = true;
}

/// @brief ダウンからの起き上がり条件を満たしているかどうか
/// @return 
bool Character::CheckGetUpCondition()
{
	// ダウン中の時間が十分経過しているかどうか
	return damageReactionTimer_ <= 0.0f &&
		currentDamageReaction_ == DamageReactionState::DownLyingFront || currentDamageReaction_ == DamageReactionState::DownLyingBack;
}

/// @brief 回避を開始する
/// @param direction 
/// @param distance 
/// @param time 
void Character::StartAvoid(const Vector3& direction, float distance, float time)
{
	// 新しい連続回避の開始時に回数を初期化する
	if (!isAvoid_ && currentAvoidCount_ == 0)
	{
		currentAvoidCount_ = 1;
	}

	// 回避開始時にダッシュは解除する
	isDash_ = false;

	// 回避パラメータを初期化する
	isAvoid_ = true;
	avoidElapsedTime_ = time;
	avoidDuration_ = time;
	avoidStartPosition_ = worldTransform_->translate_;
	avoidEndPosition_ = avoidStartPosition_ + Vector3(direction.x * distance, 0.0f, direction.z * distance);

	// 回避瞬間のフラグを立てる
	isJustAvoided_ = true;

	// 回避se
	soundManager_->SeAvoid();

	// 通常移動は停止して回避移動へ移行する
	MoveStop();
}

/// @brief 回避中の更新処理
/// @param deltaTime
void Character::UpdateAvoid(float deltaTime)
{
	// 回避時間が0以下だったら処理しない
	if (avoidDuration_ <= 0.0f)
		return;

	// 回避時間を進める
	avoidElapsedTime_ -= deltaTime;

	// 開始位置から終了位置まで線形補間で移動する
	const float t = std::clamp<float>(1.0f - (avoidElapsedTime_ / avoidDuration_), 0.0f, 1.0f);
	const float easeOutT = 1.0f - std::powf(1.0f - t, 3); // イーズアウト補間
	worldTransform_->translate_ = Lerp(avoidStartPosition_, avoidEndPosition_, easeOutT);

	// 到達したら回避フラグを下ろす
	if (t >= 1.0f)
	{
		// 連続回避が終了したので回避回数を回復する
		isAvoid_ = false;
		avoidElapsedTime_ = 0.0f;
		currentAvoidCount_ = 0;
	}
}

/// @brief 回避方向を取得する
/// @param moveInputDirection
/// @param hasMoveInput
/// @param cameraYaw
/// @return
Vector2 Character::GetAvoidDirection(const Vector2& moveInputDirection, bool hasMoveInput, float cameraYaw) const
{
	// 移動入力がある場合はその方向へ回避する
	if (hasMoveInput)
	{
		const Vector2 worldMoveDirection = ToWorldMoveDirectionFromCamera(moveInputDirection, cameraYaw);
		if (worldMoveDirection.Length() > 0.0f)
		{
			return worldMoveDirection.Normalize();
		}
	}

	// 移動入力がない場合は現在向いている方向の後ろへ回避する
	Vector2 backwardDirection = Vector2(-direction_.x, -direction_.z);
	if (backwardDirection.Length() <= 0.0f)
	{
		backwardDirection = Vector2(0.0f, -1.0f);
	}

	return backwardDirection.Normalize();
}

/// @brief 移動を停止させる
void Character::MoveStop()
{
	// 目標速度と現在の速度を0にする
	targetVelocity_ = Vector3(0.0f, 0.0f, 0.0f);
	currentVelocity_ = Vector3(0.0f, 0.0f, 0.0f);
}

/// @brief XZ平面の移動入力を設定する
/// @param direction
/// @param maxSpeed
void Character::SetMoveInputXZ(const Vector2& direction, float maxSpeed)
{
	// 入力された方向の長さを計算する
	const float length = direction.Length();

	// 長さが0の場合 や 地面に接していない場合は移動しない
	if (length <= 0.0f || !IsGrounded() || IsStyleChanging())
	{
		targetVelocity_ = Vector3(0.0f, 0.0f, 0.0f);
		return;
	}

	// 長さが1を超える場合は1にクランプする
	const float clampedLength = (length < 1.0f) ? length : 1.0f;
	const float moveSpeed = clampedLength * maxSpeed;

	// 入力された方向を正規化して、目標速度を計算する
	targetVelocity_.x = direction.x * moveSpeed;
	targetVelocity_.y = 0.0f;
	targetVelocity_.z = direction.y * moveSpeed;

	// つかまれていない場合は入力方向を向く。つかんでいる場合は入力方向の逆を向く
	if (!IsGrabbing())
	{
		// つかまれていない場合は入力方向を向く
		if ((targetVelocity_.x * targetVelocity_.x + targetVelocity_.z * targetVelocity_.z) > kRotateThreshold)
		{
			targetYaw_ = std::atan2(direction.x, direction.y);
			hasTargetYaw_ = true;
		}
	}
	else
	{
		// つかんでいる場合は入力方向の逆を向く
		if ((targetVelocity_.x * targetVelocity_.x + targetVelocity_.z * targetVelocity_.z) > kRotateThreshold)
		{
			targetYaw_ = std::atan2(-direction.x, -direction.y);
			hasTargetYaw_ = true;
		}
	}
}

// 構え中のロックオン候補を更新する
void Character::UpdateLockOnTargets()
{
	// 構え中でない場合、構えなしでロックオンできない設定の場合、ダウン中の場合、すでにロックオンしている相手が死んでいる場合は、ロックオンを解除する
	if ((!isStance_ && !canLockOnWithoutStance_) || IsDown() || (lockOnTarget_ && lockOnTarget_->IsDead()))
	{
		lockOnTarget_ = nullptr;
		return;
	}

	// すでにターゲット確定済みの場合は再検索しない
	if (lockOnTarget_)
		return;

	// ターゲットをクリアする
	lockOnTarget_ = nullptr;

	// 最も視線方向に近い相手を探す
	float bestDistance = std::numeric_limits<float>::max();
	float bestDot = -1.0f;

	const Vector3 selfPosition = GetWorldPosition();

	// ロックオン対象の側を決定する
	const bool isSelfPlayerSide = IsPlayerSide();

	for (Character* character : characters_)
	{
		// 無効または自分自身は除外する
		if (!character || character == this)continue;

		// 自分と同じ側の相手は除外する
		if (isSelfPlayerSide == character->IsPlayerSide()) continue;

		// 死んでいる相手は除外する
		if(character->IsDead())continue;

		// 自分から相手へのベクトルを計算する
		Vector3 toTarget = character->GetWorldPosition() - selfPosition;
		toTarget.y = 0.0f;

		// 距離の二乗を計算する
		const float distanceSq = toTarget.x * toTarget.x + toTarget.z * toTarget.z;
		if (distanceSq <= 0.0f)
			continue;

		// ロックオン可能な距離内にいる相手のみを候補にする
		if (IsPlayer())
		{
			// プレイヤーの場合は、目の前にいる相手のみロックオン候補にする

			// 目の前にいる相手のみリストに登録する
			const Vector3 toTargetDirection = toTarget.Normalize();
			if (Dot(direction_, toTargetDirection) <= 0.0f)
			{
				continue;
			}

			// 距離と相手ポインタを登録する
			const float distance = std::sqrt(distanceSq);

			// 視線方向との内積を計算する
			const float viewDot = Dot(direction_, toTargetDirection);

			// まず距離が最も近い相手を優先する
			if (distance < bestDistance)
			{
				bestDistance = distance;
				bestDot = viewDot;
				lockOnTarget_ = character;
			}
			else if (distance == bestDistance && viewDot > bestDot)
			{
				// 距離が同じ場合は、視線方向に最も近い相手を優先する
				bestDot = viewDot;
				lockOnTarget_ = character;
			}
		}
		else
		{
			// NPCの場合は、距離が近い相手をロックオン候補にする

			// 距離と相手ポインタを登録する
			const float distance = std::sqrt(distanceSq);

			// まず距離が最も近い相手を優先する
			if (distance < bestDistance)
			{
				bestDistance = distance;
				lockOnTarget_ = character;
			}
		}
	}
}

/// @brief アニメーションを設定する
/// @param hAnimation 
void Character::SetAnimation(AnimationHandle hAnimation, bool isReset, bool isLoop)
{
	if (!model_)return;

	if (!(model_->param_->animation.hAnimation == hAnimation))
	{
		// モデルのアニメーションハンドルを更新する
		model_->param_->animation.hAnimation = hAnimation;

		// アニメーションの時間を取得する
		animationTime_ = GrowthEngine::GetInstance()->GetAnimationTime(model_->param_->animation.hAnimation);
	}

	// アニメーションをリセットする
	if (isReset)
		model_->param_->animation.timer = 0.0f;

	// アニメーションのループ設定を更新する
	isAnimationLoop_ = isLoop;
}

/// @brief アクションの更新処理
void Character::ActionUpdate()
{
	// 現在の攻撃がある場合は更新する
	if (currentAttack_)
		currentAttack_->Update();

	// 現在の移動がある場合は更新する
	if (currentMove_)
		currentMove_->Update();

	// 掴み掴まれの状態の場合は回避させない
	if(IsGrabbed() || IsGrabbing())return;

	// 現在の回避がる場合は更新する
	if (currentAvoid_)
		currentAvoid_->Update();
}

/// @brief アニメーションの更新
void Character::UpdateAnimation()
{
	if (!model_)return;

	float dt = engine_->GetDeltaTime() * engine_->GetTimeScale();

	// プレイヤーのスタイルチェンジモーションはtimeScaleの影響を受けないようにする
	if (isStyleChanging_ && IsPlayer())
		dt = engine_->GetDeltaTime();

	// 死亡していない場合は、通常のモーションを再生する
	if (!IsDead())
	{
		// スタイルチェンジ中でない場合は、通常のモーションを再生する
		if (!IsStyleChanging())
		{
			if (!currentAttack_ && !IsDamageReaction() && !IsGrabbedDamage())
			{
				// 立ちモーションを再生する
				SetAnimation(hStandMotion_, false, true);

				//　移動している場合は歩きモーションを再生する
				if (targetVelocity_.Length() > 0.0f)
					SetAnimation(hWalkMotion_, false, true);

				// ダッシュしている場合はダッシュモーションを再生する
				if (isDash_)
					SetAnimation(hDashMotion_, false, true);

				// 構え中は構えモーションを優先して再生する
				if (isStance_)
					SetAnimation(hStanceMotion_, false, true);

				// 防御の待機モーションを再生する
				if (IsGuard())
					SetAnimation(hGuardMotion_, true, false);

				// 回避中は回避モーションを優先して再生する
				if (isAvoid_)
				{
					// 回避方向
					Vector3 avoidDirection = (avoidEndPosition_ - avoidStartPosition_).Normalize();

					if (avoidDirection.Length() > 0.0f)
					{
						// キャラクターの向き（前）と右方向
						Vector3 forward = direction_;
						Vector3 right = Vector3(forward.z, 0.0f, -forward.x); // 左手系(DirectX等)の右方向

						// 回避方向と各軸の内積を取り、ローカルの前後・左右の移動成分を出す
						float localZ = Dot(avoidDirection, forward); // +なら前、-なら後ろ
						float localX = Dot(avoidDirection, right);   // +なら右、-なら左

						// 前後成分と左右成分、どちらの影響が強いか（絶対値で比較）
						if (std::abs(localZ) > std::abs(localX))
						{
							// 前後への回避
							if (localZ > 0.0f)
							{
								// 前回避モーションを再生する
								SetAnimation(hAvoidFrontMotion_, false, false);
							}
							else
							{
								// 後ろ回避モーションを再生する
								SetAnimation(hAvoidBackMotion_, false, false);
							}
						}
						else
						{
							// 左右への回避
							if (localX > 0.0f)
							{
								// 右回避モーションを再生する
								SetAnimation(hAvoidRightMotion_, false, false);
							}
							else
							{
								// 左回避モーションを再生する
								SetAnimation(hAvoidLeftMotion_, false, false);
							}
						}
					}
				}
			}

			// 掴み攻撃や掴まれダメージの状態でない場合は、掴みや掴まれのモーションを再生する
			if (!IsGrabStrikeAttack())
			{
				// 掴まれている場合は掴まれモーションを再生する
				if (IsGrabbed() && !IsGrabbedDamage())
				{
					SetAnimation(hGrabbedMotion_, false, true);
				}
				else if (isGuardReaction_)
				{
					// 防御成功時のノックバック中
					guardReactionTimer_ += dt;
					if (guardReactionTimer_ > 0.3f) // ノックバック時間（任意）
					{
						isGuardReaction_ = false;
					}
				}
				else if (IsGrabbing())
				{
					// つかみモーションを再生する
					SetAnimation(hGrabMotion_, false, true);
				}
			}
		}
	}


	if (isAnimationLoop_)
	{
		// それ以外のモーションはtimeScaleの影響を受けるようにする
		model_->param_->animation.timer += dt;

		// アニメーションをループさせる
		model_->param_->animation.timer = std::fmod(model_->param_->animation.timer, animationTime_);
	}
	else
	{
		// アニメーションの時間よりもタイマーが大きい場合は更新しない
		if (model_->param_->animation.timer > animationTime_)
			return;

		// プレイヤーのスタイルチェンジモーションはtimeScaleの影響を受けないようにする
		model_->param_->animation.timer += dt;

		// アニメーションの時間を超えないようにする
		model_->param_->animation.timer = std::min(model_->param_->animation.timer, animationTime_);
	}
}

/// @brief ボーン行列を取得する
/// @param partName 
/// @return 
Matrix4x4 Character::GetBoneMatrix(const JointType& jointType) const
{
	// モーションマネージャからジョイント名を取得する
	std::string jointName = motionManager_->GetJointName(jointType);
	return model_->GetBoneWorldMatrix(jointName);
}

/// @brief ボーンの位置を取得する
/// @param jointType 
/// @return 
Vector3 Character::GetBonePosition(const JointType& jointType) const
{
	Matrix4x4 boneMatrix = GetBoneMatrix(jointType);
	return Vector3(boneMatrix.m[3][0], boneMatrix.m[3][1], boneMatrix.m[3][2]);
}

/// @brief ダウン中かどうか
/// @return 
bool Character::IsDown()const
{
	// ダウン中の状態は、ダウン落下、ダウン中、ダウン起き上がりのいずれかの状態である
	return 
		currentDamageReaction_ == DamageReactionState::DownFallingFront ||
		currentDamageReaction_ == DamageReactionState::DownFallingBack ||
		currentDamageReaction_ == DamageReactionState::DownFallingLeft ||
		currentDamageReaction_ == DamageReactionState::DownFallingRight ||

		currentDamageReaction_ == DamageReactionState::DownLyingFront ||
		currentDamageReaction_ == DamageReactionState::DownLyingBack ||

		currentDamageReaction_ == DamageReactionState::DownStaggerFront ||
		currentDamageReaction_ == DamageReactionState::DownStaggerBack ||

		currentDamageReaction_ == DamageReactionState::DownGettingUpFront ||
		currentDamageReaction_ == DamageReactionState::DownGettingUpBack;
}

/// @brief 倒れこみ中かどうか
/// @return 
bool Character::IsDownFalling() const
{
	return
		currentDamageReaction_ == DamageReactionState::DownFallingFront ||
		currentDamageReaction_ == DamageReactionState::DownFallingBack ||
		currentDamageReaction_ == DamageReactionState::DownFallingLeft ||
		currentDamageReaction_ == DamageReactionState::DownFallingRight; 
}

/// @brief 相手をつかむ
/// @param target 
void Character::ExecuteGrab(Character* target, float duration)
{
	grabbedTarget_ = target;

	// 掴まれた相手の処理を呼び出す
	target->OnGrabbed(this);
}

/// @brief 掴まれた相手の処理
void Character::OnGrabbed(Character* grabber)
{
	// 自分を掴んでいる相手を設定する
	grabber_ = grabber;
	grabbedTimer_ = 0.0f;

	// 掴まれた状態になったときの処理をここに書く
	isGuard_ = false;
	isDash_ = false;
	isAvoid_ = false;
}

/// @brief 掴んだ相手を離す
void Character::ReleaseGrab()
{
	// 掴んでいる相手がいる場合は、相手のgrabber_をクリアする
	if (grabbedTarget_) 
	{
		grabbedTarget_->grabber_ = nullptr;
		grabbedTarget_ = nullptr;
	}
}

/// @brief 防御を設定する
/// @param isGuard 
void Character::SetGuard(bool isGuard)
{
	if (isGuard && !isGuard_) guardActiveTimer_ = 0.0f; // ガードした瞬間にリセット
	isGuard_ = isGuard;
}

/// @brief 掴んだ状態の攻撃をしているかどうか
/// @return 
bool Character::IsGrabStrikeAttack() const
{
	if (!currentAttack_)return false;

	// 現在の攻撃が掴み攻撃かどうかは、攻撃のタイプが掴み攻撃かどうかで判断する
	return currentAttack_->GetType() == AttackType::GrabStrike;
}

/// @brief 掴まれた状態で攻撃されているかどうか
/// @return 
bool Character::IsGrabbedDamage()const
{
	if (!grabber_)return false;

	// 掴まれた状態で攻撃されているかどうかは、掴んでいる相手の攻撃が掴み攻撃かどうかで判断する
	return grabber_->IsGrabStrikeAttack();
}

/// @brief 武器を掴む
/// @param weapon 
void Character::GrabWeapon(Weapon* weapon)
{
	// 無効な武器の場合は処理しない
	if (weapon == nullptr)return;

	// 武器を持っていないときは処理しない
	if (HasWeapon())return;

	weapon_ = weapon;

	// 武器の持ち主を自分に設定する
	weapon_->SetOwner(this);
	weapon_->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
}

/// @brief 武器を離す
void Character::ReleaseWeapon(const Vector3& blowVelocity)
{
	// 武器を持っていないときは処理しない
	if (!HasWeapon())return;

	// 武器の所有者をクリアする
	weapon_->SetOwner(nullptr);
	weapon_->SetPosition(weapon_->GetWorldPosition());

	// 吹き飛び速度が指定されている場合は武器を飛ばす
	if (blowVelocity.Length() > 0.001f) 
	{
		weapon_->BlowAway(blowVelocity);
	}

	weapon_ = nullptr;
}

/// @brief 受け流しを実行する
/// @param attacker 
void Character::ExecuteParry(Character* attacker)
{
	if (!attacker) return;

	// 自分の位置と向きを取得する
	Vector3 myPos = GetWorldPosition();
	Vector3 myForward = GetDirection();

	// 引き込む位置
	Vector3 pullPos = myPos - myForward * 0.2f;

	// 受け流され処理を実行
	attacker->OnParried(pullPos, attacker->GetDirection());

	// se受け流し
	soundManager_->SeParried();
}

/// @brief 弾きを実行する
/// @param attacker 
void Character::ExecuteDeflect(Character* attacker)
{
	if (!attacker) return;

	// 相手の攻撃を弾いたことによる反動（ノックバック方向）を計算
	Vector3 pushDir = attacker->GetWorldPosition() - GetWorldPosition();
	pushDir.y = 0.0f;

	// ノックバック方向を正規化する
	pushDir = pushDir.Normalize();

	// 相手に弾きのリアクションを与える
	attacker->OnDeflect(pushDir, 5.0f);

	// 相手が武器を持っている場合は、武器を落とさせる
	if (attacker->HasWeapon())
	{
		// 自分から相手への水平ベクトルを計算（吹き飛ぶ大まかな方向）
		Vector3 blowDir = attacker->GetWorldPosition() - GetWorldPosition();
		blowDir.y = 0.0f;

		// 吹き飛ぶ方向を正規化する。もし位置が完全に重なっている場合は、相手の後ろ方向にする
		if (blowDir.Length() > 0.01f) 
		{
			blowDir = blowDir.Normalize();
		}
		else 
		{
			// 位置が完全に重なっている場合は相手の後ろ方向にする
			blowDir = -attacker->GetDirection();
		}

		// 吹き飛ぶ方向に自分の右方向を少し加える
		Vector3 myRight = Vector3(GetDirection().z, 0.0f, -GetDirection().x);
		blowDir = (blowDir + myRight * 0.5f).Normalize();

		// 吹き飛ぶ速度を計算（調整可能な定数を使用）
		float horizontalForce = 5.0f;
		float upwardForce = 6.0f;
		Vector3 blowVelocity = (blowDir * horizontalForce) + Vector3(0.0f, upwardForce, 0.0f);

		// 武器を吹き飛ばす
		attacker->ReleaseWeapon(blowVelocity);
	}

	// プレイヤーが攻撃を弾いた場合は、スローモーションを開始する
	if(IsPlayer())
		GrowthEngine::GetInstance()->StartSlowMotion(0.1f, 0.5f);

	// se弾き
	soundManager_->SeDeflect();
}

/// @brief スタイルチェンジを開始する
/// @param style 
void Character::StartStyleChange(FightStyle style)
{
	// すでにそのスタイルの場合や、スタイルチェンジ中の場合は何もしない
	if (currentStyle_ == style || isStyleChanging_) return;

	// ダウン中や掴み・掴まれ中など、スタイルチェンジを許容しない状態の場合は何もしない
	if (IsDown() || IsGrabbed()) return;

	// 移動を止める
	MoveStop();

	nextStyle_ = style;
	styleChangeTimer_ = kStyleChangeDuration;
	isStyleChanging_ = true;

	// スタイルチェンジ開始のイベントを発生させる
	StyleChangeStart();
}

/// @brief スタイルチェンジ開始のイベント
void Character::StyleChangeStart()
{
	switch(nextStyle_)
	{
		// 旋嵐
	case FightStyle::Tempest:
		SetAnimation(motionManager_->GetMotion(MotionType::StyleChange, "Senran"), true, false);
		soundManager_->SeStyleChangeSenran();
		break;

		// 撃鉄
	case FightStyle::Hammer:
		SetAnimation(motionManager_->GetMotion(MotionType::StyleChange, "Gekitetu"), true, false);
		soundManager_->SeStyleChangeGekitetu();
		break;
	}
}

/// @brief スタイルが変化したときの処理
/// @param newStyle 
void Character::OnStyleChanged(FightStyle newStyle)
{
	switch (newStyle)
	{
		// 旋嵐
	case FightStyle::Tempest:
		canParry_ = true; // 旋嵐スタイルは受け流しが可能になる
		canDeflect_ = false; // 旋嵐スタイルは弾きが不可能になる
		break;

		// 撃鉄
	case FightStyle::Hammer:
		canParry_ = false; // 撃鉄スタイルは受け流しが不可能になる
		canDeflect_ = true; // 撃鉄スタイルは弾きが可能になる
		break;
	}
}

/// @brief スタイルチェンジの更新処理
/// @param dt 
void Character::UpdateStyleChange(float dt)
{
	// スタイルチェンジ中でない場合は何もしない
	if (!isStyleChanging_) return;

	styleChangeTimer_ -= dt;

	// スタイルチェンジの時間が十分経過したら、スタイルを変更する
	if (styleChangeTimer_ <= 0.0f)
	{
		// スタイルを変更する
		currentStyle_ = nextStyle_;
		isStyleChanging_ = false;

		// プレイヤーやNPCにスタイルが変更されたことを通知
		OnStyleChanged(currentStyle_);
	}
}

/// @brief 武器所持の更新
void Character::UpdateWeapon()
{
	// 武器を持っていないときは処理しない
	if (!weapon_)return;
}

/// @brief 落下の更新
/// @param deltaTime 
void Character::FallUpdate(float deltaTime)
{
	// 重力による落下処理
	if (!isGrounded_)
	{
		// 落下速度を更新する
		velocityY_ += kGravity * deltaTime;
		if (velocityY_ < kMaxFallSpeed) velocityY_ = kMaxFallSpeed;

		// Y方向の位置を更新する
		worldTransform_->translate_.y += velocityY_ * deltaTime;
	}
}

/// @brief 着地判定の更新
void Character::LandingCheck()
{
	// 着地しているかどうかのフラグをリセットする
	isGrounded_ = false;

	// コリジョンがないと処理しない
	if (!landingCollision_)return;

	// コリジョンの状態を確認する
	if (landingCollision_->isCollision_)
	{
		// コリジョンの当たり判定がAABBであることを前提に、床との接触位置を計算する
		auto floorCollision = static_cast<Collision3DInstanceAABB*>(landingCollision_->hitOpponent_);
		worldTransform_->translate_.y = floorCollision->param_->center.y + floorCollision->param_->radius.y;

		// 着地していると判定する
		isGrounded_ = true;

		// Y方向の速度をリセットする（着地したので落下を止める）
		velocityY_ = 0.0f;
	}
}

/// @brief 押し出し判定処理
void Character::UpdatePushOut()
{
	// キャラクターの押し出し半径（ゲームのモデルサイズに合わせて調整してください）
	constexpr float kHeight = 1.6f; // キャラクターの高さ
	constexpr float kPushRadius = 0.25f; // 押し出し半径
	constexpr float kDistanceLimit = kPushRadius * 2.0f; // 2人の半径の和

	// 全キャラクターのリストを取得
	const auto& characters = Character::GetCharacters();

	for (auto* other : characters)
	{
		// 自分自身とは判定しない
		if (this == other) continue;

		// ダウン中や掴み・掴まれ中など、めり込みを許容したい状態の場合は判定をスキップする
		if (IsDown() || other->IsDown() ||
			IsGrabbed() || other->IsGrabbed() ||
			IsGrabbing() || other->IsGrabbing() ||
			IsParried() || other->IsParried())
		{
			continue;
		}

		// 自分と相手の位置を取得する（Y軸はキャラクターの中心の高さに合わせる）
		Vector3 myPos = GetPosition();
		Vector3 otherPos = other->GetPosition();

		// Y軸方向の重なりもあるか確認する（高さが同じくらいの相手のみ押し出す）
		if (myPos.y <= otherPos.y + kHeight && myPos.y + kHeight >= otherPos.y)
		{
			// XZ平面での距離を計算（高さ(Y軸)は無視して円柱状に判定する）
			Vector3 diff = myPos - otherPos;
			diff.y = 0.0f;

			// 距離の二乗を計算する（平方根を取る前に比較して効率化する）
			float distSq = diff.x * diff.x + diff.z * diff.z;

			// 距離が0より大きく、かつ押し出し半径の2倍以内の場合は押し出す
			if (distSq > 0.0f && distSq < (kDistanceLimit * kDistanceLimit))
			{
				float dist = std::sqrt(distSq);

				// めり込んでいる距離を計算
				float penetration = kDistanceLimit - dist;

				// 押し出し方向の単位ベクトルを計算
				Vector3 pushDir = diff / dist;

				// お互いのUpdateで処理されるため、自分をめり込み量の半分だけ移動させる
				Vector3 newPos = myPos + pushDir * (penetration * 0.5f);
				SetPosition(newPos); // 位置を更新
			}
		}
	}
}

/// @brief デバッグ用のUIを描画する
/// @param placementData 
/// @param placementList 
/// @param history 
/// @param isDirty 
void Character::DrawDebugUI(PlacementData* placementData, std::vector<PlacementData>& placementList, StageEditorHistory* history, bool* isDirty)
{
#ifdef _DEVELOPMENT

	// キャラクターの状態を表示する
	Entity::DrawDebugUI(placementData, placementList, history, isDirty);

	ImGui::Separator();

	// 体力を表示・編集する
	if (ImGui::IsItemActivated()) { history->SaveHistory(placementList); *isDirty = true; }
	ImGui::DragInt("HP", &hp_, 1, 0, 1000000);
	if(ImGui::IsItemDeactivatedAfterEdit())placementData->hp = hp_;

#endif
}