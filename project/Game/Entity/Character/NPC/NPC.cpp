#include "NPC.h"
#include "Factory/AttackTreeFactory/AttackTreeFactory.h"

namespace
{
	constexpr float kNpcStanceDistance = 5.0f;
	constexpr float kNpcStanceDistanceSq = kNpcStanceDistance * kNpcStanceDistance;
}

/// @brief コンストラクタ
/// @param initData 
NPC::NPC(const InitData& initData, CharacterTag characterTag) : 
	Character(initData)
{
	// タグを指定する
	characterTag_ = characterTag;

   // 構え状態でなくてもロックオン候補を更新する
	canLockOnWithoutStance_ = true;

	// ビヘイビアツリーを作成する
	AttackTreeFactory attackTreeFactory;
	behaviorTree_ = std::make_unique<BehaviorTree>(attackTreeFactory.CreateTestTree(this));
}

/// @brief 初期化
void NPC::Initialize()
{
	// モデルをワールドトランスフォームの子にする
	model_->SetParent(worldTransform_.get());
}

void NPC::Update()
{
	// 怯み状態、または「つかまれている状態」なら攻撃や移動の更新は行わず、基底クラスの更新のみ行う
	if (IsDamageReaction() || IsGrabbed() || IsDown())
	{
		// つかまれている状態なら、つかまれ解き入力を受け付けて、入力があればつかまれ解きの処理を行う
		if(IsGrabbed())
		{
			grabbedTimer_ += engine_->GetDeltaTime();
		}

		Character::Update();
		return;
	}

	UpdateStanceStateByTargetDistance();

	// ビヘイビアツリーを実行する
	if(behaviorTree_)
		behaviorTree_->Exec();

	// アクションの更新
	ActionUpdate();

	// 基底クラスの更新
	Character::Update();

	UpdateStanceStateByTargetDistance();
}

/// @brief ターゲットとの距離で構え状態を更新する
void NPC::UpdateStanceStateByTargetDistance()
{
	if (!lockOnTarget_)
	{
		isStance_ = false;
		return;
	}

	Vector3 toTarget = lockOnTarget_->GetPosition() - GetPosition();
	toTarget.y = 0.0f;

	const float distanceSq = toTarget.x * toTarget.x + toTarget.z * toTarget.z;
	isStance_ = (distanceSq <= kNpcStanceDistanceSq);
}

/// @brief 描画処理
void NPC::Draw()
{
	assert(model_);

	// モデルを描画する
	model_->Draw();
}