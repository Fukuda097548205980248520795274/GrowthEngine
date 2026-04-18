#include "GameScene.h"

#include <algorithm>
#include <cmath>
#include <numbers>

namespace
{
	// ピボット中心の追従補間速度
	constexpr float kPivotFollowSpeed = 10.0f;

	// ピボット回転速度[rad/s]
	constexpr float kPivotRotateSpeed = 1.5f;

	// ピボットX軸回転の最大角度
	constexpr float kPivotMaxPitch = 70.0f * (std::numbers::pi_v<float> / 180.0f);

	// ピボット中心の高さオフセット
	const Vector3 kPivotCenterOffset = Vector3(0.0f, 1.5f, 0.0f);
}

/// @brief 初期化
void GameScene::Initialize()
{
	engine_->LoadCamera3D("MainCamera");

	engine_->LoadSound("TEST_audio", engine_->LoadAudio("./Assets/Sounds/bgm/season_goes_around.mp3"));

	// プレイヤーのモデルの生成と初期化
	playerModel_ = std::make_unique<Render3DSkinningModel>(engine_->LoadModel("./Assets/Models/bone", "bone.gltf"),
		engine_->LoadAnimation("./Assets/Models/bone", "bone.gltf"), engine_->LoadSkeleton("./Assets/Models/bone", "bone.gltf"), "Player_Model");

	// 敵のモデルの生成と初期化
	enemyModel_ = std::make_unique<Render3DSkinningModel>(engine_->LoadModel("./Assets/Models/bone", "bone.gltf"),
		engine_->LoadAnimation("./Assets/Models/bone", "bone.gltf"), engine_->LoadSkeleton("./Assets/Models/bone", "bone.gltf"), "Enemy_Model");

	// プレイヤーの生成と初期化
	Character::InitData playerInitData;
	playerInitData.position = Vector3(0.0f, 0.0f, 0.0f);
	playerInitData.hp = 100;
	playerInitData.model_ = playerModel_.get();
	player_ = std::make_unique<Player>(playerInitData);
	player_->Initialize();

	// 敵の生成と初期化
	Character::InitData enemyInitData;
	enemyInitData.position = Vector3(5.0f, 0.0f, 0.0f);
	enemyInitData.hp = 100;
	enemyInitData.model_ = enemyModel_.get();
	enemy_ = std::make_unique<NPC>(enemyInitData, Character::CharacterTag::EnemySide);
	enemy_->Initialize();

	// カメラ制御の初期化
	InitializeCameraControl();
}

/// @brief 更新処理
void GameScene::Update()
{
	// デルタタイムを取得する
	const float deltaTime = engine_->GetDeltaTime();

	// プレイヤーの更新
	player_->Update();

	// 敵の更新
	enemy_->Update();

	// カメラ制御の更新
	UpdateCameraControl(deltaTime);
}

/// @brief カメラ制御の初期化
void GameScene::InitializeCameraControl()
{
	// カメラ用のピボットポイントを生成する
	pivotPoint_ = std::make_unique<PivotPoint>();
	pivotPoint_->GetData()->center = player_->GetPosition();
	pivotPoint_->GetData()->radius = 10.0f;
	pivotPoint_->GetData()->phi = -std::numbers::pi_v<float> / 2.0f;

	// カメラ回転入力の生成
	inputCameraRotate_ = std::make_unique<InputGamepadRightStick>("Camera_Rotate", InputState::Press, 0, Vector2(0.0f, 0.0f), 0.5f);
}

/// @brief カメラ制御の更新
/// @param deltaTime
void GameScene::UpdateCameraControl(float deltaTime)
{
	// ピボットポイントがない場合は更新しない
	if (!pivotPoint_)
		return;

	// ピボット中心の追従更新
	UpdatePivotFollow(deltaTime);

	// ピボット回転入力の更新
	UpdatePivotRotateInput(deltaTime);

	// ピボットの球面座標と注視方向を更新する
	pivotPoint_->Update();

	// ピボットからカメラ姿勢を更新する
	ApplyCameraFromPivot();
}

/// @brief ピボット中心をプレイヤーへ追従させる
/// @param deltaTime
void GameScene::UpdatePivotFollow(float deltaTime)
{
	// データを取得する
	PivotPoint::Data* pivotData = pivotPoint_->GetData();

	// ピボット中心を補間しながらプレイヤー位置へ追従させる
	const float followT = 1.0f - std::exp(-kPivotFollowSpeed * deltaTime);
	pivotData->center = Lerp(pivotData->center, player_->GetPosition() + kPivotCenterOffset, followT);
}

/// @brief ピボット回転入力を反映する
/// @param deltaTime
void GameScene::UpdatePivotRotateInput(float deltaTime)
{
	PivotPoint::Data* pivotData = pivotPoint_->GetData();

	// キー入力でピボットを回転させる
	if (engine_->GetKeyPress(DIK_LEFT))
	{
		pivotData->phi += kPivotRotateSpeed * deltaTime;
	}
	if (engine_->GetKeyPress(DIK_RIGHT))
	{
		pivotData->phi -= kPivotRotateSpeed * deltaTime;
	}
	if (engine_->GetKeyPress(DIK_DOWN))
	{
		pivotData->theta += kPivotRotateSpeed * deltaTime;
	}
	if (engine_->GetKeyPress(DIK_UP))
	{
		pivotData->theta -= kPivotRotateSpeed * deltaTime;
	}

	// キーでカメラ回転している間はゲームパッド回転を加算しない
	const bool isKeyCameraRotate =
		engine_->GetKeyPress(DIK_LEFT) || engine_->GetKeyPress(DIK_RIGHT) ||
		engine_->GetKeyPress(DIK_DOWN) || engine_->GetKeyPress(DIK_UP);

	// 右スティックでピボットを回転させる
	if (!isKeyCameraRotate && inputCameraRotate_ && inputCameraRotate_->param_)
	{
		const Vector2 rightStick = engine_->GetGamepadRightStick(inputCameraRotate_->param_->controller);
		pivotData->phi += -rightStick.x * kPivotRotateSpeed * deltaTime;
		pivotData->theta += -rightStick.y * kPivotRotateSpeed * deltaTime;
	}

	// ピボットのX軸回転は70度以内に制限する
	pivotData->theta = std::clamp(pivotData->theta, -kPivotMaxPitch, kPivotMaxPitch);
}

/// @brief ピボットからカメラ姿勢へ反映する
void GameScene::ApplyCameraFromPivot()
{
	PivotPoint::Data* pivotData = pivotPoint_->GetData();

	// カメラ位置と回転をピボット情報から設定する
	if (Engine::Camera3DData::Param* cameraParam = engine_->GetCamera3DParam("MainCamera"))
	{
		cameraParam->transform.translate = pivotData->sphericalCoordinates;

		// center方向を向くようにオイラー角を計算する
		const Vector3 lookDirection = pivotData->toCenter;
		const float yaw = std::atan2(lookDirection.x, lookDirection.z);
		const float horizontal = std::sqrt(lookDirection.x * lookDirection.x + lookDirection.z * lookDirection.z);
		const float pitch = std::atan2(-lookDirection.y, horizontal);
		cameraParam->transform.rotate = Vector3(pitch, yaw, 0.0f);
	}
}

/// @brief 描画処理
void GameScene::Draw()
{
	// プレイヤーの描画
	player_->Draw();

	// 敵の描画
	enemy_->Draw();
}