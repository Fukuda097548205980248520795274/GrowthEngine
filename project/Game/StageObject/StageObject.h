#pragma once
#include "GrowthEngine.h"

struct PlacementData;

class StageObject
{
public:

	// ステージオブジェクトのタグ
	enum class StageObjectTag
	{
		None,
		Floor,
		Wall
	};


public:

	/// @brief コンストラクタ
	StageObject();

	/// @brief 仮想デストラクタ
	virtual ~StageObject() = default;

	/// @brief 更新処理
	virtual void Update();

	/// @brief ステージオブジェクトのタグを取得する
	/// @return 
	StageObjectTag GetTag() const { return tag_; }

	/// @brief ワールド座標を取得する
	/// @return 
	Vector3 GetWorldPosition()const { return worldTransform_->GetWorldPosition(); }

	/// @brief 終了したかどうか
	/// @return 
	bool IsFinished() const { return isFinished_; }

	/// @brief 終了する
	void Delete() { isFinished_ = true; }



protected:

	/// @brief ワールドトランスフォーム
	std::unique_ptr<WorldTransform3D> worldTransform_ = nullptr;

	/// @brief 終了フラグ
	bool isFinished_ = false;

	// ステージオブジェクトのタグ
	StageObjectTag tag_ = StageObjectTag::None;


public:

	/// @brief デバッグUIを描画する
	virtual void DrawUI(PlacementData* placementData);

	/// @brief ステージオブジェクトの更新を有効にするかどうかを設定する
	/// @param enabled 
	static void SetUpdateEnabled(bool enabled) { updateEnabled_ = enabled; }

protected:

	// ステージオブジェクトの更新を有効にするかどうか
	static bool updateEnabled_;
};

