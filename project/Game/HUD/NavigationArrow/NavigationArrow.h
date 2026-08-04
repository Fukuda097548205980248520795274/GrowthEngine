#pragma once
#include "../HUD.h"

class Player;

class NavigationArrow : public HUD
{
public:

	/// @brief コンストラクタ
	/// @param player 
	void Initialize(Sprite* sprite);

	/// @brief 更新処理
	void Update() override;

	/// @brief 描画処理
	void Draw() override;

	/// @brief プレイヤーを設定する
	/// @param player 
	void SetPlayer(Player* player) { player_ = player; }

	/// @brief 目標位置を設定する
	/// @param targetPosition 
	void SetTargetPosition(const Vector3& targetPosition) { targetPosition_ = targetPosition; }


private:

	/// @brief プレイヤー
	Player* player_ = nullptr;

	/// @brief スプライト
	Sprite* sprite_ = nullptr;


private:

	/// @brief 補間係数
	float t_ = 0.0f;

	/// @brief タイマー
	float timer_ = 0.0f;

	/// @brief 最大時間
	static constexpr float kMaxTime = 0.3f;


	/// @brief 目標位置
	std::optional<Vector3> targetPosition_ = std::nullopt;
};

