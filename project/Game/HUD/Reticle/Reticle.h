#pragma once
#include "../HUD.h"

class Character;

class Reticle : public HUD
{
public:

	/// @brief デストラクタ
	~Reticle();

	/// @brief 初期化処理
	/// @param leftUpFrame 
	/// @param rightUpFrame 
	/// @param leftDownFrame 
	/// @param rightDownFrame 
	/// @param centerPoint 
	void Initialize(PrefabInstanceSprite* leftUpFrame, PrefabInstanceSprite* rightUpFrame, PrefabInstanceSprite* leftDownFrame, PrefabInstanceSprite* rightDownFrame,
		Sprite* centerPoint);

	/// @brief 更新処理
	void Update() override;

	/// @brief 描画処理
	void Draw() override;

	/// @brief ロックオンする
	/// @param position 
	void LockOn(const Character* lockOnTarget);

	/// @brief ロックオンをし始めたかどうか
	/// @return 
	bool IsStartingLockOn() const { return isLockOn_ && !isPrevLockOn_; }

	/// @brief ロックオンを終了したかどうか
	/// @return 
	bool IsEndingLockOn() const { return !isLockOn_ && isPrevLockOn_; }

	/// @brief ロックオンしているかどうか
	/// @return 
	bool IsLockOn() const { return isLockOn_ || isPrevLockOn_; }



private:

	// 枠
	PrefabInstanceSprite* leftUpFrame_ = nullptr;
	PrefabInstanceSprite* rightUpFrame_ = nullptr;
	PrefabInstanceSprite* leftDownFrame_ = nullptr;
	PrefabInstanceSprite* rightDownFrame_ = nullptr;

	// 中心点
	Sprite* centerPoint_ = nullptr;


private:

	/// @brief ロックオンしているかどうか
	bool isLockOn_ = false;

	/// @brief 前回のロックオン状態
	bool isPrevLockOn_ = false;


private:

	// ロックオン開始までの時間
	static constexpr float kStartLockOnTime = 1.0f;

	// ロックオン開始までのタイマー
	float startLockOnTimer_ = kStartLockOnTime;


	/// @brief ロックオン対象のキャラクター
	const Character* lockOnTarget_ = nullptr;

};

