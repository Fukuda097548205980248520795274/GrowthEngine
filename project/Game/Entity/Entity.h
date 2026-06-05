#pragma once
#include "GrowthEngine.h"

// 前方宣言
struct PlacementData;

class Entity
{
public:

	/// @brief タグ
	/// @note プレイヤーや敵などの識別に使用する
	enum class EntityTag
	{
		None,
		Character,
		Item,
	};


public:

	
	/// @brief コンストラクタ 位置を指定して生成する
	/// @param position 
	Entity();

	/// @brief コンストラクタ
	virtual ~Entity() = default;

	/// @brief 更新処理
	virtual void Update();
	
	/// @brief 終了したかどうか
	/// @return 
	bool IsFinished()const { return isFinished_; }

	/// @brief エンティティのタグを取得する
	/// @return 
	EntityTag GetEntityTag()const { return entityTag_; }

	/// @brief 位置を取得する
	/// @return 
	Vector3 GetPosition()const { return worldTransform_->translate_; }

	/// @brief 位置を設定する
	/// @param position 
	void SetPosition(const Vector3& position) { worldTransform_->translate_ = position; }

	/// @brief ワールド座標を取得する
	/// @return 
	Vector3 GetWorldPosition()const { return worldTransform_->GetWorldPosition(); }

	/// @brief 回転を取得する
	/// @return 
	Quaternion GetRotation()const { return worldTransform_->GetQuaternion(); }

	/// @brief 回転を設定する
	/// @param rotation 
	void SetRotation(const Quaternion& rotation) { worldTransform_->SetRotate(rotation); }

	/// @brief ワールドトランスフォームを取得する
	/// @return 
	WorldTransform3D* GetWorldTransform()const { return worldTransform_.get(); }

	/// @brief 削除する
	void Delete() { isFinished_ = true; }


protected:

	/// @brief 終了したかどうか
	bool isFinished_ = false;

	/// @brief エンティティのタグ
	EntityTag entityTag_;

	/// @brief ワールドトランスフォーム
	std::unique_ptr<WorldTransform3D> worldTransform_ = nullptr;


public:

	/// @brief デバッグUIを描画する
	virtual void DrawDebugUI(PlacementData* placementData);

	/// @brief 更新処理が有効かどうかを設定する
	/// @param enabled 
	static void SetUpdateEnabled(bool enabled) { updateEnabled_ = enabled; }


protected:

	// 更新処理が有効かどうか
	static bool updateEnabled_;
};

