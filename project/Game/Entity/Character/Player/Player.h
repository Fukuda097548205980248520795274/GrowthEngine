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

	/// @brief ダッシュ入力
	std::unique_ptr<InputGamepadButton> inputDash_ = nullptr;

	/// @brief 回避入力
	std::unique_ptr<InputGamepadButton> inputAvoid_ = nullptr;

	/// @brief 弱攻撃入力
	std::unique_ptr<InputGamepadButton> inputLightAttack_ = nullptr;

	/// @brief 強攻撃入力
	std::unique_ptr<InputGamepadButton> inputHeavyAttack_ = nullptr;

	/// @brief 構え入力
	std::unique_ptr<InputGamepadButton> inputStance_ = nullptr;


	/// @brief 前方移動入力
	std::unique_ptr<InputKey> keyFrontMove_ = nullptr;

	/// @brief 後方移動入力
	std::unique_ptr<InputKey> keyBackMove_ = nullptr;

	/// @brief 左移動入力
	std::unique_ptr<InputKey> keyLeftMove_ = nullptr;

	/// @brief 右移動入力
	std::unique_ptr<InputKey> keyRightMove_ = nullptr;

	/// @brief キーの構え入力
	std::unique_ptr<InputKey> keyStance_ = nullptr;

	/// @brief キーのカメラ左回転入力
	std::unique_ptr<InputKey> keyCameraRotateLeft_ = nullptr;

	/// @brief キーのカメラ右回転入力
	std::unique_ptr<InputKey> keyCameraRotateRight_ = nullptr;
};

