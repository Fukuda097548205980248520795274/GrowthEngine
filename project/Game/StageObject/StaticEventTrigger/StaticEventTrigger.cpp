#include "StaticEventTrigger.h"
#include <numbers>
#include "StageEditor/StageData/StageData.h"
#include "StageEditor/StageEditorHistory/StageEditorHistory.h"

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

	// イベントの種類
	eventType_ = initData.eventType;

	// イベントが発生したときのコールバック関数
	onTriggerCallback_ = initData.onTriggerCallback;

	// 戦闘エリア開始フラグ
	isStartBattleArea_ = initData.isStartBattleArea;

	// ゲームクリアフラグ
	isGameClear_ = initData.isGameClear;

	// ナビメッシュのグループID
	navMeshGroupId_ = initData.navMeshGroupId;

	// ナビメッシュの状態
	isNavMeshEnabled_ = initData.isNavMeshEnabled;

	// イベントの整数パラメータ
	strcpy_s(eventStageDataFileName_, sizeof(eventStageDataFileName_), initData.eventStageDataFileName);

	// ワールドトランスフォームを更新する
	worldTransform_->Update();

	// 衝突判定
	if (initData.collision)
	{
		collision_ = initData.collision;

		// 衝突判定のパラメータを更新
		collision_->param_->center = GetWorldPosition();
		collision_->param_->radius = worldTransform_->scale_;
	}
}

/// @brief 更新処理
void StaticEventTrigger::Update()
{
	// 基底クラスの更新処理
	StageObject::Update();

	// 衝突判定のパラメータを更新
	if (collision_)
	{
		collision_->param_->center = GetWorldPosition();
		collision_->param_->radius = worldTransform_->scale_;
	}

	if (!updateEnabled_)return;

	// 衝突判定のパラメータを更新
	if (collision_)
	{
		// 衝突しているかどうか
		if (collision_->isCollision_)
		{
			// コールバック関数を呼び出す
			if (onTriggerCallback_)
			{
				bool shouldDelete =
					onTriggerCallback_(eventType_, eventStageDataFileName_, isStartBattleArea_, isGameClear_, navMeshGroupId_, isNavMeshEnabled_);

				// イベントが発生したときのコールバック関数がtrueを返した場合は削除する
				if (shouldDelete)Delete();
			}
		}
	}
}
