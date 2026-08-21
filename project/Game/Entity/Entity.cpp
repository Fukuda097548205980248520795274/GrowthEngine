#include "Entity.h"
#include "StageEditor/StageData/StageData.h"
#include "StageEditor/StageEditorHistory/StageEditorHistory.h"

// 静的メンバの定義
bool Entity::updateEnabled_ = false;

/// @brief コンストラクタ 位置を指定して生成する
/// @param position 
Entity::Entity()
{
	// ワールドトランスフォームを生成
	worldTransform_ = std::make_unique<WorldTransform3D>();
}

/// @brief 更新処理
void Entity::Update()
{
	if (!updateEnabled_)return;

	// ワールドトランスフォームの更新
	worldTransform_->Update();
}

/// @brief ビューポート内に存在するかどうかを取得する
/// @return 
bool Entity::IsInViewport() const
{
	return false;
}