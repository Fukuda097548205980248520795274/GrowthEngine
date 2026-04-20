#include "NPC.h"
#include "Factory/AttackTreeFactory/AttackTreeFactory.h"

/// @brief コンストラクタ
/// @param initData 
NPC::NPC(const InitData& initData, CharacterTag characterTag) : 
	Character(initData)
{
	// タグを指定する
	characterTag_ = characterTag;

	// ビヘイビアツリーを作成する
	AttackTreeFactory attackTreeFactory;
	behaviorTree_ = std::make_unique<BehaviorTree>(attackTreeFactory.CreateTestAttackTree(this));
}

/// @brief 初期化
void NPC::Initialize()
{
	// モデルをワールドトランスフォームの子にする
	model_->SetParent(worldTransform_.get());
}

void NPC::Update()
{
	behaviorTree_->Exec();

	// 基底クラスの更新
	Character::Update();
}

/// @brief 描画処理
void NPC::Draw()
{
	assert(model_);

	// モデルを描画する
	model_->Draw();
}