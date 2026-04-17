#pragma once
#define NOMINMAX
#include "../Entity.h"

class Character : public Entity
{
public:

	/// @brief キャラクターのタグ
	enum class CharacterTag
	{
		PlayerSide,
		EnemySide,
	};

	/// @brief 初期化用データ
	struct InitData
	{
		/// @brief 位置
		Vector3 position = Vector3(0.0f, 0.0f, 0.0f);

		/// @brief 体力
		int hp = 0;
		
		/// @brief モデル
		Render3DSkinningModel* model_ = nullptr;
	};


public:

	/// @brief コンストラクタ
	/// @param initData 
	Character(const InitData& initData);

	/// @brief 更新処理
	virtual void Update() override;

	/// @brief キャラクターのタグを取得する
	/// @return 
	CharacterTag GetCharacterTag()const { return characterTag_; }

	/// @brief 移動を停止させる
	void MoveStop();

	/// @brief XZ平面の移動入力を設定する
	/// @param direction
	/// @param maxSpeed
	void SetMoveInputXZ(const Vector2& direction, float maxSpeed);


protected:

	// キャラクターのタグ
	CharacterTag characterTag_;


protected:

	// 速度の線形補間の速度
	float velocityLerpSpeed_ = 0.1f;

	/// @brief 目標速度
	Vector3 targetVelocity_ = Vector3(0.0f, 0.0f, 0.0f);

	/// @brief 現在の速度
	Vector3 currentVelocity_ = Vector3(0.0f, 0.0f, 0.0f);


protected:

	/// @brief 目標Y回転
	float targetYaw_ = 0.0f;

	/// @brief 目標回転が有効かどうか
	bool hasTargetYaw_ = false;

	// 入力に応じて目標速度と目標回転を更新する
	static constexpr float kRotateThreshold = 0.0001f;


protected:

	/// @brief モデル
	Render3DSkinningModel* model_ = nullptr;
};

