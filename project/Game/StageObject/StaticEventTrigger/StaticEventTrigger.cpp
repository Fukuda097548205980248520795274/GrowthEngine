#include "StaticEventTrigger.h"

/// @brief コンストラクタ
/// @param initData 
StaticEventTrigger::StaticEventTrigger() : StageObject()
{
	// タグ
	tag_ = StageObjectTag::StaticEventTrigger;
}

/// @brief デストラクタ
StaticEventTrigger::~StaticEventTrigger()
{
	// 衝突判定の削除
	if (collision_)
	{
		collision_->Delete();
		collision_ = nullptr;
	}
}

/// @brief 初期化
/// @param initData 
void StaticEventTrigger::Initialize(const InitData& initData)
{
	// nullptr
	assert(initData.collision != nullptr);

	// 位置
	worldTransform_->translate_ = initData.position;

	//　大きさ
	worldTransform_->scale_ = initData.scale;

	// 衝突判定
	collision_ = initData.collision;

	// イベントの種類
	eventType_ = initData.eventType;

	// イベントの整数パラメータ
	eventStringParam_ = initData.eventStringParam;

	// イベントが発生したときのコールバック関数
	onTriggerCallback_ = initData.onTriggerCallback;

	// ワールドトランスフォームを更新する
	worldTransform_->Update();
}

/// @brief 更新処理
void StaticEventTrigger::Update()
{
	// 基底クラスの更新処理
	StageObject::Update();

	// 衝突判定のパラメータを更新
	collision_->param_->center = GetWorldPosition();
	collision_->param_->radius = worldTransform_->scale_;

	// 衝突しているかどうか
	if (collision_->isCollision_)
	{
		// コールバック関数を呼び出す
		if (onTriggerCallback_)
			onTriggerCallback_(eventType_, eventStringParam_);

		// 削除する
		Delete();
	}
}