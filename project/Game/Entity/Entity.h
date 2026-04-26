#pragma once
#include "GrowthEngine.h"

class Entity
{
public:

	/// @brief タグ
	/// @note プレイヤーや敵などの識別に使用する
	enum EntityTag
	{
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

	/// @brief 体力を取得する
	/// @return 
	int GetHp()const { return hp_; }

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


protected:

	/// @brief 終了したかどうか
	bool isFinished_ = false;

	/// @brief エンティティのタグ
	EntityTag entityTag_;

	/// @brief ワールドトランスフォーム
	std::unique_ptr<WorldTransform3D> worldTransform_ = nullptr;

	/// @brief 体力
	int hp_ = 0;
};

