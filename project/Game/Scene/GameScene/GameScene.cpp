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

	// 太陽光の生成と初期化
	sunLight_ = std::make_unique<LightDirectional>("SunLight");

	// マネージャの生成と初期化
	motionManager_ = MotionManager::GetInstance();
	soundManager_ = SoundManager::GetInstance();
	effectManager_ = EffectManager::GetInstance();

	engine_->LoadPostEffect("TAA", Engine::PostEffect::Type::TAA);

	// ポストエフェクトマネージャの生成と初期化
	postEffectManager_ = std::make_unique<PostEffectManager>();
	postEffectManager_->Initialize();

	// モーションマネージャのエディタの生成と初期化
	motionManagerEditor_ = std::make_unique<MotionManagerEditor>();

	// ビヘイビアツリーエディタの生成と初期化
	behaviorTreeEditor_ = std::make_unique<BehaviorTreeEditor>();


	// プレイヤーのモデルの生成と初期化
	playerModel_ = std::make_unique<Render3DSkinningModel>(engine_->LoadModel("./Assets/Models/Character", "bone.gltf"),
		motionManager_->GetMotion(MotionType::Stand, "Standing"), motionManager_->GetSkeleton(), "Player_Model");


	
	// プレイヤー側の当たり判定グループの生成と初期化
	playerHurtboxGroup_ = std::make_unique<Collision3DBaseSphere>("PlayerSide_Hurtbox");
	playerHitboxGroup_ = std::make_unique<Collision3DBaseSphere>("PlayerSide_Hitbox");

	// 敵側の当たり判定グループの生成と初期化
	enemyHurtboxGroup_ = std::make_unique<Collision3DBaseSphere>("EnemySide_Hurtbox");
	enemyHitboxGroup_ = std::make_unique<Collision3DBaseSphere>("EnemySide_Hitbox");

	// 着地の当たり判定グループの生成と初期化
	landingCollision_ = std::make_unique<Collision3DBaseAABB>("Landing_Collision");
	floorCollision_ = std::make_unique<Collision3DBaseAABB>("Floor_Collision");



	// 「プレイヤーの攻撃」は「敵の体」に当たる
	enemyHurtboxGroup_->SetCollisionTarget(playerHitboxGroup_->GetHandle());

	// 「敵の攻撃」は「プレイヤーの体」に当たる
	playerHurtboxGroup_->SetCollisionTarget(enemyHitboxGroup_->GetHandle());

	// 「床」に当たる
	landingCollision_->SetCollisionTarget(floorCollision_->GetHandle());


	// プレイヤーの武器のモデルの生成と初期化
	playerWeaponModel_ = std::make_unique<Render3DStaticModel>(engine_->LoadModel("./Assets/Models/weapon/PoliceBaton", "PoliceBaton.obj"), "Player_Weapon_Model");

	// プレイヤーの武器の生成と初期化
	Weapon::InitData playerWeaponInitData;
	playerWeaponInitData.position = Vector3(0.0f, 0.0f, 0.0f);
	playerWeaponInitData.model = playerWeaponModel_.get();
	playerWeaponInitData.durability = 0;
	playerWeaponInitData.attackPower = 1.0f;
	playerWeaponInitData.category = WeaponCategory::OneHanded;
	playerWeaponInitData.isUnbreakable = true;
	playerWeaponInitData.landingCollision = landingCollision_->CreateInstance();
	playerWeapon_ = std::make_unique<Weapon>(playerWeaponInitData);

	// プレイヤーの生成と初期化
	Character::InitData playerInitData;
	playerInitData.position = Vector3(0.0f, 0.0f, 0.0f);
	playerInitData.hp = 100;
	playerInitData.model_ = playerModel_.get();
	playerInitData.weapon = nullptr;
	playerInitData.avoidDuration = 0.3f;
	playerInitData.avoidDistance = 1.5f;
	playerInitData.hStandMotion = motionManager_->GetMotion(MotionType::Stand, "Standing");
	playerInitData.hStanceMotion = motionManager_->GetMotion(MotionType::Stance, "Nimble");
	playerInitData.hWalkMotion = motionManager_->GetMotion(MotionType::Walk, "Walk");
	playerInitData.hDashMotion = motionManager_->GetMotion(MotionType::Dash, "Dash");
	playerInitData.hAvoidFrontMotion = motionManager_->GetMotion(MotionType::Avoid, "Front");
	playerInitData.hAvoidBackMotion = motionManager_->GetMotion(MotionType::Avoid, "Back");
	playerInitData.hGuardMotion = motionManager_->GetMotion(MotionType::Guard, "BothHands");
	playerInitData.hGuardHitMotion = motionManager_->GetMotion(MotionType::Guard, "OneLeg");
	playerInitData.hAvoidLeftMotion = 0;
	playerInitData.hAvoidRightMotion = 0;
	playerInitData.hurtboxGroup = playerHurtboxGroup_.get();
	playerInitData.hitboxGroup = playerHitboxGroup_.get();
	playerInitData.landingCollision = landingCollision_->CreateInstance();
	player_ = std::make_unique<Player>(playerInitData);
	player_->Initialize(playerWeapon_.get());

	//// 味方の生成と初期化
	//Character::InitData allyInitData;
	//allyInitData.position = Vector3(-5.0f, 0.0f, 0.0f);
	//allyInitData.hp = 100;
	//allyInitData.avoidDuration = 0.3f;
	//allyInitData.avoidDistance = 1.5f;
	//allyInitData.model_ = allyModel_.get();
	//allyInitData.hStandMotion = motionManager_->GetMotion(MotionType::Stand, 0);
	//allyInitData.hStanceMotion = motionManager_->GetMotion(MotionType::Stance, 2);
	//allyInitData.hWalkMotion = motionManager_->GetMotion(MotionType::Walk, 0);
	//allyInitData.hDashMotion = motionManager_->GetMotion(MotionType::Dash, 0);
	//allyInitData.hAvoidFrontMotion = motionManager_->GetMotion(MotionType::AvoidFont, 0);
	//allyInitData.hAvoidBackMotion = motionManager_->GetMotion(MotionType::AvoidBack, 0);
	//allyInitData.hAvoidLeftMotion = 0;
	//allyInitData.hAvoidRightMotion = 0;
	//allyInitData.hurtboxGroup = playerHurtboxGroup_.get();
	//allyInitData.hitboxGroup = playerHitboxGroup_.get();
	//ally_ = std::make_unique<NPC>(allyInitData, Character::CharacterTag::PlayerSide);
	//ally_->Initialize();

	// 味方の生成と初期化
	for (int i = 0; i < 4; ++i)
	{
		std::unique_ptr<Render3DSkinningModel> allyModel = std::make_unique<Render3DSkinningModel>(engine_->LoadModel("./Assets/Models/Character", "bone.gltf"),
			motionManager_->GetMotion(MotionType::Stand, "Standing"), motionManager_->GetSkeleton(), "Ally_Model_" + std::to_string(i));

		Character::InitData allyInitData;
		allyInitData.position = Vector3(5.0f * (i + 1), 0.0f, 5.0f);
		allyInitData.rotate = Vector3(0.0f, std::numbers::pi_v<float>, 0.0f);
		allyInitData.hp = 100;
		allyInitData.avoidDuration = 0.3f;
		allyInitData.avoidDistance = 1.5f;
		allyInitData.model_ = allyModel.get();
		allyInitData.weapon = nullptr;
		allyInitData.hStandMotion = motionManager_->GetMotion(MotionType::Stand, "Standing");
		allyInitData.hStanceMotion = motionManager_->GetMotion(MotionType::Stance, "Fighter");
		allyInitData.hWalkMotion = motionManager_->GetMotion(MotionType::Walk, "Walk");
		allyInitData.hDashMotion = motionManager_->GetMotion(MotionType::Dash, "Dash");
		allyInitData.hAvoidFrontMotion = motionManager_->GetMotion(MotionType::Avoid, "Front");
		allyInitData.hAvoidBackMotion = motionManager_->GetMotion(MotionType::Avoid, "Back");
		allyInitData.hAvoidLeftMotion = 0;
		allyInitData.hAvoidRightMotion = 0;
		allyInitData.hGuardMotion = motionManager_->GetMotion(MotionType::Guard, "BothHands");
		allyInitData.hGuardHitMotion = motionManager_->GetMotion(MotionType::Guard, "OneLeg");
		allyInitData.hurtboxGroup = playerHurtboxGroup_.get();
		allyInitData.hitboxGroup = playerHitboxGroup_.get();
		allyInitData.landingCollision = landingCollision_->CreateInstance();


		std::unique_ptr<NPC> ally = std::make_unique<NPC>(allyInitData, Character::CharacterTag::PlayerSide);
		ally->Initialize(behaviorTreeEditor_->CreateTree("TEST", ally.get()));

		npcModels_.push_back(std::move(allyModel));
		npcs_.push_back(std::move(ally));
	}

	for (int i = 0; i < 5; ++i)
	{
		// 敵のモデルの生成と初期化
		std::unique_ptr<Render3DSkinningModel> enemyModel = std::make_unique<Render3DSkinningModel>(engine_->LoadModel("./Assets/Models/Character", "bone.gltf"),
			motionManager_->GetMotion(MotionType::Stand, "Standing"), motionManager_->GetSkeleton(), "Enemy_Model_" + std::to_string(i));

		// 敵の生成と初期化
		Character::InitData enemyInitData;
		enemyInitData.position = Vector3(5.0f * (i + 1), 0.0f, 0.0f);
		enemyInitData.rotate = Vector3(0.0f, 0.0f, 0.0f);
		enemyInitData.hp = 100;
		enemyInitData.avoidDuration = 0.3f;
		enemyInitData.avoidDistance = 1.5f;
		enemyInitData.model_ = enemyModel.get();
		enemyInitData.weapon = nullptr;
		enemyInitData.hStandMotion = motionManager_->GetMotion(MotionType::Stand, "Standing");
		enemyInitData.hStanceMotion = motionManager_->GetMotion(MotionType::Stance, "Fighter");
		enemyInitData.hWalkMotion = motionManager_->GetMotion(MotionType::Walk, "Walk");
		enemyInitData.hDashMotion = motionManager_->GetMotion(MotionType::Dash, "Dash");
		enemyInitData.hAvoidFrontMotion = motionManager_->GetMotion(MotionType::Avoid, "Front");
		enemyInitData.hAvoidBackMotion = motionManager_->GetMotion(MotionType::Avoid, "Back");
		enemyInitData.hAvoidLeftMotion = 0;
		enemyInitData.hAvoidRightMotion = 0;
		enemyInitData.hGuardMotion = motionManager_->GetMotion(MotionType::Guard, "BothHands");
		enemyInitData.hGuardHitMotion = motionManager_->GetMotion(MotionType::Guard, "OneLeg");
		enemyInitData.hurtboxGroup = enemyHurtboxGroup_.get();
		enemyInitData.hitboxGroup = enemyHitboxGroup_.get();
		enemyInitData.landingCollision = landingCollision_->CreateInstance();

		std::unique_ptr<NPC> enemy = std::make_unique<NPC>(enemyInitData, Character::CharacterTag::EnemySide);
		enemy->Initialize(behaviorTreeEditor_->CreateTree("TEST", enemy.get()));

		npcModels_.push_back(std::move(enemyModel));
		npcs_.push_back(std::move(enemy));
	}


	// 床
	FloorInitData floorInitData;
	floorInitData.position = Vector3(0.0f, -2.0f, 0.0f);
	floorInitData.scale = Vector3(50.0f, 1.0f, 20.0f);
	floorInitData.collision = floorCollision_->CreateInstance();
	floorInitData.model = nullptr;
	floor_ = std::make_unique<Floor>();
	floor_->Initialize(floorInitData);


	// カメラ制御の初期化
	InitializeCameraControl();
}

/// @brief 更新処理
void GameScene::Update()
{
	// デルタタイムを取得する
	const float deltaTime = engine_->GetDeltaTime() * engine_->GetTimeScale();

	// 床の更新
	floor_->Update();

	// プレイヤーの更新
	player_->Update();
	playerWeapon_->Update();

	// 敵の更新
	for (auto& npc : npcs_)npc->Update();

	// エフェクトの更新
	effectManager_->Update();

	// カメラ制御の更新
	UpdateCameraControl(deltaTime);
}

/// @brief 描画処理
void GameScene::Draw()
{
	// エディタの描画
	behaviorTreeEditor_->DrawProjectWindow();
	behaviorTreeEditor_->DrawNodeTable();
	behaviorTreeEditor_->DrawPropertyWindow();
	motionManagerEditor_->Draw();

	// プレイヤーの描画
	player_->Draw();
	playerWeapon_->Draw();

	//// 味方の描画
	//ally_->Draw();

	// 敵の描画
	for (auto& npc : npcs_)npc->Draw();

	// エフェクトの描画
	effectManager_->Draw();

	// ポストエフェクトの描画処理
	postEffectManager_->Draw(player_.get());

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
	// ピボットのデータを取得する
	PivotPoint::Data* pivotData = pivotPoint_->GetData();

	// ターゲットの位置をプレイヤーの位置に設定する
	Vector3 targetPivotPos = player_->GetPosition();

	// プレイヤーの位置からピボット中心までの高さオフセットを加算する
	if (player_->IsStance())
	{
		targetPivotPos.y += 1.2f;
	}
	else
	{
		// プレイヤーの位置からピボット中心までの高さオフセットを加算する
		targetPivotPos.y += 1.2f;
	}

	// ロックオン中はターゲットの位置にピボットをオフセットする
	if (player_->IsStance() && player_->GetLockOnTarget() != nullptr)
	{
		// ロックオン中はターゲットの位置にピボットをオフセットする
		Vector3 targetPos = player_->GetLockOnTarget()->GetPosition();
		Vector3 playerPos = player_->GetPosition();

		// ターゲットの方向ベクトルを計算する
		Vector3 dir = targetPos - playerPos;
		dir.y = 0.0f;
		dir = dir.Normalize();

		// ターゲットの左右方向ベクトルを計算する
		Vector3 rightDir = Vector3(dir.z, 0.0f, -dir.x);

		// ターゲットの左右どちらかにピボットをオフセットする量
		constexpr float kRightOffset = 0.0f;

		// ターゲットの左右どちらかにピボットをオフセットする
		targetPivotPos += rightDir * kRightOffset;
	}

	// ピボット中心をターゲット位置に向かって補間移動させる
	Vector3 diff = targetPivotPos - pivotData->center;
	pivotData->center += diff * kPivotFollowSpeed * deltaTime;
}

/// @brief ピボット回転入力を反映する
/// @param deltaTime
void GameScene::UpdatePivotRotateInput(float deltaTime)
{
	// ピボットのデータを取得する
	PivotPoint::Data* pivotData = pivotPoint_->GetData();

	// キー入力でカメラ回転しているかを判定する
	const bool isKeyCameraRotate =
		engine_->GetKeyPress(DIK_LEFT) || engine_->GetKeyPress(DIK_RIGHT) ||
		engine_->GetKeyPress(DIK_DOWN) || engine_->GetKeyPress(DIK_UP);

	// ゲームパッドの右スティック入力を取得する
	Vector2 rightStick(0.0f, 0.0f);
	if (inputCameraRotate_ && inputCameraRotate_->param_)
	{
		rightStick = engine_->GetGamepadRightStick(inputCameraRotate_->param_->controller);
	}

	// キー入力または右スティック入力がある場合は手動でカメラ回転しているとみなす
	bool isManualCameraControl = isKeyCameraRotate || (rightStick.Length() > 0.01f);
	
	// 手動でカメラ回転入力がある場合はピボットを回転させる
	if (isManualCameraControl)
	{
		// 手動でカメラ回転入力がある場合はピボットを回転させる
		if (!isKeyCameraRotate)
		{
			pivotData->phi += -rightStick.x * kPivotRotateSpeed * deltaTime;
			pivotData->theta += -rightStick.y * kPivotRotateSpeed * deltaTime;
		}
	}
	else if (player_->IsStance() && player_->GetLockOnTarget() != nullptr)
	{
		// ロックオン中はターゲットの方向にピボットを回転させる
		Character* target = player_->GetLockOnTarget();
		Vector3 targetPos = target->GetPosition();
		targetPos.y += 1.0f;

		// ターゲットの方向ベクトルを計算する
		Vector3 dir = targetPos - pivotData->center;
		dir.Normalize();

		// ターゲットの方向からピボットの回転角度を計算する
		float targetPhi = std::atan2(-dir.x, dir.z) - (std::numbers::pi_v<float> / 2.0f);
		float targetTheta = std::atan2(-dir.y, std::sqrt(dir.x * dir.x + dir.z * dir.z));

		// ターゲットの高さに合わせてピボットのX軸回転を制限する
		constexpr float kLockOnMaxPitch = 20.0f * (std::numbers::pi_v<float> / 180.0f);
		targetTheta = std::clamp(targetTheta, -kLockOnMaxPitch, kLockOnMaxPitch);

		// ピボットのY軸回転とターゲットの方向の差を計算する
		float diffPhi = targetPhi - pivotData->phi;

		// 角度の差を-π～πの範囲に収める
		while (diffPhi > std::numbers::pi_v<float>)  diffPhi -= 2.0f * std::numbers::pi_v<float>;
		while (diffPhi < -std::numbers::pi_v<float>) diffPhi += 2.0f * std::numbers::pi_v<float>;

		// ピボットのY軸回転はターゲットの方向に合わせて補間する
		constexpr float kLockOnCameraFollowSpeed = 3.0f;
		pivotData->phi += diffPhi * kLockOnCameraFollowSpeed * deltaTime;

		// ピボットのX軸回転はターゲットの高さに合わせて補間する
		float diffTheta = targetTheta - pivotData->theta;
		pivotData->theta += diffTheta * kLockOnCameraFollowSpeed * deltaTime;
	}

	// ピボットのX軸回転は最大角度で制限する
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