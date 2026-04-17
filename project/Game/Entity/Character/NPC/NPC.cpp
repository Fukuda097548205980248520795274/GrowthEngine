#include "NPC.h"

/// @brief コンストラクタ
/// @param initData 
NPC::NPC(const InitData& initData) : Character(initData)
{
	// タグを指定する
	characterTag_ = CharacterTag::PlayerSide;
}

/// @brief 初期化
void NPC::Initialize()
{

}

void NPC::Update()
{
	// 基底クラスの更新
	Character::Update();


}