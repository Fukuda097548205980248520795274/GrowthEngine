#pragma once
#include "GrowthEngine.h"
#include "StageEditor/StageData/StageData.h"
#include "StageEditor/StageEditorHistory/StageEditorHistory.h"

class HUD
{
public:

	/// @brief タグ
	enum class Tag
	{
		None,
		PlayerHP,
		VipHp,
		AllyHP,
		EnemyHP,
		BossHP,
		Timer,
		AttackTutorial,
		GuardTutorial
	};


public:

	/// @brief コンストラクタ
	HUD();

	/// @brief 仮想デストラクタ
	virtual ~HUD() = default;

	/// @brief 更新処理
	virtual void Update();

	/// @brief 描画処理
	virtual void Draw() {};

	/// @brief コンストラクタ
	/// @return 
	Tag GetTag() const { return tag_; }

	/// @brief 終了したかどうか
	/// @return 
	bool IsFinished() const { return isFinished_; }

	/// @brief 終了する
	void Delete() { isFinished_ = true; }

	/// @brief 位置を設定する
	/// @param position 
	void SetPosition(const Vector2& position) { worldTransform_->translate_ = position; }

	/// @brief 位置を設定する
	/// @param position 
	void SetPosition(const Vector3& position);

	/// @brief ワールド座標を取得する
	/// @return 
	Vector2 GetWorldPosition() { return worldTransform_->GetWorldPosition(); }

	/// @brief 表示するかどうか
	/// @param visible 
	void SetVisible(bool visible) { isVisible_ = visible; }


protected:

	const GrowthEngine* engine_ = GrowthEngine::GetInstance();

	/// @brief ワールドトランスフォーム
	std::unique_ptr<WorldTransform2D> worldTransform_ = nullptr;

	/// @brief タグ
	Tag tag_ = Tag::None;

	/// @brief 終了したかどうか
	bool isFinished_ = false;

	/// @brief 表示するかどうか
	bool isVisible_ = true;


public:

	/// @brief デバッグ用描画処理
	virtual void DrawDebugUI(PlacementData* placementData, std::vector<PlacementData>& placementList, StageEditorHistory* history, bool* isDirty){}

	/// @brief HUDの更新を有効にするかどうかを設定する
	/// @param enabled 
	static void SetUpdateEnabled(bool enabled) { updateEnabled_ = enabled; }


protected:

	// HUDの更新を有効にするかどうか
	static bool updateEnabled_;
};

