#pragma once
#include "../Character.h"
#include "PlayerInputController/PlayerInputController.h"

class Player : public Character
{
public:

	/// @brief コンストラクタ
	/// @param initData 
	Player();

	/// @brief 初期化
	void Initialize(const InitData& initData, Weapon* baton);

	/// @brief 更新処理
	virtual void Update() override;

	/// @brief 描画処理
	void Draw();

	/// @brief カメラを操作しているかどうかを設定する
	/// @param isOperationCamera 
	void SetIsOperationCamera(bool isOperationCamera) { isOperationCamera_ = isOperationCamera; }

	/// @brief ターゲット方向を向く
	void TargetDirection() override;

	/// @brief 弱攻撃入力があったかどうかを取得する
	/// @return 
	bool IsInputLightAttack() const { return isInputLightAttack_; }

private:

	/// @brief 更新処理開始前のリセット
	void StartUpdate() override;

	/// @brief 攻撃処理を更新する
	void UpdateAttack();

	/// @brief 構え状態を更新する
	void UpdateStanceState();

	/// @brief 連続回避を試行する
	/// @param moveInputDirection
	/// @param hasMoveInput
	/// @param cameraYaw
	void ReserveNextAvoid(const Vector2& moveInputDirection, bool hasMoveInput, float cameraYaw);

	/// @brief ダッシュ状態を更新する
	/// @param hasMoveInput
	void UpdateDashState(bool hasMoveInput);

	/// @brief 現在の移動速度を取得する
	/// @return
	float GetCurrentMoveSpeed() const;

	/// @brief カメラのY回転を取得する
	/// @return
	float GetCameraYaw() const;

	/// @brief ダウン後起き上がり条件を満たしているかどうか
	/// @return 
	bool CheckGetUpCondition()override;

	/// @brief スタイルチェンジ開始時の処理
	void StyleChangeStart() override;

	/// @brief スタイルが変化したときの処理
	/// @param newStyle 
	void OnStyleChanged(FightStyle newStyle) override;

	/// @brief スタイルチェンジ処理
	void StyleChange();

	/// @brief カメラによるターゲットの更新
	void UpdateTargetByCamera();


private:

	/// @brief 入力コントローラー
	std::unique_ptr<PlayerInputController> inputController_ = nullptr;

	/// @brief 警棒
	Weapon* baton_ = nullptr;


private:

	// 攻撃入力のバッファ時間
	float attackInputBufferTime_ = 0.2f;


	/// @brief コンボアタック
	std::vector<std::unique_ptr<ComboAttack>> comboAttacks_;

	/// @brief つかみ攻撃
	std::unique_ptr<GrabAttack> grabAttack_ = nullptr;

	/// @brief 前フレームのカメラのY回転
	float prevCameraYaw_ = 0.0f;

	/// @brief カメラを操作しているかどうか
	bool isOperationCamera_ = false;


	// 攻撃入力状態
	bool isInputLightAttack_ = false;

	// 前フレームの攻撃入力状態
	bool isPrevInputLightAttack_ = false;


private:

	/// @brief 防御状態を更新する
	void UpdateGuardState();
};

