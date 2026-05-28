#include "NPC.h"
#include "BattleDirector/BattleDirector.h"

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
}

/// @brief 初期化
void NPC::Initialize(std::unique_ptr<BehaviorTree> behaviorTree)
{
	if (characterTag_ == CharacterTag::PlayerSide)
	{
		model_->param_->meshMaterial[0].color = Vector4(0.1f, 0.1f, 1.0f, 1.0f);
	}
	else if (characterTag_ == CharacterTag::EnemySide)
	{
		model_->param_->meshMaterial[0].color = Vector4(1.0f, 0.1f, 0.1f, 1.0f);
	}

	// ビヘイビアツリーを設定する
	behaviorTree_ = std::move(behaviorTree);

	// モデルをワールドトランスフォームの子にする
	model_->SetParent(worldTransform_.get());
}

void NPC::Update()
{
	// デルタタイムを取得する
	float dt = engine_->GetDeltaTime() * engine_->GetTimeScale();

	// 動けない状態かどうか
	bool isIncapacitated = IsIncapacitated();

	// 更新処理開始前のリセット
	StartUpdate();

	// 攻撃のクールタイムの更新
	UpdateAttackCooltime(dt);

	// 動ける状態なら、ビヘイビアツリーを実行する
	if (!isIncapacitated)
	{
		// ビヘイビアツリーを実行する
		if (behaviorTree_)
			behaviorTree_->Exec();
	}

	// アクションの更新
	ActionUpdate();

	// 動けない状態なら、攻撃トークンを返却して、基底クラスの更新処理を行って終了する
	if (isIncapacitated)
	{
		// つかまれている状態なら、つかまれ解き入力を受け付けて、入力があればつかまれ解きの処理を行う
		if(IsGrabbed())
		{
			grabbedTimer_ += engine_->GetDeltaTime();
		}

		// 攻撃トークンを返却する
		BattleDirector::GetInstance().ReleaseAttackToken(this);

		Character::Update();
		return;
	}

	UpdateStanceStateByTargetDistance();

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

	Vector3 toTarget = lockOnTarget_->GetWorldPosition() - GetWorldPosition();
	toTarget.y = 0.0f;

	const float distanceSq = toTarget.x * toTarget.x + toTarget.z * toTarget.z;
	isStance_ = (distanceSq <= kNpcStanceDistanceSq);
}

/// @brief スタイルが変化したときの処理
/// @param newStyle 
void NPC::OnStyleChanged(FightStyle newStyle)
{
	// スタイルに応じたビヘイビアツリーを生成して設定する
	behaviorTree_ = CreateBehaviorTreeForStyle(newStyle);
}

/// @brief スタイルに応じたビヘイビアツリーを生成する
/// @param style 
/// @return 
std::unique_ptr<BehaviorTree> NPC::CreateBehaviorTreeForStyle(FightStyle style)
{
	// ビヘイビアツリー
	std::unique_ptr<BehaviorTree> behaviorTree = nullptr;

	switch (style)
	{
	// 旋嵐スタイル
	case FightStyle::Tempest:

		break;

	// 撃鉄スタイル
	case FightStyle::Hammer:

		break;
	}

	return behaviorTree;
}

/// @brief 描画処理
void NPC::Draw()
{
	assert(model_);

	// モデルを描画する
	model_->Draw();
}

/// @brief 攻撃クールタイムの更新処理
/// @param deltaTime 
void NPC::UpdateAttackCooltime(float deltaTime)
{
	// 既にクールタイムが0以下なら何もしない
	if (attackCooltime_ <= 0.0f)return;

	// クールタイムを減らす
	attackCooltime_ -= deltaTime;
	attackCooltime_ = std::max(0.0f, attackCooltime_);
}