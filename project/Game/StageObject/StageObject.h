#pragma once
#include "GrowthEngine.h"

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
};

