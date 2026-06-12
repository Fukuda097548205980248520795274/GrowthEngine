#pragma once
#include "GrowthEngine.h"

struct PlacementData;
class StageEditorHistory;

class StageObject
{
public:

	// ステージオブジェクトのタグ
	enum class StageObjectTag
	{
		None,
		Floor,
		Wall,
		StaticEventTrigger, // 静的イベントトリガー
	};


public:

	/// @brief コンストラクタ
	StageObject();

	/// @brief 仮想デストラクタ
	virtual ~StageObject() = default;

	/// @brief 更新処理
	virtual void Update();

	/// @brief 描画処理
	virtual void Draw() {};

	/// @brief ステージオブジェクトのタグを取得する
	/// @return 
	StageObjectTag GetTag() const { return tag_; }

	/// @brief ワールド座標を取得する
	/// @return 
	Vector3 GetWorldPosition()const { return worldTransform_->GetWorldPosition(); }

	/// @brief ワールドトランスフォームを取得する
	/// @return 
	WorldTransform3D* GetWorldTransform() const { return worldTransform_.get(); }

	/// @brief 終了したかどうか
	/// @return 
	bool IsFinished() const { return isFinished_; }

	/// @brief 終了する
	void Delete() { isFinished_ = true; }

	/// @brief 位置を設定する
	/// @param position 
	void SetPosition(const Vector3& position) { worldTransform_->translate_ = position; }

	/// @brief 回転を設定する
	/// @param rotation 
	void SetRotation(const Vector3& rotation) { worldTransform_->rotate_ = rotation; }

	/// @brief 拡縮を設定する
	/// @param scale 
	void SetScale(const Vector3& scale) { worldTransform_->scale_ = scale; }



protected:

	/// @brief ワールドトランスフォーム
	std::unique_ptr<WorldTransform3D> worldTransform_ = nullptr;

	/// @brief 終了フラグ
	bool isFinished_ = false;

	// ステージオブジェクトのタグ
	StageObjectTag tag_ = StageObjectTag::None;


public:

	/// @brief デバッグUIを描画する
	/// @param placementData 
	/// @param placementList 
	/// @param history 
	/// @param isDirty 
	virtual void DrawDebugUI(PlacementData* placementData, std::vector<PlacementData>& placementList, StageEditorHistory* history, bool* isDirty) {};

	/// @brief ステージオブジェクトの更新を有効にするかどうかを設定する
	/// @param enabled 
	static void SetUpdateEnabled(bool enabled) { updateEnabled_ = enabled; }

protected:

	// ステージオブジェクトの更新を有効にするかどうか
	static bool updateEnabled_;
};

