#pragma once
#include "../Action.h"

struct AvoidInitData
{
	// 回避方向
	Vector2 localDirection = Vector2(0.0f, -1.0f);

	// 回避時間
	float time = 0.3f;

	// 回避距離
	float distance = 1.0f;

	/// @brief ターゲット方向を向くかどうか
	bool isTargetDirection = false;
};

class Avoid : public Action
{
public:

	/// @brief デストラクタ
	~Avoid();

	/// @brief コンストラクタ
	/// @param character 
	/// @param initData 
	Avoid(Character* character, const AvoidInitData& initData);

	/// @brief 実行
	void Exec() override;

	/// @brief 終了、中断
	void Exit() override;

	/// @brief リセット
	void Reset() override;

	/// @brief 更新処理
	void Update() override;

	/// @brief 使用中かどうか
	/// @return 
	bool IsUse() const override;


private:

	// 回避方向 ローカル
	Vector2 localDirection_ = Vector2(0.0f, -1.0f);

	// 回避時間
	float time_ = 0.3f;

	// 回避距離
	float distance_ = 1.0f;

	/// @brief ターゲット方向を向くかどうか
	bool isTargetDirection_ = false;


private:

	// タイマー
	float timer_ = 0.0f;
};

