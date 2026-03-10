#include "Player.h"
#include "Context/Context.h"

/// @brief 初期化
/// @param context 
void Player::Initialize(Context* context)
{
	// nullptrチェック
	assert(context);

	// 引数を受け取る
	context_ = context;
}

/// @brief 更新処理
void Player::Update()
{
	// 左に進む
	if (context_->keyInputPlayerLeftMove_->IsInput())
	{

	}

	// 右に進む
	if (context_->keyInputPlayerRightMove_->IsInput())
	{

	}

	// 基底クラス更新
	Object::Update();
}