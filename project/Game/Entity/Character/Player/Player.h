#pragma once
#include "../Character.h"

class Player : public Character
{
public:

	/// @brief コンストラクタ
	/// @param initData 
	Player(const InitData& initData);

	/// @brief 初期化
	void Initialize();

	/// @brief 更新処理
	virtual void Update() override;

	/// @brief 描画処理
	void Draw();


private:

	/// @brief 移動入力
	std::unique_ptr<InputGamepadLeftStick> inputMove_;

	/// @brief 回避入力
	std::unique_ptr<InputGamepadButton> inputAvoid_ = nullptr;

	/// @brief 弱攻撃入力
	std::unique_ptr<InputGamepadButton> inputLightAttack_ = nullptr;

	/// @brief 強攻撃入力
	std::unique_ptr<InputGamepadButton> inputHeavyAttack_ = nullptr;
};

