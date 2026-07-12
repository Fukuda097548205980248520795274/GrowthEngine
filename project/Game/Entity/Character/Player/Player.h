#pragma once
#include "../Character.h"
#include "PlayerInputController/PlayerInputController.h"
#include "ComboTree/ComboTree.h"

class Player : public Character
{
public:

	/// @brief コンストラクタ
	/// @param initData 
	Player();

	/// @brief 初期化
	/// @param initData 
	/// @param comboTree 
	/// @param baton 
	void Initialize(const CharacterInitData& initData, Weapon* baton,
		std::unique_ptr<ComboTree> comboTreeX, std::unique_ptr<ComboTree> comboTreeY, std::unique_ptr<ComboTree> comboTreeB);

	/// @brief 更新処理
	virtual void Update() override;

	/// @brief 描画処理
	void Draw();

	/// @brief カメラを操作しているかどうかを設定する
	/// @param isOperationCamera 
	void SetIsOperationCamera(bool isOperationCamera) { isOperationCamera_ = isOperationCamera; }

	/// @brief 弱攻撃入力があったかどうかを取得する
	/// @return 
	bool IsInputLightAttack() const { return isInputLightAttack_; }

	/// @brief レイジモード入力があったかどうかを取得する
	/// @return 
	bool IsInputRageMode() const { return isInputRageMode_ || isPrevInputRageMode_; }

	/// @brief 入力コントローラーを取得する
	/// @return 
	PlayerInputController* GetInputController() const { return inputController_.get(); }

	/// @brief カメラのY回転を取得する
	/// @return
	float GetCameraYaw() const;

private:

	/// @brief 更新処理開始前のリセット
	void StartUpdate() override;

	/// @brief 攻撃処理を更新する
	void UpdateAttack();

	/// @brief 構え状態を更新する
	void UpdateStanceState();

	/// @brief ダッシュ状態を更新する
	/// @param hasMoveInput
	void UpdateDashState(bool hasMoveInput);

	/// @brief 現在の移動速度を取得する
	/// @return
	float GetCurrentMoveSpeed() const;

	/// @brief スタイルチェンジ開始時の処理
	void StyleChangeStart() override;

	/// @brief スタイルが変化したときの処理
	/// @param newStyle 
	void OnStyleChanged(FightStyle newStyle) override;

	/// @brief スタイルチェンジ処理
	void StyleChange();

	/// @brief カメラによるターゲットの更新
	void UpdateTargetByCamera();

	/// @brief レイジモード入力処理
	void RageModeInput() override;


private:

	/// @brief 入力コントローラー
	std::unique_ptr<PlayerInputController> inputController_ = nullptr;

	/// @brief 警棒
	Weapon* baton_ = nullptr;


private:

	// 攻撃入力のバッファ時間
	float attackInputBufferTime_ = 0.2f;


	/// @brief 前フレームのカメラのY回転
	float prevCameraYaw_ = 0.0f;

	/// @brief カメラを操作しているかどうか
	bool isOperationCamera_ = false;


	// 攻撃入力状態
	bool isInputLightAttack_ = false;

	// 前フレームの攻撃入力状態
	bool isPrevInputLightAttack_ = false;


	/// @brief レイジモード入力状態
	bool isInputRageMode_ = false;

	/// @brief 前フレームのレイジモード入力状態
	bool isPrevInputRageMode_ = false;


private:

	// 現在のコンボタイプ
	AttackInputType activeComboType_ = AttackInputType::None;

	// コンボツリー
	std::unique_ptr<ComboTree> comboTreeX_ = nullptr;
	std::unique_ptr<ComboTree> comboTreeY_ = nullptr;
	std::unique_ptr<ComboTree> comboTreeB_ = nullptr;


private:

	/// @brief 防御状態を更新する
	void UpdateGuardState();
};

