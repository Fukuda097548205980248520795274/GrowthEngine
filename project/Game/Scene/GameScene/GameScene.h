#pragma once
#pragma once
#include "GrowthEngine.h"

#include "Entity/Character/Player/Player.h"
#include "Entity/Character/NPC/NPC.h"
#include "PivotPoint/PivotPoint.h"
#include "MotionManager/MotionManager.h"

class GameScene : public Scene
{
public:

	/// @brief コンストラクタ
	/// @param sceneManager 
	GameScene(SceneManager* sceneManager) : Scene(sceneManager) {}

	/// @brief 初期化
	void Initialize() override;

	/// @brief 更新処理
	void Update() override;

	/// @brief 描画処理
	void Draw() override;


private:

	/// @brief カメラ制御の初期化
	void InitializeCameraControl();

	/// @brief カメラ制御の更新
	/// @param deltaTime
	void UpdateCameraControl(float deltaTime);

	/// @brief ピボット中心をプレイヤーへ追従させる
	/// @param deltaTime
	void UpdatePivotFollow(float deltaTime);

	/// @brief ピボット回転入力を反映する
	/// @param deltaTime
	void UpdatePivotRotateInput(float deltaTime);

	/// @brief ピボットからカメラ姿勢へ反映する
	void ApplyCameraFromPivot();


private:

	// モーションマネージャ
	MotionManager* motionManager_ = nullptr;


	/// @brief プレイヤーの当たり判定グループ
	std::unique_ptr<Collision3DBaseSphere> playerHurtboxGroup_;

	/// @brief プレイヤーの攻撃判定グループ
	std::unique_ptr<Collision3DBaseSphere> playerHitboxGroup_;

	/// @brief 敵の当たり判定グループ
	std::unique_ptr<Collision3DBaseSphere> enemyHurtboxGroup_;

	/// @brief 敵の攻撃判定グループ
	std::unique_ptr<Collision3DBaseSphere> enemyHitboxGroup_;



	/// @brief プレイヤーのモデル
	std::unique_ptr<Render3DSkinningModel> playerModel_ = nullptr;

	/// @brief プレイヤー
	std::unique_ptr<Player> player_ = nullptr;


	/// @brief 味方のモデル
	std::unique_ptr<Render3DSkinningModel> allyModel_ = nullptr;

	/// @brief 味方
	std::unique_ptr<NPC> ally_ = nullptr;


	/// @brief 敵のモデル
	std::unique_ptr<Render3DSkinningModel> enemyModel_ = nullptr;

	/// @brief 敵
	std::unique_ptr<NPC> enemy_;



	/// @brief カメラのピボットポイント
	std::unique_ptr<PivotPoint> pivotPoint_ = nullptr;

	/// @brief カメラ回転入力
	std::unique_ptr<InputGamepadRightStick> inputCameraRotate_ = nullptr;



	/// @brief 太陽光
	std::unique_ptr<LightDirectional> sunLight_ = nullptr;
};

