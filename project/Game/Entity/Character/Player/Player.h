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

	/// @brief 攻撃処理を更新する
	void UpdateAttack();

	/// @brief 構え状態を更新する
	void UpdateStanceState();

	/// @brief 移動入力方向を取得する
	/// @param hasMoveInput
	/// @return
	Vector2 GetMoveInputDirection(bool& hasMoveInput) const;

	/// @brief ダッシュ状態を更新する
	/// @param hasMoveInput
	void UpdateDashState(bool hasMoveInput);

	/// @brief 現在の移動速度を取得する
	/// @return
	float GetCurrentMoveSpeed() const;

	/// @brief カメラのY回転を取得する
	/// @return
	float GetCameraYaw() const;


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

	/// @brief 掴み入力
	std::unique_ptr<InputGamepadButton> inputGrab_ = nullptr;

	/// @brief 構え入力
	std::unique_ptr<InputGamepadButton> inputStance_ = nullptr;

	/// @brief 防御入力
	std::unique_ptr<InputGamepadButton> inputGuard_ = nullptr;


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


private:

	// 攻撃入力のバッファ時間
	float attackInputBufferTime_ = 0.2f;


	/// @brief コンボアタック
	std::vector<std::unique_ptr<ComboAttack>> comboAttacks_;

	/// @brief つかみ攻撃
	std::unique_ptr<GrabAttack> grabAttack_ = nullptr;


private:

	/// @brief 防御状態を更新する
	void UpdateGuardState();
};

