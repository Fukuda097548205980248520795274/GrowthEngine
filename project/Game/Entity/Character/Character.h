#pragma once
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

	/// @brief モデル
	Render3DSkinningModel* model_ = nullptr;
};

