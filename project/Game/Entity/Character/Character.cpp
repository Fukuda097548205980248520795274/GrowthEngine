#include "Character.h"
#include <algorithm>
#include <cmath>
#include <numbers>

#include "Action/Attack/Attack.h"
#include "Action/Move/Move.h"
#include "Action/Avoid/Avoid.h"

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

// Characterインスタンスの共有リスト
std::vector<Character*> Character::characters_{};

/// @brief 
/// @param position 
Character::Character(const InitData& initData) : Entity()
{
	// インスタンスリストに登録する
	characters_.push_back(this);

	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

	// モーションマネージャのインスタンスを取得する
	motionManager_ = MotionManager::GetInstance();

	// タグを指定する
	entityTag_ = EntityTag::Character;


	// 位置
	worldTransform_->translate_ = initData.position;

	// 体力
	hp_ = initData.hp;

	// 回避時間と回避距離
	avoidDuration_ = initData.avoidDuration;
	avoidDistance_ = initData.avoidDistance;

	// モデルデータ
	if (initData.model_)
	{
		// モデル
		model_ = initData.model_;

		// アニメーションの時間を取得する
		animationTime_ = engine_->GetAnimationTime(model_->param_->animation.hAnimation);
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
	hGuardMotion_ = initData.hGuardMotion;
	hGuardHitMotion_ = initData.hGuardHitMotion;

	hDamageLightMotion_ = motionManager_->GetMotion(MotionType::Stagger, 0);
	hDamageHeavyMotion_ = motionManager_->GetMotion(MotionType::Stagger, 0);

	hDownFallMotion_ = motionManager_->GetMotion(MotionType::DownFall, 0);
	hDownLyingMotion_ = motionManager_->GetMotion(MotionType::DownLying, 0);
	hDownGetUpMotion_ = motionManager_->GetMotion(MotionType::DowoGetUp, 0);

	hGrabMotion_ = motionManager_->GetMotion(MotionType::Grab, 0);
	hGrabbedMotion_ = motionManager_->GetMotion(MotionType::Grabbed, 0);

	// 当たり判定グループ
	hurtbox_.collider_ = initData.hurtboxGroup->CreateInstance();
	hurtbox_.owner_ = this;
	hurtbox_.type_ = ColliderType::Hurtbox;

	// 攻撃判定グループ
	hitboxGroup_ = initData.hitboxGroup;
	

	// ブラックボードの生成
	blackboard_ = std::make_unique<Blackboard>();
}

/// @brief デストラクタ
Character::~Character()
{
	// インスタンスリストから自分を除外する
	auto it = std::remove(characters_.begin(), characters_.end(), this);
	characters_.erase(it, characters_.end());
}

/// @brief 更新処理
void Character::Update()
{
	auto collider = static_cast<Collision3DInstanceAABB*>(hurtbox_.collider_);
	collider->param_->center = worldTransform_->translate_ + Vector3(0.0f, 1.0f, 0.0f);

	// デルタタイムの取得
	const float dt = std::max(engine_->GetDeltaTime(), 0.0f);

	// つかまれている場合は、限界時間に達するまで自力で振りほどく処理を行う
	if (IsGrabbed())
	{
		// 限界時間に達したら、自力で振りほどく
		if (grabbedTimer_ >= escapeTimeLimit_)
		{
			// つかみ状態を解除する前に、相手に軽い怯みを与える（振りほどいたことへのリアクション）
			grabber_->OnDamage(0, DamageReaction::LightStagger, 0.1f, Vector3(0.0f, 0.0f, -1.0f), GetWorldPosition());

			worldTransform_->rotate_ = Vector3(0.0f, grabber_->GetWorldTransform()->rotate_.y + std::numbers::pi_v<float>, 0.0f);

			// 相手のつかみ状態を解除する
			grabber_->grabbedTarget_ = nullptr;

			// 自分のポインタを解除し、タイマーをリセット
			grabber_ = nullptr;
			grabbedTimer_ = 0.0f;
		}

		// 押し出し判定の更新
		UpdatePushOut();

		// アニメーションの更新
		UpdateAnimation();

		// 基底クラスの更新
		Entity::Update();

		return;
	}

	// つかまれている場合は、つかみ時間を減らしながら、相手の位置を自分の手の位置に同期させる
	if (IsGrabbing())
	{
		// 相手の座標を強制的に自分の手の位置（または目の前）に同期する
		Matrix4x4 handMatrix = GetBoneMatrix("RightHand");
		Vector3 handPos = Vector3(handMatrix.m[3][0], handMatrix.m[3][1], handMatrix.m[3][2]);
		const Vector3 grabOffset = Vector3(0.0f, -1.2f, 0.1f); // 手の前に少しオフセット
		grabbedTarget_->SetPosition(handPos + grabOffset);

		// 自分の回転を取得する
		Quaternion myRotation = GetRotation();

		// つかまれている相手は自分の回転に対して、Y軸に180度回転した向きになるようにする
		Quaternion offsetRot = ToQuaternion(std::numbers::pi_v<float>, Vector3(0.0f, 1.0f, 0.0f));
		Quaternion targetRotation = myRotation * offsetRot;

		grabbedTarget_->SetRotation(targetRotation);
	}

	// ノックバックの更新
	if (knockbackVelocity_.Length() > 0.01f)
	{
		// ノックバックの速度に基づいて位置を更新する
		Vector3 position = GetPosition();
		position += knockbackVelocity_ * dt;
		SetPosition(position);

		// ノックバックの速度を減衰させる
		knockbackVelocity_ = knockbackVelocity_ * std::pow(0.1f, dt);
	}
	else
	{
		// 十分に小さくなったらノックバックを止める
		knockbackVelocity_ = Vector3(0.0f, 0.0f, 0.0f);
	}

	// 怯み状態の更新
	if (IsDamageReaction())
	{
		damageReactionTimer_ -= dt;

		// 次の状態へ移行するかどうかのフラグ
		bool shouldTransition = false;

		// ダウン中のリアクションは、タイマーではなく、立ち上がり条件で判定する
		if (currentDamageReaction_ == DamageReaction::DownLying)
		{
			shouldTransition = CheckGetUpCondition();
		} 
		else
		{
			// それ以外のリアクションはタイマーで判定
			shouldTransition = (damageReactionTimer_ <= 0.0f);
		}
		
		// タイマーが0以下になったら、次の状態へ移行する
		if (shouldTransition)
		{
			// 怯みの状態遷移
			switch (currentDamageReaction_)
			{
			case DamageReaction::LightStagger:
			case DamageReaction::HeavyStagger:
				// 怯みが終わったら通常状態へ
				currentDamageReaction_ = DamageReaction::None;
				break;

			case DamageReaction::DownFalling:
				// ダウン落下が終わったら、ダウン中状態へ
				currentDamageReaction_ = DamageReaction::DownLying;
				damageReactionTimer_ = 2.0f;
				SetAnimation(hDownLyingMotion_, true, true);
				break;

			case DamageReaction::DownLying:
				// ダウン中が終わったら、立ち上がり状態へ
				currentDamageReaction_ = DamageReaction::DownGettingUp;
				damageReactionTimer_ = 1.0f;
				SetAnimation(hDownGetUpMotion_, true, false);
				break;

			case DamageReaction::DownGettingUp:
				// 立ち上がりが終わったら、通常状態へ
				currentDamageReaction_ = DamageReaction::None;
				break;
			}
		}
	}

	// 回避中は回避移動のみ更新する
	if (isAvoid_)
	{
		UpdateAvoid(dt);
	}

	// 構え中のみロックオン候補を更新する
	UpdateLockOnTargets();

	// ロックオンターゲットがいる場合は、ターゲット方向を向く
	if (lockOnTarget_ && !IsGrabbing())
	{
		// 自分からターゲットへの方向を計算する
		Vector3 toTarget = lockOnTarget_->worldTransform_->translate_ - worldTransform_->translate_;
		toTarget.y = 0.0f;

		// 十分な距離がある場合のみ目標回転を更新する
		const float targetLengthSq = toTarget.x * toTarget.x + toTarget.z * toTarget.z;
		if (targetLengthSq > kRotateThreshold)
		{
			targetYaw_ = std::atan2(toTarget.x, toTarget.z);
			hasTargetYaw_ = true;
		}
	}

	// 目標回転が有効な場合、現在の回転と目標回転の差を計算して、線形補間で回転を更新する
	if (hasTargetYaw_)
	{
		float currentYaw = worldTransform_->rotate_.y;

		// 現在の回転と目標回転の差を計算する
		const float deltaYaw = std::atan2(std::sin(targetYaw_ - currentYaw), std::cos(targetYaw_ - currentYaw));

		// 目標回転に向かって秒基準の補間で回転を更新する
		constexpr float kRotateLerpSpeedPerSecond = 12.0f;
		const float rotateLerpT = 1.0f - std::exp(-kRotateLerpSpeedPerSecond * dt);
		worldTransform_->rotate_.y = currentYaw + deltaYaw * rotateLerpT;

		// 回転が目標回転に十分近い場合、回転を目標回転に設定して、目標回転を無効にする
		if ((deltaYaw * deltaYaw) <= kRotateThreshold)
		{
			worldTransform_->rotate_.y = targetYaw_;
			hasTargetYaw_ = false;
		}
	}


	// ガードリアクション中は、目標回転に向かって回転を補間する
	if (isGuardReaction_)
	{
		// 現在のY軸回転角度を取得
		float currentRotationY = worldTransform_->rotate_.y;

		// 目標角度との差分を計算
		float diff = targetRotationY_ - currentRotationY;

		// 角度の差分を -π ～ +π ( -180度 ～ 180度 ) の範囲に正規化する
		while (diff > std::numbers::pi)
		{
			diff -= 2.0f * std::numbers::pi_v<float>;
		}
		while (diff < -std::numbers::pi)
		{
			diff += 2.0f * std::numbers::pi_v<float>;
		}

		// 補間（デルタタイムを掛けてフレームレート非依存にする）
		currentRotationY += diff * rotationSpeed_ * engine_->GetDeltaTime();

		// モデルに新しい角度を適用
		worldTransform_->rotate_.y = currentRotationY;
	}


	// 補間後のY回転から向いている方向ベクトルを更新する
	direction_.x = std::sin(worldTransform_->rotate_.y);
	direction_.y = 0.0f;
	direction_.z = std::cos(worldTransform_->rotate_.y);

	// 速度の更新
	const float velocityLerpT = 1.0f - std::exp(-velocityLerpSpeed_ * dt);
	currentVelocity_ = Lerp(currentVelocity_, targetVelocity_, velocityLerpT);

	// 位置の更新
	worldTransform_->translate_ += currentVelocity_ * dt;


	// 押し出し判定の更新
	UpdatePushOut();

	// アニメーションの更新
	UpdateAnimation();

	// 基底クラスの更新
	Entity::Update();
}

/// @brief ダメージを受ける
/// @param damage 
/// @param staggerTime
/// @param knockback
/// @param knockDirection
bool Character::OnDamage(int damage, DamageReaction damageReaction, float knockback, const Vector3& knockDirection, const Vector3& enemyPosition)
{
	if (IsGuard())
	{
		// 移動を強制停止
		MoveStop();

		// 現在実行中のアクションを強制キャンセル
		currentAttack_ = nullptr;
		currentMove_ = nullptr;
		currentAvoid_ = nullptr;

		// 状態フラグのリセット
		isAvoid_ = false;
		isDash_ = false;
		bufferedAttackInput_ = AttackInputType::None;

		// 自身から攻撃者へのベクトルを計算
		Vector3 dirToAttacker = enemyPosition - GetWorldPosition();
		dirToAttacker.y = 0.0f; // Y軸(高さ)は無視して水平方向のみにする
		if (dirToAttacker.Length() > 0.0f)
		{
			dirToAttacker = dirToAttacker.Normalize();
			
			// 目標回転を攻撃者の方向に設定する（ガード成功のリアクションで振り向くため）
			targetRotationY_ = std::atan2(dirToAttacker.x, dirToAttacker.z);
		}

		// ガード成功のフラグを立てる
		isGuardReaction_ = true;
		guardReactionTimer_ = 0.0f;

		//ノックバック用のベクトルを設定（攻撃方向の逆、または後退ベクトルなど）
		knockbackVelocity_ = -dirToAttacker * 2.0f; 

		// 防御成功モーションを再生
		SetAnimation(hGuardHitMotion_, false, true);

		// ガードされ、ダメージが通らなかったことを返す
		return false;
	}
	else
	{
		// 体力を減らす
		hp_ -= damage;

		// 移動を強制停止
		MoveStop();

		// 現在実行中のアクションを強制キャンセル
		currentAttack_ = nullptr;
		currentMove_ = nullptr;
		currentAvoid_ = nullptr;

		// 状態フラグのリセット
		isAvoid_ = false;
		isDash_ = false;
		bufferedAttackInput_ = AttackInputType::None;

		// ノックバック処理
		if (knockback > 0.0f)
		{
			// ノックバック方向を正規化する
			Vector3 backDirection = knockDirection.Normalize();

			// ノックバック力を初速として設定する
			// ※減衰させながら移動するため、少し大きめの値（* 10.0f など）をかけると丁度良くなります
			knockbackVelocity_ = backDirection * (knockback * 10.0f);
		}


		// リアクション状態を更新
		currentDamageReaction_ = damageReaction;

		// 状態に合わせてモーションを再生し、タイマーを設定する
		switch (currentDamageReaction_)
		{
		case DamageReaction::LightStagger:
			SetAnimation(hDamageLightMotion_, true, false); // ループしない
			damageReactionTimer_ = 0.3f; // 弱怯み時間
			break;

		case DamageReaction::HeavyStagger:
			SetAnimation(hDamageHeavyMotion_, true, false);
			damageReactionTimer_ = 1.0f; // 強怯み時間
			break;

		case DamageReaction::DownFalling:
			SetAnimation(hDownFallMotion_, true, false);
			damageReactionTimer_ = 0.3f; // 最初の倒れ込み時間
			break;
		}

		// 体力が0以下になったら死亡フラグを立てる
		if (hp_ <= 0)
		{
			hp_ = 0;
			isDead_ = true;
		}

		// ダメージが通ったことを返す
		return true;
	}
}

/// @brief ダウンからの起き上がり条件を満たしているかどうか
/// @return 
bool Character::CheckGetUpCondition()
{
	// ダウン中の時間が十分経過しているかどうか
	return damageReactionTimer_ <= 0.0f && currentDamageReaction_ == DamageReaction::DownLying;
}

/// @brief 回避を開始する
/// @param moveInputDirection
/// @param hasMoveInput
/// @param cameraYaw
void Character::StartAvoid(const Vector2& moveInputDirection, bool hasMoveInput, float cameraYaw)
{
	// 新しい連続回避の開始時に回数を初期化する
	if (!isAvoid_ && currentAvoidCount_ == 0)
	{
		currentAvoidCount_ = 1;
	}

	// 回避開始時にダッシュは解除する
	isDash_ = false;

	// 回避方向を決定する
	const Vector2 avoidDirection = GetAvoidDirection(moveInputDirection, hasMoveInput, cameraYaw);

	// 回避パラメータを初期化する
	isAvoid_ = true;
	avoidElapsedTime_ = 0.0f;
	avoidStartPosition_ = worldTransform_->translate_;
	avoidEndPosition_ = avoidStartPosition_ + Vector3(avoidDirection.x * avoidDistance_, 0.0f, avoidDirection.y * avoidDistance_);

	// 通常移動は停止して回避移動へ移行する
	MoveStop();
}

/// @brief 連続回避を試行する
/// @param moveInputDirection
/// @param hasMoveInput
/// @param cameraYaw
void Character::ReserveNextAvoid(const Vector2& moveInputDirection, bool hasMoveInput, float cameraYaw)
{
	// 回避中でない場合は何もしない
	if (!isAvoid_)
	{
		return;
	}

	// 最大連続回避回数に達している場合は連続回避できない
	if (currentAvoidCount_ >= maxConsecutiveAvoidCount_)
	{
		return;
	}

    // 現在位置から次の連続回避を即時開始する
	++currentAvoidCount_;
	StartAvoid(moveInputDirection, hasMoveInput, cameraYaw);
}

/// @brief 回避中の更新処理
/// @param deltaTime
void Character::UpdateAvoid(float deltaTime)
{
	// 回避時間を進める
	avoidElapsedTime_ += deltaTime;

	// 開始位置から終了位置まで線形補間で移動する
	const float t = std::clamp<float>(avoidElapsedTime_ / avoidDuration_, 0.0f, 1.0f);
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

	// 長さが0以下の場合、目標速度を0にして終了する
	if (length <= 0.0f)
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
	// 構えていない場合はターゲット情報をクリアする
	if (!isStance_ && !canLockOnWithoutStance_)
	{
		lockOnTarget_ = nullptr;
		return;
	}

	// すでにターゲット確定済みの場合は再検索しない
	if (lockOnTarget_)
		return;

	// ターゲットをクリアする
	lockOnTarget_ = nullptr;

	// 視線方向との内積が最大の相手を優先ターゲットにする
	float bestDot = -1.0f;
	float bestDistance = 0.0f;

	// 自分とは反対側の陣営を候補にする
	const CharacterTag targetSide = (characterTag_ == CharacterTag::PlayerSide) ? CharacterTag::EnemySide : CharacterTag::PlayerSide;
	const Vector3 selfPosition = worldTransform_->translate_;

	for (Character* character : characters_)
	{
		// 無効または自分自身は除外する
		if (!character || character == this)
			continue;

		// 別の側ではない相手は除外する
		if (character->GetCharacterTag() != targetSide)
			continue;

		// 自分から相手へのベクトルを計算する
		Vector3 toTarget = character->worldTransform_->translate_ - selfPosition;
		toTarget.y = 0.0f;

		// 距離の二乗を計算する
		const float distanceSq = toTarget.x * toTarget.x + toTarget.z * toTarget.z;
		if (distanceSq <= 0.0f)
			continue;

		// 目の前にいる相手のみリストに登録する
		const Vector3 toTargetDirection = toTarget.Normalize();
		if (Dot(direction_, toTargetDirection) <= 0.0f)
		{
			continue;
		}

		// 距離と相手ポインタを登録する
		const float distance = std::sqrt(distanceSq);

		// 視線方向との内積が1.0fに最も近い相手を優先する
		const float viewDot = Dot(direction_, toTargetDirection);
		if (viewDot > bestDot)
		{
			bestDot = viewDot;
			bestDistance = distance;
			lockOnTarget_ = character;
		}
		else if (viewDot == bestDot && distance < bestDistance)
		{
			// 内積が同じ場合は近い相手を優先する
			bestDistance = distance;
			lockOnTarget_ = character;
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

	if (!currentAttack_ && !IsDamageReaction())
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

	// 掴まれモーションを優先して再生する
	if (IsGrabbed())
	{
		SetAnimation(hGrabbedMotion_, false, true);
	}
	else if (isGuardReaction_)
	{
		// 防御成功時のノックバック中
		guardReactionTimer_ += engine_->GetDeltaTime();
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


	if (isAnimationLoop_)
	{
		// タイマーを進める
		model_->param_->animation.timer += engine_->GetDeltaTime();

		// アニメーションをループさせる
		model_->param_->animation.timer = std::fmod(model_->param_->animation.timer, animationTime_);
	}
	else
	{
		// アニメーションの時間よりもタイマーが大きい場合は更新しない
		if (model_->param_->animation.timer > animationTime_)
			return;

		// タイマーを進める
		model_->param_->animation.timer += engine_->GetDeltaTime();

		// アニメーションの時間を超えないようにする
		model_->param_->animation.timer = std::min(model_->param_->animation.timer, animationTime_);
	}
}

/// @brief ボーン行列を取得する
/// @param partName 
/// @return 
Matrix4x4 Character::GetBoneMatrix(const std::string& partName) const
{
	// モーションマネージャからジョイント名を取得する
	std::string jointName = motionManager_->GetJointName(partName);
	return model_->GetBoneWorldMatrix(jointName);
}

/// @brief 相手をつかむ
/// @param target 
void Character::ExecuteGrab(Character* target, float duration)
{
	grabbedTarget_ = target;
	target->grabber_ = this;
	target->grabbedTimer_ = 0.0f;

	// 必要ならここで双方専用の「つかみ合い待機モーション」をセットする
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
		if (IsDown() || other->IsDown() || IsGrabbed() || other->IsGrabbed() || IsGrabbing() || other->IsGrabbing())
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