#include "NPC.h"

/// @brief コンストラクタ
/// @param initData 
NPC::NPC(const InitData& initData, CharacterTag characterTag) : 
	Character(initData)
{
	// タグを指定する
	characterTag_ = characterTag;
}

/// @brief 初期化
void NPC::Initialize()
{
	// モデルをワールドトランスフォームの子にする
	model_->SetParent(worldTransform_.get());
}

void NPC::Update()
{
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