#include "Character.h"
#include <algorithm>
#include <cmath>

#include "Action/Attack/Attack.h"
#include "Action/Move/Move.h"

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

	// タグを指定する
	entityTag_ = EntityTag::Character;

	// 位置
	worldTransform_->translate_ = initData.position;

	// 体力
	hp_ = initData.hp;

	// モデル
	if(initData.model_)
		model_ = initData.model_;


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
	// デルタタイム(秒)を取得する
	const float deltaTime = std::max(GrowthEngine::GetInstance()->GetDeltaTime(), 0.0f);

	// 現在の攻撃がある場合は更新する
	if(currentAttack_)
		currentAttack_->Update();

	// 現在の移動がある場合は更新する
	if(currentMove_)
		currentMove_->Update();

	// 回避中は回避移動のみ更新する
	if (isAvoid_)
	{
		UpdateAvoid(deltaTime);
	}

	// 現在のY回転から向いている方向ベクトルを更新する
	direction_.x = std::sin(worldTransform_->rotate_.y);
	direction_.y = 0.0f;
	direction_.z = std::cos(worldTransform_->rotate_.y);

	// 構え中のみロックオン候補を更新する
	UpdateLockOnTargets();

	// ロックオンターゲットがいる場合は、ターゲット方向を向く
	if (lockOnTarget_)
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
		// 現在の回転と目標回転の差を計算する
		const float currentYaw = worldTransform_->rotate_.y;
		const float deltaYaw = std::atan2(std::sin(targetYaw_ - currentYaw), std::cos(targetYaw_ - currentYaw));

        // 目標回転に向かって秒基準の補間で回転を更新する
		constexpr float kRotateLerpSpeedPerSecond = 12.0f;
		const float rotateLerpT = 1.0f - std::exp(-kRotateLerpSpeedPerSecond * deltaTime);
		worldTransform_->rotate_.y = currentYaw + deltaYaw * rotateLerpT;

		// 回転が目標回転に十分近い場合、回転を目標回転に設定して、目標回転を無効にする
		if ((deltaYaw * deltaYaw) <= kRotateThreshold)
		{
			worldTransform_->rotate_.y = targetYaw_;
			hasTargetYaw_ = false;
		}
	}

	// 補間後のY回転から向いている方向ベクトルを更新する
	direction_.x = std::sin(worldTransform_->rotate_.y);
	direction_.y = 0.0f;
	direction_.z = std::cos(worldTransform_->rotate_.y);

	// 速度の更新
    const float velocityLerpT = 1.0f - std::exp(-velocityLerpSpeed_ * deltaTime);
	currentVelocity_ = Lerp(currentVelocity_, targetVelocity_, velocityLerpT);

	// 位置の更新
	worldTransform_->translate_ += currentVelocity_ * deltaTime;

	// 基底クラスの更新
	Entity::Update();
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

	// 目標速度のXZ成分の長さが回転の閾値より大きい場合、目標回転を更新する
	if ((targetVelocity_.x * targetVelocity_.x + targetVelocity_.z * targetVelocity_.z) > kRotateThreshold)
	{
		targetYaw_ = std::atan2(direction.x, direction.y);
		hasTargetYaw_ = true;
	}
}

// 構え中のロックオン候補を更新する
void Character::UpdateLockOnTargets()
{
	// 構えていない場合はターゲット情報をクリアする
	if (!isStance_)
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