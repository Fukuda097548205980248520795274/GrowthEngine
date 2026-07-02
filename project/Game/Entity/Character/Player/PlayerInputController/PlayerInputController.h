#pragma once
#include "GrowthEngine.h"

class PlayerInputController
{
public:

	/// @brief コンストラクタ
	PlayerInputController();

	/// @brief デストラクタ
	~PlayerInputController() = default;


	/// @brief 初期化
	void Initialize();


	/// @brief ダッシュ入力があったかどうかを取得する
	/// @return 
	bool IsDashRequested() const;

	/// @brief 回避入力があったかどうかを取得する
	/// @return 
	bool IsAvoidRequested() const;

	/// @brief 弱攻撃入力があったかどうかを取得する
	/// @return 
	bool IsLightAttackRequested() const;

	/// @brief 強攻撃入力があったかどうかを取得する
	/// @return 
	bool IsHeavyAttackRequested() const;

	/// @brief 掴み入力があったかどうかを取得する
	/// @return 
	bool IsGrabRequested() const;

	/// @brief 防御入力があったかどうかを取得する
	/// @return 
	bool IsGuardRequested() const;

	/// @brief 掴まれ解き入力があったかどうかを取得する
	/// @return 
	bool IsEscapeMashRequested() const;

	/// @brief スタイルチェンジ入力があったかどうかを取得する
	/// @return 
	bool IsStyleChangeRequested() const;

	/// @brief レイジモード入力があったかどうかを取得する
	/// @return 
	bool IsRageModeRequested() const;

	/// @brief 移動入力方向を取得する
	/// @param outHasMoveInput 
	/// @return 
	Vector2 GetMoveDirection(bool& outHasMoveInput) const;


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

	/// @brief 防御入力
	std::unique_ptr<InputGamepadButton> inputGuard_ = nullptr;

	/// @brief 掴まれ解き入力
	std::unique_ptr<InputGamepadButton> inputEscapeMash_ = nullptr;

	/// @brief スタイルチェンジ入力
	std::unique_ptr<InputGamepadButton> inputStyleChange_ = nullptr;

	/// @brief レイジモード入力
	std::unique_ptr<InputGamepadRightTrigger> inputRageMode_ = nullptr;


	/// @brief 前方移動入力
	std::unique_ptr<InputKey> keyFrontMove_ = nullptr;

	/// @brief 後方移動入力
	std::unique_ptr<InputKey> keyBackMove_ = nullptr;

	/// @brief 左移動入力
	std::unique_ptr<InputKey> keyLeftMove_ = nullptr;

	/// @brief 右移動入力
	std::unique_ptr<InputKey> keyRightMove_ = nullptr;

	/// @brief キーのカメラ左回転入力
	std::unique_ptr<InputKey> keyCameraRotateLeft_ = nullptr;

	/// @brief キーのカメラ右回転入力
	std::unique_ptr<InputKey> keyCameraRotateRight_ = nullptr;
};

