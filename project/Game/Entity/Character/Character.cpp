#include "Character.h"
#include "Character.h"
#include <algorithm>
#include <cmath>

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