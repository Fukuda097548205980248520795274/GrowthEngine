#pragma once
#include "../Character.h"
#include "PlayerInputController/PlayerInputController.h"
#include "ComboTree/ComboTree.h"

class ComboTreeEditor;
class Weapon;
class Gage;

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
	void Initialize(const CharacterInitData& initData, Weapon* baton);

	/// @brief 更新処理
	virtual void Update() override;

	/// @brief 描画処理
	void Draw();

	/// @brief 死亡処理
	void Dead() override;

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

	/// @brief コンボツリーの変更をリクエストする
	/// @param combTreeX 
	/// @param comboTreeY 
	/// @param comboTreeB 
	void RequestComboTreeChange(ComboTree* comboTreeX, ComboTree* comboTreeY, ComboTree* comboTreeB);

	/// @brief コンボツリーを初期化する
	/// @param comboTreeConfig 
	void InitComboTree(const ComboTreeConfig& comboTreeConfig, ComboTreeEditor* comboTreeEditor);

	/// @brief 現在のコンボツリーを取得する
	/// @return 
	ComboTree* GetCurrentComboTreeX() const { return currentComboTreeX_; }

	/// @brief 現在のコンボツリーを取得する
	/// @return 
	ComboTree* GetCurrentComboTreeY() const { return currentComboTreeY_; }

	/// @brief 現在のコンボツリーを取得する
	/// @return 
	ComboTree* GetCurrentComboTreeB() const { return currentComboTreeB_; }

	/// @brief 次のコンボツリーを取得する
	/// @return 
	ComboTree* GetNextComboTreeX() const { return nextComboTreeX_; }

	/// @brief 次のコンボツリーを取得する
	/// @return 
	ComboTree* GetNextComboTreeY() const { return nextComboTreeY_; }

	/// @brief 次のコンボツリーを取得する
	/// @return 
	ComboTree* GetNextComboTreeB() const { return nextComboTreeB_; }

	/// @brief 現在のコンボツリーを設定する
	/// @param comboTree 
	void SetCurrentComboTreeX(ComboTree* comboTree) { currentComboTreeX_ = comboTree; nextComboTreeX_ = nullptr; }

	/// @brief 現在のコンボツリーを設定する
	/// @param comboTree 
	void SetCurrentComboTreeY(ComboTree* comboTree) { currentComboTreeY_ = comboTree; nextComboTreeY_ = nullptr; }

	/// @brief 現在のコンボツリーを設定する
	/// @param comboTree 
	void SetCurrentComboTreeB(ComboTree* comboTree) { currentComboTreeB_ = comboTree; nextComboTreeB_ = nullptr; }

	/// @brief コンボ中かどうかを取得する
	/// @return 
	bool IsCombo() const { return isCombo_; }

	/// @brief レイジゲージHUDを設定する
	/// @param rageGageHud 
	void SetRageGageHud(Gage* rageGageHud);

	/// @brief 武器の耐久力ゲージHUDを設定する
	/// @param weaponHpGageHud 
	void SetWeaponHpGageHud(Gage* weaponHpGageHud, Sprite* weaponKnife, Sprite* weaponGun);


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

	/// @brief ロックオンしているターゲットを検索する
	void SearchLockOnTarget() override;

	/// @brief 最も近い武器を検索する
	/// @return 
	Weapon* FindClosestWeapon();


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

	/// @brief コンボ中かどうか
	bool isCombo_ = false;


private:

	/// @brief レイジゲージを更新する
	void RageGageUpdate();

	/// @brief レイジゲージHUD
	Gage* rageGageHud_ = nullptr;

	/// @brief レイジゲージの通常カラー
	Vector3 rageNormalColor_ = Vector3(0.25f, 0.25f, 1.0f);

	/// @brief レイジゲージのレイジモードの暗い色
	Vector3 rageModeDarkColor_ = Vector3(0.3f, 0.3f, 1.0f);

	/// @brief レイジゲージのレイジモードの明るい色
	Vector3 rageModelLightColor_ = Vector3(0.8f, 0.8f, 1.0f);

	/// @brief レイジゲージカラー用タイマー
	float rageGageColorTimer_ = 0.0f;

	/// @brief レイジゲーカラー用タイマー加算パラメータ
	float rageGageAddParam_ = 1.0f;

	/// @brief レイジゲージカラー用の時間
	static constexpr float kRageGageColorTime = 0.5f;


private:


	/// @brief 武器の耐久力ゲージを更新する
	void WeaponHpGageUpdate();

	/// @brief 武器の耐久力ゲージHUD
	Gage* weaponHpGageHud_ = nullptr;

	/// @brief ナイフのスプライト
	Sprite* weaponKnife_ = nullptr;

	/// @brief 銃のスプライト
	Sprite* weaponGun_ = nullptr;

	/// @brief 武器の耐久力ゲージの不透明度
	float weaponHpGageAlphaTimer_ = 0.0f;

	/// @brief 武器の耐久力ゲージの不透明度が変化する時間
	static constexpr float kWeaponHpGageAlphaTime = 0.5f;


private:

	// 現在のコンボタイプ
	AttackInputType activeComboType_ = AttackInputType::None;

	/// @brief コンボツリーを変更するかどうかのフラグ
	bool isChangeComboTree_ = false;

	// 今のコンボツリー
	ComboTree* currentComboTreeX_ = nullptr;
	ComboTree* currentComboTreeY_ = nullptr;
	ComboTree* currentComboTreeB_ = nullptr;

	// 次のコンボツリー
	ComboTree* nextComboTreeX_ = nullptr;
	ComboTree* nextComboTreeY_ = nullptr;
	ComboTree* nextComboTreeB_ = nullptr;


private:

	/// @brief 防御状態を更新する
	void UpdateGuardState();
};

