#include "Character.h"
#include <algorithm>
#include <cmath>
#include <numbers>
#include "Scene/GameScene/GameScene.h"
#include "Entity/Weapon/Weapon.h"
#include "EffectManager/EffectManager.h"

#include "Action/Attack/Attack.h"
#include "Action/Move/Move.h"
#include "Action/Avoid/Avoid.h"

#include "StageEditor/StageData/StageData.h"
#include "StageEditor/StageEditorHistory/StageEditorHistory.h"

#include "HUD/HP/HP.h"

#include "CharacterStateMachine/CharacterState/CharacterStateNone/CharacterStateNone.h"
#include "CharacterStateMachine/CharacterState/CharacterStateGrabbed/CharacterStateGrabbed.h"
#include "CharacterStateMachine/CharacterState/CharacterStateGrabbing/CharacterStateGrabbing.h"
#include "CharacterStateMachine/CharacterState/CharacterStateGuard/CharacterStateGuard.h"
#include "CharacterStateMachine/CharacterState/CharacterStateLightDamage/CharacterStateLightDamage.h"
#include "CharacterStateMachine/CharacterState/CharacterStateHeavyDamage/CharacterStateHeavyDamage.h"
#include "CharacterStateMachine/CharacterState/CharacterStateDownFalling/CharacterStateDownFalling.h"
#include "CharacterStateMachine/CharacterState/CharacterStateDownLying/CharacterStateDownLying.h"
#include "CharacterStateMachine/CharacterState/CharacterStateDownGettingUp/CharacterStateDownGettingUp.h"
#include "CharacterStateMachine/CharacterState/CharacterStateDownStagger/CharacterStateDownStagger.h"
#include "CharacterStateMachine/CharacterState/CharacterStateBlownAway/CharacterStateBlownAway.h"
#include "CharacterStateMachine/CharacterState/CharacterStateBlownFalling/CharacterStateBlownFalling.h"
#include "CharacterStateMachine/CharacterState/CharacterStateRepelled/CharacterStateRepelled.h"
#include "CharacterStateMachine/CharacterState/CharacterStateDeflected/CharacterStateDeflected.h"

// 静的メンバの定義
std::vector<Character*> Character::characters_{};

/// @brief 
/// @param position 
Character::Character() : Entity()
{
	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

	// マネージャのインスタンスを取得する
	motionManager_ = MotionManager::GetInstance();
	soundManager_ = SoundManager::GetInstance();
	effectManager_ = EffectManager::GetInstance();

	// タグを指定する
	entityTag_ = EntityTag::Character;

	// シェイクの生成と初期化
	shake_ = std::make_unique<Shake>();

	// キャラクターの移動処理の生成
	movement_ = std::make_unique<CharacterMovement>(this);

	// ブラックボードの生成
	blackboard_ = std::make_unique<Blackboard>();

	// ステートマシンの生成
	stateMachine_ = std::make_unique<CharacterStateMachine>();
	stateMachine_->AddState("None", std::make_unique<CharacterStateNone>(this));
	stateMachine_->AddState("Grabbed", std::make_unique<CharacterStateGrabbed>(this));
	stateMachine_->AddState("Grabbing", std::make_unique<CharacterStateGrabbing>(this));
	stateMachine_->AddState("Guard", std::make_unique<CharacterStateGuard>(this, 
		motionManager_->GetMotion(MotionType::Guard, "BothHands"),
		motionManager_->GetMotion(MotionType::Stand, "Standing")));
	stateMachine_->AddState("LightDamage", std::make_unique<CharacterStateLightDamage>(this,
		motionManager_->GetMotion(MotionType::Stagger, "Front"),
		motionManager_->GetMotion(MotionType::Stagger, "Front"),
		motionManager_->GetMotion(MotionType::Stagger, "Front"),
		motionManager_->GetMotion(MotionType::Stagger, "Front")));
	stateMachine_->AddState("HeavyDamage", std::make_unique<CharacterStateHeavyDamage>(this,
		motionManager_->GetMotion(MotionType::Stagger, "Front_Heavy"),
		motionManager_->GetMotion(MotionType::Stagger, "Front_Heavy"),
		motionManager_->GetMotion(MotionType::Stagger, "Front_Heavy"),
		motionManager_->GetMotion(MotionType::Stagger, "Front_Heavy")));
	stateMachine_->AddState("DownFalling", std::make_unique<CharacterStateDownFalling>(this,
		motionManager_->GetMotion(MotionType::DownFall, "Front"),
		motionManager_->GetMotion(MotionType::DownFall, "Front"),
		motionManager_->GetMotion(MotionType::DownFall, "Front"),
		motionManager_->GetMotion(MotionType::DownFall, "Front")));
	stateMachine_->AddState("DownLying", std::make_unique<CharacterStateDownLying>(this,
		motionManager_->GetMotion(MotionType::DownLying, "Front"),
		motionManager_->GetMotion(MotionType::DownLying, "Front")));
	stateMachine_->AddState("DownGettingUp", std::make_unique<CharacterStateDownGettingUp>(this,
		motionManager_->GetMotion(MotionType::DowoGetUp, "Front"),
		motionManager_->GetMotion(MotionType::DowoGetUp, "Front")));
	stateMachine_->AddState("DownStagger", std::make_unique<CharacterStateDownStagger>(this,
		motionManager_->GetMotion(MotionType::Stagger, "Front_Down"),
		motionManager_->GetMotion(MotionType::Stagger, "Front_Down")));
	stateMachine_->AddState("BlownAway", std::make_unique<CharacterStateBlownAway>(this,
		motionManager_->GetMotion(MotionType::DownFall, "Front"),
		motionManager_->GetMotion(MotionType::DownFall, "Front")));
	stateMachine_->AddState("BlownFalling", std::make_unique<CharacterStateBlownFalling>(this,
		motionManager_->GetMotion(MotionType::DownLying, "Front"),
		motionManager_->GetMotion(MotionType::DownLying, "Front")));
	stateMachine_->AddState("Repelled", std::make_unique<CharacterStateRepelled>(this, motionManager_->GetMotion(MotionType::Stagger, "Front")));
	stateMachine_->AddState("Deflected", std::make_unique<CharacterStateDeflected>(this, motionManager_->GetMotion(MotionType::Stagger, "Front")));
	stateMachine_->ChangeState("None");
}

/// @brief デストラクタ
Character::~Character()
{
	// 当たり判定の削除
	if (landingCollision_)landingCollision_->Delete();
	landingCollision_ = nullptr;

	if (wallTouchCollision_) wallTouchCollision_->Delete();
	wallTouchCollision_ = nullptr;

	// 死亡処理
	Dead();
}

/// @brief アニメーションの初期化
/// @param animData 
void Character::SetAnimationHandle(const AnimationHandleData& animData)
{
	hStandMotion_ = animData.hStandMotion;
	hStanceMotion_ = animData.hStanceMotion;
	hWalkMotion_ = animData.hWalkMotion;
	hDashMotion_ = animData.hDashMotion;
	hAvoidFrontMotion_ = animData.hAvoidFrontMotion;
	hAvoidBackMotion_ = animData.hAvoidBackMotion;
	hAvoidLeftMotion_ = animData.hAvoidLeftMotion;
	hAvoidRightMotion_ = animData.hAvoidRightMotion;
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

	// 壁接触の当たり判定をチェックする
	WallTouchCheck();

	// 最後のまとめた処理
	auto FinalizeUpdate = [&]()
		{
			// 移動の更新
			movement_->Update(dt);

			// レイジゲージの更新
			RageGageUpdate(dt);

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

			// ダッシュ中に攻撃をした場合は、ダッシュを解除する
			if (IsAttack())
			{
				isDash_ = false;
			}

			// 押し出し処理
			UpdatePushOut();

			// アニメーションの更新
			UpdateAnimation();

			// 基底クラスの更新処理
			Entity::Update();

			// シェイクの更新
			if (model_)model_->param_->modelTransform.translate = shake_->GetShakeOffset();
			shake_->Update(dt);

			// 現在の体力をHUDに反映する
			if (hpHUD_)
			{
				hpHUD_->SetCurrentHP(hp_);

				// 体力HUDの位置を更新する
				HpHudUpdate();
			}



			// 当たり判定の更新
			UpdateCollisionPosition(landingCollision_);
			UpdateCollisionPosition(wallTouchCollision_);
			UpdateCollisionPosition(eventTriggerCollision_);
			UpdateHurtbox(hurtboxHead_, JointType::Head);
			UpdateHurtbox(hurtboxChest_, JointType::Chest);
			UpdateHurtbox(hurtboxRoot_, JointType::Root);

			// ダッシュ中のエフェクト更新
			if (isDash_ && !isStance_)
			{
				dashTimer_ -= dt;
				if (dashTimer_ <= 0.0f)
				{
					effectManager_->DashSmoke000(GetWorldPosition() + Vector3(0.0f, 0.0f, 0.0f));
					dashTimer_ = 0.15f;
				}
			}

			// 死亡処理の更新
			if (isDead_)
			{
				// 死亡タイマーを減算する
				deadTimer_ -= dt;

				// 死亡タイマーが0以下になったら、終了フラグを立てる
				if (deadTimer_ <= 0.0f)
				{
					isFinished_ = true;
				}
			}
		};


	// ダウン着地のエフェクトを生成する
	if (IsBlownFalling() && IsGrounded())
	{
		// ダウン着地のエフェクトを生成する
		effectManager_->ImpactGround000(GetBonePosition(JointType::Root));
		effectManager_->ImpactGround001(GetBonePosition(JointType::Root));
		effectManager_->ImpactGround002(GetBonePosition(JointType::Root));
		effectManager_->ImpactGround003(GetBonePosition(JointType::Root));
		effectManager_->ImpactGround004(GetBonePosition(JointType::Root));
		effectManager_->ImpactGround005(GetBonePosition(JointType::Root));
		effectManager_->ImpactGround006(GetBonePosition(JointType::Root));
	}


	// ステートマシンの更新
	stateMachine_->Update(dt);

	// ステートマシンがNone状態でない場合は、体力HUDの更新を行い、処理を終了する
	if (stateMachine_->GetCurrentStateName() != "None")
	{
		FinalizeUpdate();
		return;
	}

	// 回避の更新
	if (isAvoid_)
		UpdateAvoid(dt);

	// ターゲットをロックオンする処理
	UpdateLockOnTargets();

	/// ターゲットの方向を向く処理
	movement_->TargetDirection(dt);

	// 壁接触の処理
	WallTouchUpdate();

	// 最後のまとめた処理
	FinalizeUpdate();
}

/// @brief 更新処理開始前のリセット
void Character::StartUpdate()
{
	// 回避直後のフラグを更新する
	isJustAvoidedPrev_ = isJustAvoided_;
	isJustAvoided_ = false;

	// 攻撃直後のフラグを更新する
	isPrevHitAttack_ = isHitAttack_;
	isHitAttack_ = false;

	// ダメージを受けた直後のフラグを更新する
	isPrevHitDamage_ = isHitDamage_;
	isHitDamage_ = false;

	// ガード成功のフラグを更新する
	isPrevGuardHit_ = isGuardHit_;
	isGuardHit_ = false;

	// 弾き成功のフラグを更新する
	isPrevHitRepel_ = isHitRepel_;
	isHitRepel_ = false;

	// レイジモード開始成功のフラグを更新する
	isPrevSuccessRageModeStart_ = isSuccessRageModeStart_;
	isSuccessRageModeStart_ = false;
}

/// @brief ダメージを受ける
/// @param damage 
/// @param staggerTime
/// @param knockback
/// @param knockDirection
bool Character::OnDamage(int damage, DamageReaction damageReaction, float knockback, 
	const Vector3& knockDirection, const Vector3& enemyPosition, Character* attacker, std::optional<Vector3> hitPosition, bool isGuardBreak, bool isThrow)
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
		// 防御状態のステートを取得する
		auto guardState = static_cast<CharacterStateGuard*>(stateMachine_->GetCurrentState());

		// 攻撃者をロックオンターゲットに設定する
		if (attacker)lockOnTarget_ = attacker;

		// ガード成功のフラグを立てる
		isGuardHit_ = true;

		// 受け流し可能で、ガードが有効なタイミングで攻撃を受けた場合は、受け流し成功の処理を行う
		if (canDeflect_ && guardState->CanJustGuard() && attacker != nullptr)
		{
			// 受け流す
			ExecuteDeflect(attacker);

			// 受け流し成功モーションを設定
			// SetAnimation(hDeflectSuccessMotion_, false, false);

			// ダメージ無効
			return false;
		}

		// 弾き可能で、ガードが有効なタイミングで攻撃を受けた場合は、弾き成功の処理を行う
		if (canRepel_ && guardState->CanJustGuard() && attacker != nullptr)
		{
			// 受け流す
			ExecuteRepel(attacker, hitPosition);

			// 弾き成功モーションを設定
			// SetAnimation(hRepelSuccessMotion_, false, false);
			
			// ダメージ無効
			return false;
		}


		Vector3 dirToAttacker = enemyPosition - GetWorldPosition();
		dirToAttacker.y = 0.0f; // 水平方向のみ

		if (dirToAttacker.Length() > 0.0f)
		{
			dirToAttacker = dirToAttacker.Normalize();
			movement_->SetTargetRotationY(std::atan2(dirToAttacker.x, dirToAttacker.z));
		}

		// 攻撃者の方向と逆方向にノックバックを加える
		movement_->AddKnockback(-dirToAttacker * 2.0f);

		// ガード成功の処理を行う
		guardState->HitGuard();

		// ガードse
		soundManager_->SeGuard();

		// ガードエフェクト
		effectManager_->CreateGuardEffect(hitPosition.value(), worldTransform_->rotate_);

		// ガードしたのがプレイヤーの場合は、スローモーションを開始する
		if (IsPlayer())
			GrowthEngine::GetInstance()->StartSlowMotion(0.0f, 0.1f);

		return false; // ガード成功によりダメージ無効
	}


	// ダメージを受ける処理

	// 攻撃者が攻撃をヒットさせたことを通知する
	if (attacker)
	{
		attacker->SetHitAttack(true);

		// 攻撃者のレイジゲージを増加させる
		attacker->ChargeRageGage(damageReaction);
	}

	// 飛ばされているときに、ダメージを受けたかどうか
	bool isBlownHit = IsBlownAway() || IsBlownFalling();

	// 弱攻撃が攻撃中のプレイヤーやボスに当たったかどうか
	bool isLightAttackHit = false;

	// ヒットエフェクト
	if (hitPosition)
	{
		// エフェクトを再生する
		effectManager_->ImpactDrop000(*hitPosition);
		effectManager_->ImpactSmoke000(*hitPosition);
		effectManager_->ImpactSmoke001(*hitPosition);
		if (attacker)effectManager_->Impact000(*hitPosition, attacker->GetWorldTransform()->rotate_);
		effectManager_->Impact001(*hitPosition);
		effectManager_->Impact002(*hitPosition);
		effectManager_->Impact003(*hitPosition);
		effectManager_->Impact004(*hitPosition);
		effectManager_->Impact005(*hitPosition);
	}


	float slowMotionTimeScale = 0.0f; // スローモーションの時間倍率
	float slowMotionDuration = 0.0f; // スローモーションの持続時間

	if (IsDownLying() || IsDownStagger())
	{
		// 軽い怯みのSEを再生する
		soundManager_->SeLightDamage();

		// 軽い怯みのスローモーションを設定する
		slowMotionTimeScale = 0.0f;
		slowMotionDuration = 0.1f;
	}
	else if (IsBlownAway() || IsBlownFalling())
	{
		// 落下速度をリセットする
		movement_->SetVelocityY(0.0f);

		// 重い怯みのSEを再生する
		soundManager_->SeHeavyDamage();

		// 軽い怯みのスローモーションを設定する
		slowMotionTimeScale = 0.0f;
		slowMotionDuration = 0.1f;

		// 落下中の状態に遷移する
		if (!IsBlownAway())
		{
			stateMachine_->ChangeState("BlownFalling");
			if (auto state = dynamic_cast<CharacterStateBlownFalling*>(stateMachine_->GetCurrentState()))
				state->DamageReaction();
		}
		else
		{
			// すでに落下中の状態の場合は、再度Enterを呼び出して、落下の時間をリセットする
			stateMachine_->GetCurrentState()->Enter();
			if (auto state = dynamic_cast<CharacterStateBlownFalling*>(stateMachine_->GetCurrentState()))
				state->DamageReaction();
		}
	}
	else
	{
		// リアクションごとの処理
		if (damageReaction == DamageReaction::LightStagger)
		{
			if (!isThrow)
			{
				// 軽い怯みのSEを再生する
				soundManager_->SeLightDamage();

				// 軽い怯みのスローモーションを設定する
				slowMotionTimeScale = 0.0f;
				slowMotionDuration = 0.1f;
			}

			if (!IsLightDamage())
			{
				// 軽い怯みの状態に遷移する
				stateMachine_->ChangeState("LightDamage");
				if (auto state = dynamic_cast<CharacterStateLightDamage*>(stateMachine_->GetCurrentState()))
					state->DamageReaction(hitPosition);
			}
			else
			{
				// すでに軽い怯みの状態の場合は、再度Enterを呼び出して、怯みの時間をリセットする
				stateMachine_->GetCurrentState()->Enter();
				if (auto state = dynamic_cast<CharacterStateLightDamage*>(stateMachine_->GetCurrentState()))
					state->DamageReaction(hitPosition);
			}
		}
		else if (damageReaction == DamageReaction::HeavyStagger)
		{
			if (!isThrow)
			{
				// 重い怯みのSEを再生する
				soundManager_->SeHeavyDamage();

				// 重い怯みのスローモーションを設定する
				slowMotionTimeScale = 0.0f;
				slowMotionTimeScale = 0.125f;
			}

			if (!IsHeavyDamage())
			{
				// 重い怯みの状態に遷移する
				stateMachine_->ChangeState("HeavyDamage");
				if (auto state = dynamic_cast<CharacterStateHeavyDamage*>(stateMachine_->GetCurrentState()))
					state->DamageReaction(hitPosition);
			}
			else
			{
				// すでに重い怯みの状態の場合は、再度Enterを呼び出して、怯みの時間をリセットする
				stateMachine_->GetCurrentState()->Enter();
				if (auto state = dynamic_cast<CharacterStateHeavyDamage*>(stateMachine_->GetCurrentState()))
					state->DamageReaction(hitPosition);
			}
		}
		else if (damageReaction == DamageReaction::Down)
		{
			if (!isThrow)
			{
				// 重い怯みのSEを再生する
				soundManager_->SeHeavyDamage();

				// ダウンのスローモーションを設定する
				slowMotionTimeScale = 0.0f;
				slowMotionDuration = 0.15f;
			}

			// ノックバックが上方向の場合は、吹き飛ばしの状態に遷移する
			if (knockDirection.y * knockback > 0.0f)
			{
				if (!IsBlownAway())
				{
					stateMachine_->ChangeState("BlownAway");
					if (auto state = dynamic_cast<CharacterStateBlownAway*>(stateMachine_->GetCurrentState()))
						state->DamageReaction(hitPosition);
				}
			}
			else
			{
				if (!IsDownFalling() || !IsDownLying())
				{
					// ダウンの状態に遷移する
					stateMachine_->ChangeState("DownFalling");
					if (auto state = dynamic_cast<CharacterStateDownFalling*>(stateMachine_->GetCurrentState()))
						state->DamageReaction(hitPosition);
				}
			}
		}
	}

	// プレイヤーが攻撃した場合、またはプレイヤーがダメージを受けた場合は、スローモーションを開始する
	if (attacker && attacker->IsPlayer() || IsPlayer())if (IsHitDamage())
		GrowthEngine::GetInstance()->StartSlowMotion(slowMotionTimeScale, slowMotionDuration);


	// ダメージを受けたことを通知する
	isHitDamage_ = true;

	// プレイヤーが攻撃中の場合は、軽い怯みを無効化する
	if (!((!IsPlayer() && !IsBoss()) || !IsAttack()))
	{
		// 弱攻撃が攻撃中のプレイヤーに当たった
		isLightAttackHit = true;

		// ノックバックを無効化する
		knockback = 0.0f;
	}

	// 弱攻撃を受けたプレイヤーとボス以外は、攻撃者をロックオンターゲットに設定する
	if (!isLightAttackHit)if (attacker)lockOnTarget_ = attacker;


	// 最終的な攻撃力を計算する
	int finalDamage = damage;
	finalDamage = static_cast<int32_t>(static_cast<float>(finalDamage) * attackPower_);

	if (attacker)
	{
		// レイジモード中の攻撃力を考慮して最終的なダメージを計算する
		finalDamage = static_cast<int>(static_cast<float>(finalDamage) * attacker->RageModeAttackPower());

		// 武器の攻撃力を考慮して最終的なダメージを計算する
		if (attacker->HasWeapon())
		{
			finalDamage = static_cast<int>(static_cast<float>(finalDamage) * attacker->GetWeapon()->GetAttackPower());

			// 武器の耐久力を1減らす
			attacker->GetWeapon()->TakeDamage(1);
		}
	}

	// 体力を減らし、0未満にならないようにする
	hp_ = std::max(0, hp_ - finalDamage);

	// ノックバック処理
	if (knockback > 0.0f)
	{
		if (!isBlownHit)
		{
			movement_->AddKnockback(knockDirection.Normalize() * knockback);
		}
	}

	// 死亡判定
	if (hp_ == 0)
	{
		Dead();

		// プレイヤーが相手を倒した場合は、スローモーションを開始する
		if (attacker && attacker->IsPlayer())
			GrowthEngine::GetInstance()->StartSlowMotion(0.2f, 0.55f);

		// プレイヤーが倒された場合は、スローモーションを開始する
		if(IsPlayer())
			GrowthEngine::GetInstance()->StartSlowMotion(0.3f, 3.0f);
	}

	return true; // ダメージが通った
}

/// @brief 受け流されたときの処理
/// @param pullPosition 
/// @param pushDirection 
/// @return 
void Character::OnDeflected(const Vector3& pullPosition, const Vector3& pushDirection, float knockBackPower)
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

	// 受け流され状態に遷移する
	stateMachine_->ChangeState("Deflected");

	// 受け流し成功のノックバックを設定する（相手を押し出す）
	movement_->AddKnockback(pushDirection * knockBackPower);
}

/// @brief 弾かれた時の処理
/// @param pushDirection 
void Character::OnRepelled(const Vector3& pushDirection, float knockBackPower)
{
	// すでに死亡している場合は、何もしない
	if (IsDead())return;

	// 攻撃や移動をキャンセルする
	MoveStop();

	// 回避とダッシュのフラグをリセットする
	isAvoid_ = false;
	isDash_ = false;
	bufferedAttackInput_ = AttackInputType::None;

	// 弾かれ状態に遷移する
	stateMachine_->ChangeState("Repelled");

	// ノックバックを入れる
	movement_->AddKnockback(pushDirection * knockBackPower);

	// シェイクさせる
	shake_->StartShake(0.05f, 0.15f);
}

/// @brief 掴みダメージを受けた時の処理
/// @param damage 
void Character::OnGrabDamage(int damage, DamageReaction damageReaction, Character* attacker, std::optional<Vector3> hitPosition)
{
	// すでに死亡している場合は、ダメージを受けない
	if (IsDead())return;

	// 回避とダッシュのフラグをリセットする
	bufferedAttackInput_ = AttackInputType::None;


	// ダメージを受ける処理

	// 攻撃者が攻撃をヒットさせたことを通知する
	if (attacker)
	{
		attacker->SetHitAttack(true);

		// 攻撃者のレイジゲージを増加させる
		attacker->ChargeRageGage(damageReaction);
	}

	// ヒットエフェクト
	if (hitPosition)
	{
		// エフェクトを再生する
		effectManager_->ImpactDrop000(*hitPosition);
		effectManager_->ImpactSmoke000(*hitPosition);
		effectManager_->ImpactSmoke001(*hitPosition);
		if (attacker)effectManager_->Impact000(*hitPosition, attacker->GetWorldTransform()->rotate_);
		effectManager_->Impact001(*hitPosition);
		effectManager_->Impact002(*hitPosition);
		effectManager_->Impact003(*hitPosition);
		effectManager_->Impact004(*hitPosition);
		effectManager_->Impact005(*hitPosition);
	}

	float slowMotionTimeScale = 0.0f; // スローモーションの時間倍率
	float slowMotionDuration = 0.0f; // スローモーションの持続時間

	// リアクションごとの処理
	if (damageReaction == DamageReaction::LightStagger)
	{
		// 軽い怯みのSEを再生する
		soundManager_->SeLightDamage();

		// 軽い怯みのスローモーションを設定する
		slowMotionTimeScale = 0.0f;
		slowMotionDuration = 0.1f;
	}
	else if (damageReaction == DamageReaction::HeavyStagger)
	{
		// 重い怯みのSEを再生する
		soundManager_->SeHeavyDamage();

		// 重い怯みのスローモーションを設定する
		slowMotionTimeScale = 0.0f;
		slowMotionTimeScale = 0.125f;
	}
	else if (damageReaction == DamageReaction::Down)
	{
		// 重い怯みのSEを再生する
		soundManager_->SeHeavyDamage();

		// ダウンのスローモーションを設定する
		slowMotionTimeScale = 0.0f;
		slowMotionDuration = 0.15f;
	}

	// プレイヤーが攻撃した場合、またはプレイヤーがダメージを受けた場合は、スローモーションを開始する
	if (attacker && attacker->IsPlayer() || IsPlayer())if (IsHitDamage())
		GrowthEngine::GetInstance()->StartSlowMotion(slowMotionTimeScale, slowMotionDuration);


	// 最終的な攻撃力を計算する
	int finalDamage = damage;
	finalDamage = static_cast<int32_t>(static_cast<float>(finalDamage) * attackPower_);

	if (attacker)
	{
		// レイジモード中の攻撃力を考慮して最終的なダメージを計算する
		finalDamage = static_cast<int>(static_cast<float>(finalDamage) * attacker->RageModeAttackPower());

		// 武器の攻撃力を考慮して最終的なダメージを計算する
		if (attacker->HasWeapon())
		{
			finalDamage = static_cast<int>(static_cast<float>(finalDamage) * attacker->GetWeapon()->GetAttackPower());

			// 武器の耐久力を1減らす
			attacker->GetWeapon()->TakeDamage(1);
		}
	}

	// 体力を減らし、0未満にならないようにする
	hp_ = std::max(0, hp_ - finalDamage);

	// 死亡判定
	if (hp_ == 0)
	{
		Dead();

		// プレイヤーが相手を倒した場合は、スローモーションを開始する
		if (attacker && attacker->IsPlayer())
			GrowthEngine::GetInstance()->StartSlowMotion(0.2f, 0.55f);

		// プレイヤーが倒された場合は、スローモーションを開始する
		if (IsPlayer())
			GrowthEngine::GetInstance()->StartSlowMotion(0.3f, 3.0f);
	}
}

/// @brief レイジゲージの更新
void Character::RageGageUpdate(float dt)
{
	// レイジゲージの閾値が空の場合は、インデックスを0にリセットする
	if (rageGageThresholds_.empty())
	{
		rageGageThresholdIndex_ = 0;
		return;
	}

	// レイジモード中は、レイジゲージを減少させる
	if (isRageMode_)
	{
		if (IsRageModeStart())
		{
			// レイジモード開始時の待機時間を減少させる
			rageModeStartTimer_ -= dt;
			rageModeStartTimer_ = std::max(0.0f, rageModeStartTimer_);
		}
		else
		{
			rageGage_ -= kRageGageDecrease * dt;
			rageGage_ = std::max(0.0f, rageGage_);

			// レイジゲージが0になったら、レイジモードを終了する
			if (rageGage_ <= 0.0f)
			{
				isRageMode_ = false;
				rageGageThresholdIndex_ = 0;

				// レイジモード終了のSEを再生する
				soundManager_->SeRageModeEnd();
			}
		}
	}

	// 閾値を超えた数をインデックスとする
	while (rageGageThresholdIndex_ < rageGageThresholds_.size() && rageGage_ >= rageGageThresholds_[rageGageThresholdIndex_])
	{
		rageGageThresholdIndex_++;
	}
}

/// @brief レイジゲージをチャージする
/// @param damageReaction 
void Character::ChargeRageGage(DamageReaction damageReaction)
{
	// レイジモード中はゲージをチャージしない
	if (isRageMode_)return; 

	if (damageReaction == DamageReaction::LightStagger) rageGage_ += 0.5f;
	else if (damageReaction == DamageReaction::HeavyStagger) rageGage_ += 1.0f;
	else if (damageReaction == DamageReaction::Down) rageGage_ += 1.5f;
}

/// @brief レイジモード中の入力処理
void Character::RageModeInput()
{
	// レイジモード中は、レイジゲージの閾値を超えた場合にのみ、レイジモードの入力を受け付ける
	if (IsIncapacitated())return;

	if (isRageMode_)
	{
		// レイジモードを終了させる
		isRageMode_ = false;

		// レイジモード終了のSEを再生する
		soundManager_->SeRageModeEnd();
	}
	else if(IsRageGageThresholdExceeded())
	{
		// ゲージが閾値を超えた場合は、レイジモードを開始させる

		// レイジモードを開始させる
		isRageMode_ = true;
		isSuccessRageModeStart_ = true;

		GrowthEngine::GetInstance()->StartSlowMotion(0.2f, 0.5f);

		// レイジモード開始時の待機時間を設定する
		rageModeStartTimer_ = kRageModeStartDuration;

		// レイジモード開始のSEを再生する
		soundManager_->SeRageModeStart();
	}
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

	Vector3 direction = movement_->GetDirection();

	// 移動入力がない場合は現在向いている方向の後ろへ回避する
	Vector2 backwardDirection = Vector2(-direction.x, -direction.z);
	if (backwardDirection.Length() <= 0.0f)
	{
		backwardDirection = Vector2(0.0f, -1.0f);
	}

	return backwardDirection.Normalize();
}

// 構え中のロックオン候補を更新する
void Character::UpdateLockOnTargets()
{
	// 構え中でない場合、構えなしでロックオンできない設定の場合、ダウン中の場合、すでにロックオンしている相手が死んでいる場合は、ロックオンを解除する
	if (IsDown() || (lockOnTarget_ && lockOnTarget_->IsDead()))
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
			Vector3 direction = movement_->GetDirection();

			// 目の前にいる相手のみリストに登録する
			const Vector3 toTargetDirection = toTarget.Normalize();
			if (Dot(direction, toTargetDirection) <= 0.0f)
			{
				continue;
			}

			// 距離と相手ポインタを登録する
			const float distance = std::sqrt(distanceSq);

			// 視線方向との内積を計算する
			const float viewDot = Dot(direction, toTargetDirection);

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
	// 死亡したら、モーション設定は行わない
	if (isDead_)return;

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

	// レイジモード中は、攻撃速度を上げる
	float rageModeAttackSpeed = RageModeAttackSpeed();

	float dt = engine_->GetDeltaTime() * engine_->GetTimeScale() * rageModeAttackSpeed;

	// プレイヤーのスタイルチェンジモーションはtimeScaleの影響を受けないようにする
	if (isStyleChanging_ && IsPlayer())
		dt = engine_->GetDeltaTime();

	// 死亡していない場合は、通常のモーションを再生する
	if (!IsDead())
	{
		// スタイルチェンジ中でない場合は、通常のモーションを再生する
		if (!IsStyleChanging())
		{
			if (!currentAttack_ && !IsDamageReaction() && !IsGrabbed() && !IsGuard())
			{
				// 立ちモーションを再生する
				SetAnimation(hStandMotion_, false, true);

				//　移動している場合は歩きモーションを再生する
				if (movement_->GetTargetVelocity().Length() > 0.0f)
					SetAnimation(hWalkMotion_, false, true);

				// ダッシュしている場合はダッシュモーションを再生する
				if (isDash_)
					SetAnimation(hDashMotion_, false, true);

				// 構え中は構えモーションを優先して再生する
				if (isStance_)
					SetAnimation(hStanceMotion_, false, true);

				// 回避中は回避モーションを優先して再生する
				if (isAvoid_)
				{
					// 回避方向
					Vector3 avoidDirection = (avoidEndPosition_ - avoidStartPosition_).Normalize();

					if (avoidDirection.Length() > 0.0f)
					{
						// キャラクターの向き（前）と右方向
						Vector3 forward = movement_->GetDirection();
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
				else if (IsGrabbing())
				{
					// つかみモーションを再生する
					SetAnimation(hGrabMotion_, false, true);
				}
			}
			else
			{
				// 掴み攻撃が終了した場合は、掴みモーションに戻す
				if (currentAttack_->IsFinishedTimer())
				{
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

/// @brief 相手をつかむ
/// @param target 
void Character::ExecuteGrab(Character* target, float duration,const std::optional<Vector3>& hitPosition)
{
	// 掴み状態に遷移する
	stateMachine_->ChangeState("Grabbing");
	if (auto state = dynamic_cast<CharacterStateGrabbing*>(stateMachine_->GetCurrentState()))
	{
		state->SetGrabTarget(target);
	}

	// 掴まれた相手の処理を呼び出す
	target->OnGrabbed(this);

	//// 掴みエフェクトを発生させる
	//if (hitPosition)
	//{
	//	effectManager_->GrabImpact000(*hitPosition);
	//}
	
	// 掴みSEを再生する
	soundManager_->SeGrab();
}

/// @brief 掴まれた相手の処理
void Character::OnGrabbed(Character* grabber)
{
	// 掴まれた状態に遷移する
	stateMachine_->ChangeState("Grabbed");

	// 掴まれた状態のキャラクターに掴んだキャラクターを設定する
	auto currentState = stateMachine_->GetCurrentState();
	if (auto grabbedState = dynamic_cast<CharacterStateGrabbed*>(currentState))
	{
		grabbedState->SetGrabber(grabber);
	}

	// 掴まれた状態になったときの処理をここに書く
	isDash_ = false;
	isAvoid_ = false;
}

/// @brief 防御を実行する
void Character::ExecuteGuard()
{
	// 既に防御中の場合は処理しない
	if (IsGuard())return;

	// 防御状態に遷移する
	stateMachine_->ChangeState("Guard");
}

/// @brief 掴んでいる相手を取得する
/// @return 
Character* Character::GetGrabTarget() const
{
	// 掴んでいる状態でない場合はnullptrを返す
	if (stateMachine_->GetCurrentStateName() != "Grabbing")
		return nullptr;

	// 掴んでいる状態のキャラクターのgrabTarget_を取得する
	auto state = static_cast<CharacterStateGrabbing*>(stateMachine_->GetCurrentState());
	return state->GetGrabTarget();
}

/// @brief 掴まれている相手を取得する
/// @param target 
void Character::SetGrabTarget(Character* target)
{
	// 掴んでいる状態でない場合は処理しない
	if (stateMachine_->GetCurrentStateName() != "Grabbing")
		return;

	// 掴んでいる状態のキャラクターのgrabTarget_を設定する
	auto state = static_cast<CharacterStateGrabbing*>(stateMachine_->GetCurrentState());
	state->SetGrabTarget(target);
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
	if (!IsGrabbed())return false;

	auto currentState = stateMachine_->GetCurrentState();
	if (auto grabbedState = dynamic_cast<CharacterStateGrabbed*>(currentState))
	{
		// 自分を掴む相手を取得する
		if (auto grabber = grabbedState->GetGrabber())
		{
			// 掴まれた状態で攻撃されているかどうかは、掴んでいる相手の攻撃が掴み攻撃かどうかで判断する
			return grabber->IsGrabStrikeAttack();
		}
	}

	return false;
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
void Character::ExecuteDeflect(Character* attacker)
{
	if (!attacker) return;

	// 自分の位置と向きを取得する
	Vector3 myPos = GetWorldPosition();
	Vector3 myForward = GetDirection();

	// 引き込む位置
	Vector3 pullPos = myPos - myForward * 0.2f;

	// 受け流され処理を実行
	attacker->OnDeflected(pullPos, attacker->GetDirection(), 4.0f);

	// se受け流し
	soundManager_->SeDeflect();
}

/// @brief 弾きを実行する
/// @param attacker 
/// @param hitPosition 
void Character::ExecuteRepel(Character* attacker, std::optional<Vector3> hitPosition)
{
	if (!attacker) return;

	// 相手の攻撃を弾いたことによる反動（ノックバック方向）を計算
	Vector3 pushDir = attacker->GetWorldPosition() - GetWorldPosition();
	pushDir.y = 0.0f;

	// ノックバック方向を正規化する
	pushDir = pushDir.Normalize();

	// 相手に弾きのリアクションを与える
	attacker->OnRepelled(pushDir, 5.0f);
	isHitRepel_ = true;

	// 弾きのエフェクトを発生させる
	if (hitPosition)
	{
		effectManager_->CreateRepelEffect(*hitPosition);
		effectManager_->EmitSpark000(*hitPosition);
		effectManager_->Impact001(*hitPosition);
		effectManager_->Impact003(*hitPosition);
		effectManager_->Impact005(*hitPosition);
	}

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
	soundManager_->SeRepel();
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
		canDeflect_ = true; // 旋嵐スタイルは受け流しが可能になる
		canRepel_ = false; // 旋嵐スタイルは弾きが不可能になる
		break;

		// 撃鉄
	case FightStyle::Hammer:
		canDeflect_ = false; // 撃鉄スタイルは受け流しが不可能になる
		canRepel_ = true; // 撃鉄スタイルは弾きが可能になる
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

/// @brief 着地判定の更新
void Character::LandingCheck()
{
	// 着地しているかどうかのフラグをリセットする
	movement_->SetGrounded(false);

	// コリジョンがないと処理しない
	if (!landingCollision_)return;

	// Y方向の速度が上向きで、かつ吹き飛ばされている状態の場合は着地判定を行わない
	if (movement_->GetKnockbackVelocity().y + movement_->GetVelocityY() > 0.0f && IsBlownDown())
		return;

	// コリジョンの状態を確認する
	if (landingCollision_->isCollision_)
	{
		// コリジョンの当たり判定がAABBであることを前提に、床との接触位置を計算する
		auto floorCollision = static_cast<Collision3DInstanceAABB*>(landingCollision_->hitOpponent_);
		worldTransform_->translate_.y = floorCollision->param_->center.y + floorCollision->param_->radius.y;

		// 着地していると判定する
		movement_->SetGrounded(true);

		// Y方向の速度をリセットする（着地したので落下を止める）
		movement_->SetVelocityY(0.0f);
	}
}

/// @brief 壁接触の更新
void Character::WallTouchCheck()
{
	// コリジョンがないと処理しない
	if (!wallTouchCollision_)return;

	// 壁に接触しているかどうかのフラグを更新する
	isWallTouch_ = wallTouchCollision_->isCollision_;
}

/// @brief 壁接触の更新
void Character::WallTouchUpdate()
{
	// コリジョンがないと処理しない
	if (!wallTouchCollision_)return;

	// 壁に接触していない場合は処理しない
	if (!isWallTouch_) return;

	// コリジョンの当たり判定がOBBであることを前提に、押し出しベクトルを計算する
	if (wallTouchCollision_->hitOpponent_->GetType() == Engine::Collision3D::Type::OBB)
	{
		// カプセルの情報
		auto capsule = wallTouchCollision_->param_.get();

		// 当たった相手のOBBの情報
		auto hitColliders = static_cast<Collision3DInstanceOBB*>(wallTouchCollision_->hitOpponent_);
		auto obbParam = hitColliders->param_.get();


		// キャラクターの位置（カプセルの始点）を取得する
		Vector3 pos = capsule->start;

		// キャラクターの位置からOBBの中心へのベクトルを計算する
		Vector3 offset = pos - obbParam->center;

		// ワールド座標系のベクトルをOBBのローカル座標系に変換する
		Vector3 localPos;
		localPos.x = offset.x * obbParam->oriented[0].x + offset.y * obbParam->oriented[0].y + offset.z * obbParam->oriented[0].z;
		localPos.y = offset.x * obbParam->oriented[1].x + offset.y * obbParam->oriented[1].y + offset.z * obbParam->oriented[1].z;
		localPos.z = offset.x * obbParam->oriented[2].x + offset.y * obbParam->oriented[2].y + offset.z * obbParam->oriented[2].z;

		// OBBのローカル座標系で、キャラクターの位置をOBBの中心から見たときのベクトルを、OBBの半径内にクランプする
		Vector3 closestLocal;
		closestLocal.x = std::clamp(localPos.x, -obbParam->radius.x, obbParam->radius.x);
		closestLocal.y = std::clamp(localPos.y, -obbParam->radius.y, obbParam->radius.y);
		closestLocal.z = std::clamp(localPos.z, -obbParam->radius.z, obbParam->radius.z);

		// クランプされたローカル座標をワールド座標に変換する
		Vector3 closestWorld = obbParam->center;
		closestWorld.x += obbParam->oriented[0].x * closestLocal.x + obbParam->oriented[1].x * closestLocal.y + obbParam->oriented[2].x * closestLocal.z;
		closestWorld.y += obbParam->oriented[0].y * closestLocal.x + obbParam->oriented[1].y * closestLocal.y + obbParam->oriented[2].y * closestLocal.z;
		closestWorld.z += obbParam->oriented[0].z * closestLocal.x + obbParam->oriented[1].z * closestLocal.y + obbParam->oriented[2].z * closestLocal.z;

		// キャラクターの位置とクランプされた点との距離を計算する
		Vector3 diff = pos - closestWorld;
		float distSq = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;

		// めり込んでいる場合（距離がカプセルの半径未満）
		if (distSq > 0.0f && distSq < (capsule->radius * capsule->radius))
		{
			float dist = std::sqrt(distSq);
			float penetration = capsule->radius - dist; // めり込み量

			// 押し出しベクトル（正規化方向ベクトル × めり込み量）
			Vector3 pushVector = (diff / dist) * penetration;

			// ここでもY軸の押し出しは無効化する
			pushVector.y = 0.0f;

			// 位置を補正する
			Vector3 currentPos = GetPosition();
			currentPos.x += pushVector.x;
			currentPos.z += pushVector.z;
			SetPosition(currentPos);

			// カプセルの始点も補正に合わせて更新しておく
			capsule->start.x = currentPos.x;
			capsule->start.z = currentPos.z;
		}
	}
}

/// @brief 押し出し判定処理
void Character::UpdatePushOut()
{
	// モデルがない場合は処理しない
	if (!model_)return;

	// キャラクターの押し出し半径
	constexpr float kPushRadius = 0.25f; // 押し出し半径
	constexpr float kDistanceLimit = kPushRadius * 2.0f; // 2人の半径の和

	// 押し直しのなめらかさ（抵抗感）の係数
	constexpr float kPushStrength = 0.2f;

	// 全キャラクターのリストを取得
	const auto& characters = Character::GetCharacters();

	for (auto* other : characters)
	{
		// 自分自身とは判定しない
		if (this == other) continue;

		// 自分よりも後に生成されたキャラクターは押し出し判定を行わない（重複処理防止）
		if (this > other) continue;

		// モデルを持たないキャラクターは押し出し判定を行わない
		if (!other->HasModel())continue;

		// 地面に倒れている、掴まれている、掴んでいる、受け流し中のキャラクターは押し出し判定を行わない
		if (IsGroundedDown() || other->IsGroundedDown() ||IsGrabbed() || other->IsGrabbed() ||
			IsGrabbing() || other->IsGrabbing() ||IsDeflected() || other->IsDeflected())
			continue;

		// 自分と相手の位置を取得する
		Vector3 myPos = GetPosition();
		Vector3 myHeadPos = GetBonePosition(JointType::Head);
		Vector3 otherPos = other->GetPosition();
		Vector3 otherHeadPos = other->GetBonePosition(JointType::Head);

		// 相手の体幹上の最近点を計算する
		Vector3 otherProjPos = Project(otherHeadPos - otherPos, myPos - otherPos) + otherPos;

		// Y軸方向の重なりがあるか確認（最初のコードの判定）
		if (myPos.y <= otherProjPos.y + std::fabs(otherHeadPos.y - otherPos.y) &&
			myPos.y + std::fabs(myHeadPos.y - myPos.y) >= otherProjPos.y)
		{
			// 射影点と自分の位置の差を計算する
			Vector3 diff = myPos - otherProjPos;
			diff.y = 0.0f; // 押し出し自体は水平（XZ平面）に行う

			float distSq = diff.LengthSq();

			// 完全に重なってしまった場合のスタック（引っかかり）防止
			if (distSq < 0.00001f)
			{
				diff = Vector3(0.01f, 0.0f, 0.0f);
				distSq = diff.LengthSq();
			}

			// 距離が押し出し半径の2倍以内の場合は押し出す
			if (distSq < (kDistanceLimit * kDistanceLimit))
			{
				float dist = std::sqrt(distSq);

				// めり込んでいる距離を計算
				float penetration = kDistanceLimit - dist;

				// 押し出し方向の単位ベクトル
				Vector3 pushDir = diff / dist;

				// 前回の「滑らかに押し合う（壁にならない）」移動量を計算
				float pushAmount = penetration * kPushStrength;

				// 自分（0.3）と相手（0.7）の移動比率（プレイヤーが押し込みやすくする設定）
				float myWeight = 0.3f;
				float otherWeight = 0.7f;

				// 自分の位置を更新（相手の体から離れる方向へ）
				Vector3 newMyPos = myPos + pushDir * (pushAmount * myWeight);
				SetPosition(newMyPos);

				// 相手の位置も同時に更新（自分から遠ざかる方向へズルズルと押される）
				Vector3 newOtherPos = otherPos - pushDir * (pushAmount * otherWeight);
				other->SetPosition(newOtherPos);
			}
		}
	}
}

/// @brief トレイルの位置を設定する
/// @param basePosition
/// @param tipPosition 
void Character::SetTrailPos(const Vector3& basePosition, const Vector3& tipPosition)
{
	if (attackTrail_)
	{
		// トレイルの位置を更新する
		attackTrail_->param_->basePosition = basePosition;
		attackTrail_->param_->tipPosition = tipPosition;
	}
}

/// @brief 死亡処理
void Character::Dead()
{
	// 移動を止める
	MoveStop();

	// 死亡タイマーをリセットする
	deadTimer_ = kDeadDuration;

	// 死亡モーションを再生する
	SetAnimation(motionManager_->GetMotion(MotionType::DownFall, "Front"), true, false);

	// 死亡フラグを立てる
	isDead_ = true;

	// 当たり判定の削除
	if (eventTriggerCollision_) eventTriggerCollision_->Delete();
	eventTriggerCollision_ = nullptr;

	if (hurtboxHead_.collider_) hurtboxHead_.collider_->Delete();
	hurtboxHead_.collider_ = nullptr;

	if (hurtboxChest_.collider_) hurtboxChest_.collider_->Delete();
	hurtboxChest_.collider_ = nullptr;

	if (hurtboxRoot_.collider_) hurtboxRoot_.collider_->Delete();
	hurtboxRoot_.collider_ = nullptr;

	// HUDに死亡を通知する
	if (hpHUD_)hpHUD_->Death();
	hpHUD_ = nullptr;

	// インスタンスリストから自分を除外する
	auto it = std::find(characters_.begin(), characters_.end(), this);
	if (it != characters_.end())
	{
		characters_.erase(it);
	}
}

/// @brief 初期化用データを設定する
/// @param initData 
void Character::SetInitData(const CharacterInitData& initData)
{
	// フラグをリセットする
	isFinished_ = false;
	isDead_ = false;
	isInAttackSequence_ = false;
	isDash_ = false;
	isAvoid_ = false;
	isJustAvoided_ = false;
	isJustAvoidedPrev_ = false;
	isStance_ = false;
	canLockOnWithoutStance_ = false;
	isGuardHit_ = false;
	isPrevGuardHit_ = false;
	canDeflect_ = false;
	canRepel_ = false;
	isHitRepel_ = false;
	isPrevHitRepel_ = false;
	isStyleChanging_ = false;
	isAnimationLoop_ = true;
	isHitAttack_ = false;
	isPrevHitAttack_ = false;
	isHitDamage_ = false;
	isPrevHitDamage_ = false;
	isWallTouch_ = false;

	// ロックオンターゲットをリセットする
	lockOnTarget_ = nullptr;

	// レイジゲージをリセットする
	rageGage_ = 0;
	rageGageThresholdIndex_ = 0;


	// 位置
	worldTransform_->translate_ = initData.position;

	// 回転
	worldTransform_->rotate_ = Vector3(0.0f, initData.rotateY, 0.0f);

	// 体力
	hp_ = initData.hp;

	// レイジゲージの閾値 昇順
	rageGageThresholds_ = initData.rageGageThresholds;
	std::sort(rageGageThresholds_.begin(), rageGageThresholds_.end());

	// モデルデータ
	if (initData.model_)
	{
		// モデル
		model_ = initData.model_;

		// アニメーションの時間を取得する
		animationTime_ = engine_->GetAnimationTime(model_->param_->animation.hAnimation);

		// モデルをワールドトランスフォームの子にする
		model_->SetParent(worldTransform_.get());
	}

	// 武器
	GrabWeapon(initData.weapon);

	// 攻撃用トレイル
	if (initData.attackTrail)
	{
		attackTrail_ = initData.attackTrail;
		attackTrail_->param_->easing_ = 0.5f * 0.5f * 0.5f;
	}

	// モーション
	hStandMotion_ = initData.hStandMotion;
	hStanceMotion_ = initData.hStanceMotion;
	hWalkMotion_ = initData.hWalkMotion;
	hDashMotion_ = initData.hDashMotion;
	hAvoidFrontMotion_ = initData.hAvoidFrontMotion;
	hAvoidBackMotion_ = initData.hAvoidBackMotion;
	hAvoidLeftMotion_ = initData.hAvoidLeftMotion;
	hAvoidRightMotion_ = initData.hAvoidRightMotion;

	hGrabMotion_ = motionManager_->GetMotion(MotionType::Grab, "Front");
	hGrabbedMotion_ = motionManager_->GetMotion(MotionType::Grabbed, "Front");



	// 当たり判定グループ
	if (!hurtboxHead_.collider_ && !hurtboxChest_.collider_ && !hurtboxRoot_.collider_)
	{
		hurtboxHead_.collider_ = initData.hurtboxGroup->CreateInstance();
		hurtboxHead_.owner_ = this;
		hurtboxHead_.type_ = ColliderType::Hurtbox;

		hurtboxChest_.collider_ = initData.hurtboxGroup->CreateInstance();
		hurtboxChest_.owner_ = this;
		hurtboxChest_.type_ = ColliderType::Hurtbox;

		hurtboxRoot_.collider_ = initData.hurtboxGroup->CreateInstance();
		hurtboxRoot_.owner_ = this;
		hurtboxRoot_.type_ = ColliderType::Hurtbox;
	}

	// 攻撃判定グループ
	hitboxGroup_ = initData.hitboxGroup;


	// ワールド座標を更新
	worldTransform_->Update();

	// 着地判定
	if (initData.landingCollision)
	{
		landingCollision_ = initData.landingCollision;
		landingCollision_->param_->start = GetWorldPosition();
		landingCollision_->param_->diff = Vector3(0.0f, 0.0f, 0.0f);
		landingCollision_->param_->radius = 0.25f;
	}

	// 壁接触の当たり判定
	if (initData.wallTouchCollision)
	{
		wallTouchCollision_ = initData.wallTouchCollision;
		wallTouchCollision_->param_->start = GetWorldPosition();
		wallTouchCollision_->param_->diff = Vector3(0.0f, 0.0f, 0.0f);
		wallTouchCollision_->param_->radius = 0.25f;
	}

	// イベントトリガーの当たり判定
	if (initData.eventTriggerCollision)
	{
		eventTriggerCollision_ = initData.eventTriggerCollision;
		eventTriggerCollision_->param_->start = GetWorldPosition();
		eventTriggerCollision_->param_->diff = Vector3(0.0f, 0.0f, 0.0f);
		eventTriggerCollision_->param_->radius = 0.25f;
	}

	// 体力HUD
	if (initData.hpHUD)
	{
		hpHUD_ = initData.hpHUD;
		hpHUD_->SetMaxHP(hp_);
		hpHUD_->SetCurrentHP(hp_);

		// 体力HUDの位置を更新する
		HpHudUpdate();
	}
}

/// @brief 当たり判定の更新
/// @param hurtbox 
/// @param jointType 
void Character::UpdateHurtbox(AppCollider& hurtbox, JointType jointType)
{
	// 当たり判定がない場合は処理しない
	if (!hurtbox.collider_)return;

	auto collider = static_cast<Collision3DInstanceSphere*>(hurtbox.collider_);
	collider->param_->radius = IsBlownAway() || IsBlownFalling() ? 0.5f : 0.25f;
	collider->param_->center = GetBonePosition(jointType);
}

/// @brief 当たり判定の位置を更新する
/// @param collision 
void Character::UpdateCollisionPosition(Collision3DInstanceCapsule* collision)
{
	// コリジョンがない場合は処理しない
	if (!collision)return;

	collision->param_->diff = GetWorldPosition() - collision->param_->start;
	collision->param_->start = GetWorldPosition();
}

/// @brief カメラのローカル方向をワールド座標系の移動方向に変換する
/// @param cameraLocalDirection 
/// @param cameraYaw 
/// @return 
Vector2 Character::ToWorldMoveDirectionFromCamera(const Vector2& cameraLocalDirection, float cameraYaw)
{
	// カメラ前方向(XZ平面)
	const Vector2 forward = Vector2(std::sin(cameraYaw), std::cos(cameraYaw));

	// カメラ右方向(XZ平面)
	const Vector2 right = Vector2(forward.y, -forward.x);

	// カメラ基準入力をワールド方向へ変換する
	return right * cameraLocalDirection.x + forward * cameraLocalDirection.y;
}

/// @brief 体力HUDの位置を更新する
void Character::HpHudUpdate()
{
	// 体力HUDがない場合は処理しない
	if (!hpHUD_)return;

	switch (characterTag_)
	{
		// 味方と敵は頭の上に体力HUDを表示する
	case CharacterTag::Ally:
	case CharacterTag::EnemyNormal:
	default:
		hpHUD_->SetPosition(GetBonePosition(JointType::Head) + Vector3(0.0f, 0.6f, 0.0f));
		break;

		// プレイヤーは画面左上の固定位置に体力HUDを表示する
	case CharacterTag::Player:
		hpHUD_->SetPosition(Vector2(360.0f, 670.0f));
		hpHUD_->SetVisible(true);
		break;

		// ボスは頭の上に体力HUDを表示する
	case CharacterTag::EnemyBoss:
		hpHUD_->SetPosition(GetBonePosition(JointType::Head) + Vector3(0.0f, 0.6f, 0.0f));
		break;
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