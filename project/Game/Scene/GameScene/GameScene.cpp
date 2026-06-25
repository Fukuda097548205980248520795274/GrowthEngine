#include "GameScene.h"
#include "BattleDirector/BattleDirector.h"

#include <algorithm>
#include <cmath>
#include <numbers>

#include "HUD/HP/HP.h"

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

	// ポストエフェクトマネージャの生成と初期化
	postEffectManager_ = std::make_unique<PostEffectManager>();
	postEffectManager_->Initialize();

	// カメラシェイクの生成と初期化
	cameraShake_ = std::make_unique<Shake>();

	// モーションマネージャのエディタの生成と初期化
	motionManagerEditor_ = std::make_unique<MotionManagerEditor>();

	// ビヘイビアツリーエディタの生成と初期化
	behaviorTreeEditor_ = std::make_unique<BehaviorTreeEditor>();

	// ビヘイビアツリービューアの生成と初期化
	behaviorTreeViewer_ = std::make_unique<BehaviorTreeViewer>();

	// ナビゲーションメッシュの生成と初期化
	navMesh_ = std::make_unique<NavMesh>();

	// ステージエディタの生成と初期化
	stageEditor_ = std::make_unique<StageEditor>(this);
	stageEditor_->Initialize();

	// エディタワークスペースマネージャの生成と初期化
	editorWorkspaceManager_ = std::make_unique<EditorWorkspaceManager>();
	editorWorkspaceManager_->Initialize(stageEditor_.get(), behaviorTreeEditor_.get(),behaviorTreeViewer_.get(), motionManagerEditor_.get());

	// キャラクターモデルの読み込み
	hCharacterModel_ = engine_->LoadModel("./Assets/Models/Character", "bone.gltf");
	hCharacterAnimation_ = motionManager_->GetMotion(MotionType::Stand, "Standing");
	hCharacterSkeleton_ = motionManager_->GetSkeleton();


	// プレイヤーのモデルの生成と初期化
	playerModel_ = std::make_unique<Render3DSkinningModel>(hCharacterModel_, hCharacterAnimation_, hCharacterSkeleton_, "Player_Model");

	// プレイヤーの生成と初期化
	playerTrail_ = std::make_unique<Trail3D>("Player_Trail", 0.15f, engine_->LoadTexture("./Assets/Textures/trail_000.png"));

	// 片手武器モデルの読み込み
	oneHandedWeaponModel_ = std::make_unique<PrefabBaseStaticModel>(engine_->LoadModel("./Assets/Models/weapon/PoliceBaton", "PoliceBaton.obj"), 100, "PoliceBaton");

	// 試験的な立方体の生成と初期化
	trialCube_ = std::make_unique<PrefabBaseCube>(engine_->LoadTexture("./Assets/Textures/white2x2.png"), 1000, "Trial_Cube");


	// コンマスプライトの生成と初期化
	commaSprite_ = std::make_unique<PrefabBaseSprite>(engine_->LoadTexture("./Assets/Textures/comma.png"), 100, "Comma_Sprite");

	// 数字スプライトの生成と初期化
	numbersSprite_ = std::make_unique<PrefabBaseSprite>(engine_->LoadTexture("./Assets/Textures/numbers.png"), 100, "Numbers_Sprite");

	// 攻撃ボタンのスプライトの生成と初期化
	attackButtonSprite = std::make_unique<Sprite>(engine_->LoadTexture("./Assets/Textures/attack_button.png"), "Attack_Button_Sprite");
	attackButtonSprite->param_->texture.anchor = Vector2(0.0f, 1.0f);


	
	// プレイヤー側の当たり判定グループの生成と初期化
	playerHurtboxGroup_ = std::make_unique<Collision3DBaseSphere>("PlayerSide_Hurtbox");
	playerHitboxGroup_ = std::make_unique<Collision3DBaseSphere>("PlayerSide_Hitbox");

	// 敵側の当たり判定グループの生成と初期化
	enemyHurtboxGroup_ = std::make_unique<Collision3DBaseSphere>("EnemySide_Hurtbox");
	enemyHitboxGroup_ = std::make_unique<Collision3DBaseSphere>("EnemySide_Hitbox");

	// 着地の当たり判定グループの生成と初期化
	landingCollision_ = std::make_unique<Collision3DBaseCapsule>("Landing_Collision");
	floorCollision_ = std::make_unique<Collision3DBaseAABB>("Floor_Collision");

	// 壁の当たり判定グループの生成と初期化
	wallTouchCollision_ = std::make_unique<Collision3DBaseCapsule>("WallTouch_Collision");
	wallCollision_ = std::make_unique<Collision3DBaseOBB>("Wall_Collision");

	// イベントトリガーの当たり判定グループの生成と初期化
	eventTriggerCollision_ = std::make_unique<Collision3DBaseCapsule>("EventTrigger_Collision");
	eventTriggerAABBCollision_ = std::make_unique<Collision3DBaseAABB>("EventTriggerAABB_Collision");



	// 「プレイヤーの攻撃」は「敵の体」に当たる
	enemyHurtboxGroup_->SetCollisionTarget(playerHitboxGroup_->GetHandle());

	// 「敵の攻撃」は「プレイヤーの体」に当たる
	playerHurtboxGroup_->SetCollisionTarget(enemyHitboxGroup_->GetHandle());

	// 「床」に当たる
	landingCollision_->SetCollisionTarget(floorCollision_->GetHandle());

	// 「壁」に当たる
	wallTouchCollision_->SetCollisionTarget(wallCollision_->GetHandle());

	// 「イベントトリガー」に当たる
	eventTriggerAABBCollision_->SetCollisionTarget(eventTriggerCollision_->GetHandle());


	// HUDの読み込み
	LoadHUDs();


	//// ステージを読み込む
	//stageEditor_->LoadStage("Tutorial.json");
}

/// @brief 更新処理
void GameScene::Update()
{
	// デルタタイムを取得する
	const float dt = engine_->GetDeltaTime() * engine_->GetTimeScale();

	// プレイヤーの更新
	if (player_)
	{
		player_->Update();
		playerWeapon_->Update();

		// プレイヤーの体力バーの更新
		if (playerHP_)
		{
			playerHP_->SetPosition(Vector2(400.0f, 650.0f));
			playerHP_->SetCurrentHP(player_->GetHp());
			playerHP_->Update();
		}

		// 太陽光をプレイヤーに追従させる
		sunLight_->param_->position = player_->GetPosition() + Vector3(-5.0f, 10.0f, -5.0f);
	}

	// オブジェクトの更新
	objects_.remove_if([](const std::unique_ptr<StageObject>& object) {object->Update();return object->IsFinished();});

	// NPCの更新
	npcs_.remove_if([](const std::unique_ptr<NPC>& npc) {npc->Update();return npc->IsFinished();});

	// 武器の更新
	weapons_.remove_if([](const std::unique_ptr<Weapon>& weapon) {weapon->Update();return weapon->IsFinished();});

	// HUDの更新
	huds_.remove_if([](const std::unique_ptr<HUD>& hud) {hud->Update(); return hud->IsFinished(); });


	// エフェクトの更新
	effectManager_->Update();

	// ステージエディタの更新
	stageEditor_->Update(dt);

	// カメラ制御の更新
	UpdateCameraControl(dt);

	// プレイヤーの動きによるシェイクの更新
	if (player_)
	{
		// 攻撃を当てた時
		if (player_->IsHitAttack())
			cameraShake_->StartShake(0.2f, 0.05f, Vector3(1.0f, 1.0f, 1.0f));

		// ダメージを受けた時
		if (player_->IsHitDamage())
			cameraShake_->StartShake(0.3f, 0.1f, Vector3(1.0f, 1.0f, 1.0f));

		// 弾いたとき
		if(player_->IsHitRepel())
			cameraShake_->StartShake(0.1f, 0.025f, Vector3(1.0f, 1.0f, 1.0f));
	}

	// カメラシェイクの更新
	cameraShake_->Update(dt);
}

/// @brief 描画処理
void GameScene::Draw()
{
	// エディタの描画
	editorWorkspaceManager_->DrawUI();

	// ステージオブジェクトの描画
	for (auto& object : objects_)object->Draw();
	trialCube_->Draw();

	// プレイヤーの描画
	if (player_)
	{
		player_->Draw();
		playerWeapon_->Draw();
	}

	// 敵の描画
	for (auto& npc : npcs_)npc->Draw();

	// 武器の描画
	for (auto& weapon : weapons_)weapon->Draw();

	// プレハブの描画処理
	oneHandedWeaponModel_->Draw();

	// エフェクトの描画
	effectManager_->Draw();

	// ポストエフェクトの描画処理
	postEffectManager_->Draw(player_.get());

	// HUDの描画
	for (auto& hud : huds_)hud->Draw();

	// プレイヤーの体力バーの描画
	if (playerHP_)playerHP_->Draw();

	// 体力バーの描画
	hpFrameMiddleSprite_->Draw();
	hpFrameRightSprite_->Draw();
	hpFrameLeftSprite_->Draw();
	delayHpMiddleSprite_->Draw();
	delayHpLeftSprite_->Draw();
	delayHpRightSprite_->Draw();
	hpMiddleSprite_->Draw();
	hpRightSprite_->Draw();
	hpLeftSprite_->Draw();
}


/// @brief キャラクターを生成する
/// @param initData 
/// @return 
Character* GameScene::CreateCharacter(const Character::InitData& initData, Character::CharacterTag tag)
{
	Character* character = nullptr;

	if (tag == Character::CharacterTag::Player)
	{
		// すでにプレイヤーが存在する場合は削除する
		if (player_)
		{
			player_.reset();
			player_ = nullptr;
		}

		// すでにプレイヤーの武器が存在する場合は削除する
		if (playerWeapon_)
		{
			playerWeapon_.reset();
			playerWeapon_ = nullptr;
		}

		// すでにプレイヤーの体力バーが存在する場合は削除する
		if (playerHP_)
		{
			playerHP_.reset();
			playerHP_ = nullptr;
		}

		// プレイヤーの武器の生成と初期化
		Weapon::InitData playerWeaponInitData;
		playerWeaponInitData.position = Vector3(0.0f, 0.0f, 0.0f);
		playerWeaponInitData.model = oneHandedWeaponModel_->CreateInstance();
		playerWeaponInitData.durability = 0;
		playerWeaponInitData.attackPower = 1.0f;
		playerWeaponInitData.category = WeaponCategory::OneHanded;
		playerWeaponInitData.isUnbreakable = true;
		playerWeaponInitData.landingCollision = landingCollision_->CreateInstance();
		playerWeapon_ = std::make_unique<Weapon>(playerWeaponInitData);

		// プレイヤーの生成処理
		Character::InitData playerInitData = initData;
		playerInitData.hurtboxGroup = playerHurtboxGroup_.get();
		playerInitData.hitboxGroup = playerHitboxGroup_.get();
		playerInitData.landingCollision = landingCollision_->CreateInstance();
		playerInitData.wallTouchCollision = wallTouchCollision_->CreateInstance();
		playerInitData.eventTriggerCollision = eventTriggerCollision_->CreateInstance();
		playerInitData.model_ = playerModel_.get();
		playerInitData.attackTrail = playerTrail_.get();
		player_ = std::make_unique<Player>(playerInitData);
		player_->Initialize(playerWeapon_.get());

		character = player_.get();

		// 体力ゲージの生成と初期化
		HP::InitData hpInitData;
		hpInitData.position = Vector2(0.0f, 0.0f);
		hpInitData.hpFrameLeftSprite = hpFrameLeftSprite_->CreateInstance();
		hpInitData.hpFrameMiddleSprite = hpFrameMiddleSprite_->CreateInstance();
		hpInitData.hpFrameRightSprite = hpFrameRightSprite_->CreateInstance();
		hpInitData.hpLeftSprite = hpLeftSprite_->CreateInstance();
		hpInitData.hpMiddleSprite = hpMiddleSprite_->CreateInstance();
		hpInitData.hpRightSprite = hpRightSprite_->CreateInstance();
		hpInitData.delayHpLeftSprite = delayHpLeftSprite_->CreateInstance();
		hpInitData.delayHpMiddleSprite = delayHpMiddleSprite_->CreateInstance();
		hpInitData.delayHpRightSprite = delayHpRightSprite_->CreateInstance();
		hpInitData.alpha = 1.0f;
		hpInitData.scale = Vector2(1.0f, 1.0f);
		hpInitData.width = 600;
		hpInitData.color = Vector3(0.25f, 1.0f, 0.25f);
		playerHP_ = std::make_unique<HP>();
		playerHP_->Initialize(hpInitData);
		playerHP_->SetMaxHP(player_->GetHp());
		playerHP_->SetCurrentHP(player_->GetHp());


		// カメラ制御の初期化
		InitializeCameraControl();
	}
	else
	{
		Character::InitData npcInitData = initData;

		// NPCのモデルの生成と初期化
		std::unique_ptr<Render3DSkinningModel> npcModel =
			std::make_unique<Render3DSkinningModel>(hCharacterModel_, hCharacterSkeleton_, hCharacterAnimation_, "Enemy_Model_" + std::to_string(npcCount_));

		// NPCのトレイルの生成と初期化
		std::unique_ptr<Trail3D> npcTrail = std::make_unique<Trail3D>("Enemy_Trail_" + std::to_string(npcCount_), 0.15f, engine_->LoadTexture("./Assets/Textures/trail_000.png"));

		// NPCのモデルを初期化データに設定する
		npcInitData.model_ = npcModel.get();
		npcInitData.attackTrail = npcTrail.get();

		// NPCの当たり判定グループの設定
		if (tag == Character::CharacterTag::Ally || tag == Character::CharacterTag::Vip)
		{
			npcInitData.hurtboxGroup = playerHurtboxGroup_.get();
			npcInitData.hitboxGroup = playerHitboxGroup_.get();
		}
		else if (tag == Character::CharacterTag::EnemyNormal || tag == Character::CharacterTag::EnemyBoss)
		{
			npcInitData.hurtboxGroup = enemyHurtboxGroup_.get();
			npcInitData.hitboxGroup = enemyHitboxGroup_.get();
		}

		// 着地判定グループの設定
		npcInitData.landingCollision = landingCollision_->CreateInstance();
		npcInitData.wallTouchCollision = wallTouchCollision_->CreateInstance();

		// 体力ゲージの生成と初期化
		HP::InitData hpInitData;
		hpInitData.position = Vector2(0.0f, 0.0f);
		hpInitData.hpFrameLeftSprite = hpFrameLeftSprite_->CreateInstance();
		hpInitData.hpFrameMiddleSprite = hpFrameMiddleSprite_->CreateInstance();
		hpInitData.hpFrameRightSprite = hpFrameRightSprite_->CreateInstance();
		hpInitData.hpLeftSprite = hpLeftSprite_->CreateInstance();
		hpInitData.hpMiddleSprite = hpMiddleSprite_->CreateInstance();
		hpInitData.hpRightSprite = hpRightSprite_->CreateInstance();
		hpInitData.delayHpLeftSprite = delayHpLeftSprite_->CreateInstance();
		hpInitData.delayHpMiddleSprite = delayHpMiddleSprite_->CreateInstance();
		hpInitData.delayHpRightSprite = delayHpRightSprite_->CreateInstance();
		hpInitData.alpha = 1.0f;

		if (tag == Character::CharacterTag::Ally)
		{
			hpInitData.scale = Vector2(0.75f, 0.75f);
			hpInitData.width = 150;
			hpInitData.color = Vector3(0.25f, 1.0f, 0.25f);
		}
		else if (tag == Character::CharacterTag::EnemyNormal)
		{
			hpInitData.scale = Vector2(0.5f, 0.5f);
			hpInitData.width = 200;
			hpInitData.color = Vector3(1.0f, 0.25f, 0.25f);
		}

		
		std::unique_ptr<HP> hp = std::make_unique<HP>();
		hp->Initialize(hpInitData);
		npcInitData.hpHUD = hp.get();

		// NPCの生成処理
		std::unique_ptr<NPC> npc = std::make_unique<NPC>(npcInitData, tag);
		npc->Initialize(behaviorTreeEditor_->CreateTree(npcInitData.behaviorTreeName, npc.get()), navMesh_.get());
		character = npc.get();

		// NPCのリストに追加する
		npcs_.push_back(std::move(npc));
		npcModels_.push_back(std::move(npcModel));
		npcTrails_.push_back(std::move(npcTrail));
		huds_.push_back(std::move(hp));

		// NPCをカウントする
		npcCount_++;
	}

	return character;
}

/// @brief 武器を生成する
/// @param position 
/// @return 
Weapon* GameScene::CreateWeapon(const Weapon::InitData& initData)
{
	Weapon* weapon = nullptr;

	Weapon::InitData weaponInitData = initData;
	weaponInitData.landingCollision = landingCollision_->CreateInstance();
	weaponInitData.model = oneHandedWeaponModel_->CreateInstance();

	// 武器の生成処理
	std::unique_ptr<Weapon> newWeapon = std::make_unique<Weapon>(weaponInitData);
	weapon = newWeapon.get();

	// 武器のリストに追加する
	weapons_.push_back(std::move(newWeapon));

	return weapon;
}

/// @brief 床オブジェクトを生成する
/// @param position 
/// @param scale 
/// @return 
Floor* GameScene::CreateFloorObject(const Floor::InitData& initData)
{
	// 床
	Floor::InitData floorInitData = initData;
	floorInitData.collision = floorCollision_->CreateInstance();
	floorInitData.model = trialCube_->CreateInstance();

	std::unique_ptr<Floor> newFloor = std::make_unique<Floor>();
	newFloor->Initialize(floorInitData);
	Floor* floor = newFloor.get();

	// ステージオブジェクトのリストに追加する
	objects_.push_back(std::move(newFloor));

	return floor;
}

/// @brief 壁オブジェクトを生成する
/// @param initData 
/// @return 
Wall* GameScene::CreateWallObject(const Wall::InitData& initData)
{
	// 壁
	Wall::InitData wallInitData = initData;
	wallInitData.collision = wallCollision_->CreateInstance();
	wallInitData.model = trialCube_->CreateInstance();

	std::unique_ptr<Wall> newWall = std::make_unique<Wall>();
	newWall->Initialize(wallInitData);
	Wall* wall = newWall.get();

	objects_.push_back(std::move(newWall));

	return wall;
}

/// @brief タイマーHUDを生成する
/// @param initData 
/// @return 
Timer* GameScene::CreateTimer(const Timer::InitData& initData)
{
	Timer::InitData timerInitData = initData;
	timerInitData.timerSprite[0] = numbersSprite_->CreateInstance();
	timerInitData.timerSprite[1] = numbersSprite_->CreateInstance();
	timerInitData.timerSprite[2] = numbersSprite_->CreateInstance();
	timerInitData.timerSprite[3] = numbersSprite_->CreateInstance();
	timerInitData.commaSprite = commaSprite_->CreateInstance();

	std::unique_ptr<Timer> newTimer = std::make_unique<Timer>();
	newTimer->Initialize(timerInitData);
	Timer* timer = newTimer.get();

	huds_.push_back(std::move(newTimer));

	return timer;
}

/// @brief 静的イベントトリガーオブジェクトを生成する
/// @param initData 
/// @return 
StaticEventTrigger* GameScene::CreateStaticEventTrigger(const StaticEventTrigger::InitData& initData)
{
	StaticEventTrigger::InitData triggerInitData = initData;
	triggerInitData.collision = eventTriggerAABBCollision_->CreateInstance();
	triggerInitData.onTriggerCallback = [this](int eventType, const char* param) -> bool { return HandleTriggerEvent(eventType, param); };

	std::unique_ptr<StaticEventTrigger> newTrigger = std::make_unique<StaticEventTrigger>();
	newTrigger->Initialize(triggerInitData);
	StaticEventTrigger* trigger = newTrigger.get();

	objects_.push_back(std::move(newTrigger));

	return trigger;
}

/// @brief 攻撃チュートリアルを生成する
/// @param initData 
/// @return 
AttackTutorial* GameScene::CreateAttackTutorial(const AttackTutorial::InitData& initData)
{
	AttackTutorial::InitData tutorialInitData = initData;
	tutorialInitData.player = player_.get();
	tutorialInitData.sprite = attackButtonSprite.get();

	std::unique_ptr<AttackTutorial> newTutorial = std::make_unique<AttackTutorial>();
	newTutorial->Initialize(tutorialInitData);
	AttackTutorial* tutorial = newTutorial.get();

	huds_.push_back(std::move(newTutorial));

	return tutorial;
}

/// @brief ガードチュートリアルを生成する
/// @param initData 
/// @return 
GuardTutorial* GameScene::CreateGuardTutorial(const GuardTutorial::InitData& initData)
{
	GuardTutorial::InitData tutorialInitData = initData;
	tutorialInitData.player = player_.get();

	std::unique_ptr<GuardTutorial> newTutorial = std::make_unique<GuardTutorial>();
	newTutorial->Initialize(tutorialInitData);
	GuardTutorial* tutorial = newTutorial.get();

	huds_.push_back(std::move(newTutorial));

	return tutorial;
}

/// @brief リセットする
void GameScene::Reset()
{
	// プレイヤーをリセットする
	if (player_)player_ = nullptr;

	// npcのカウンターをリセットする
	npcCount_ = 0;

	// バトル制御をリセットする
	BattleDirector::GetInstance().Clear();
}


/// @brief カメラ制御の初期化
void GameScene::InitializeCameraControl()
{
	// プレイヤーがいない場合はカメラ制御を初期化しない
	if (!player_)return;

	// すでにピボットポイントが存在する場合は削除する
	if (pivotPoint_)
	{
		pivotPoint_.reset();
		pivotPoint_ = nullptr;
	}

	// カメラ用のピボットポイントを生成する
	pivotPoint_ = std::make_unique<PivotPoint>();
	pivotPoint_->GetData()->center = player_->GetPosition();
	pivotPoint_->GetData()->radius = 8.0f;
	pivotPoint_->GetData()->phi = -std::numbers::pi_v<float> / 2.0f;

	// カメラ回転入力の生成
	inputCameraRotate_ = std::make_unique<InputGamepadRightStick>("Camera_Rotate", InputState::Press, 0, Vector2(0.0f, 0.0f), 0.5f);
}

/// @brief カメラ制御の更新
/// @param deltaTime
void GameScene::UpdateCameraControl(float deltaTime)
{
	// プレイヤーがいない場合はカメラ制御を更新しない
	if (!player_)return;

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
	// プレイヤーがいない場合は更新しない
	if (!player_)return;

	// ピボットのデータを取得する
	PivotPoint::Data* pivotData = pivotPoint_->GetData();

	// ターゲットの位置をプレイヤーの位置に設定する
	Vector3 targetPivotPos = player_->GetPosition();

	// プレイヤーの位置からピボット中心までの高さオフセットを加算する
	targetPivotPos.y += 1.5f;

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
	// プレイヤーがいない場合は更新しない
	if (!player_)return;

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
		// プレイヤーに手動でカメラ回転していることを通知する
		player_->SetIsOperationCamera(true);

		// 手動でカメラ回転入力がある場合はピボットを回転させる
		if (!isKeyCameraRotate)
		{
			pivotData->phi += -rightStick.x * kPivotRotateSpeed * deltaTime;
			pivotData->theta += -rightStick.y * kPivotRotateSpeed * deltaTime;
		}
	}
	else if (player_->GetLockOnTarget() != nullptr)
	{
		// ロックオン中はターゲットの方向にピボットを回転させる
		Character* target = player_->GetLockOnTarget();
		Vector3 targetPos = target->GetPosition();
		targetPos.y += 1.0f;

		// ターゲットの方向ベクトルを計算する
		Vector3 dir = targetPos - pivotData->center;
		if (target->IsBlownAway() || target->IsBlownFalling())dir.y = 0.0f;
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
	// プレイヤーがいない場合は更新しない
	if (!player_)return;

	PivotPoint::Data* pivotData = pivotPoint_->GetData();

	// カメラ位置と回転をピボット情報から設定する
	if (Engine::Camera3DData::Param* cameraParam = engine_->GetCamera3DParam("MainCamera"))
	{
		Vector3 finalCameraPos = pivotData->sphericalCoordinates;
		if (cameraShake_) finalCameraPos += cameraShake_->GetShakeOffset();

		cameraParam->transform.translate = finalCameraPos;

		// center方向を向くようにオイラー角を計算する
		const Vector3 lookDirection = pivotData->toCenter;
		const float yaw = std::atan2(lookDirection.x, lookDirection.z);
		const float horizontal = std::sqrt(lookDirection.x * lookDirection.x + lookDirection.z * lookDirection.z);
		const float pitch = std::atan2(-lookDirection.y, horizontal);
		cameraParam->transform.rotate = Vector3(pitch, yaw, 0.0f);
	}
}

/// @brief イベントトリガーに触れたときの処理
/// @param eventType 
/// @param param 
bool GameScene::HandleTriggerEvent(int eventType, const char* param)
{
	// ここではイベントの種類に応じて処理を分岐させることができます
	StaticEventTrigger::EventType type = static_cast<StaticEventTrigger::EventType>(eventType);

	switch (type)
	{
		// イベントなし
	case StaticEventTrigger::EventType::None:
		return true;
		break;

		// オブジェクトのスポーンイベント
	case StaticEventTrigger::EventType::ObjectSpawn:

		try
		{
			// param が空文字列の場合は何もしない
			std::string fileName = param;
			if (fileName.empty()) return true;

			// ステージデータが保存されているディレクトリのパスと結合
			// (必要であれば拡張子 .json を付ける)
			std::string filePath = "./Assets/Parameter/StageData/" + fileName + ".json";

			// ファイルストリームを開く
			std::ifstream ifs(filePath);
			if (!ifs.is_open())return false;

			// ファイルからJSONを読み込んで解析
			nlohmann::json j;
			ifs >> j;
			ifs.close();

			// JSON配列をループして、記述された各種オブジェクトを生成する
			if (j.contains("objects") && j["objects"].is_array())
			{
				for (const auto& objectDataJson : j["objects"])
				{
					PlacementData initData;
					fromJson(objectDataJson, initData);
					// 解析したデータをもとにオブジェクトを生成する
					stageEditor_->SpawnObject(initData);
					stageEditor_->SetPlacementList(initData);
				}

				j.erase("objects");
			}

			// 生成が終わったイベントトリガーを削除する場合は true を返す
			return true;
		}
		catch (const std::exception& e)
		{
			// JSONの解析に失敗した場合はエラーを出力してイベントトリガーを削除しない
			(void)e; // 未使用変数の警告を消すため
			return false;
		}
		break;
		
		break;
	}

	// イベントを処理した場合はtrueを返し、処理しなかった場合はfalseを返す
	return false;
}


/// @brief HUDらの読み込み
void GameScene::LoadHUDs()
{
	// 体力バーの枠
	hpFrameLeftSprite_ = std::make_unique<PrefabBaseSprite>(engine_->LoadTexture("./Assets/Textures/hp_left_frame.png"), 100, "HP_Frame_Left_Sprite");
	hpFrameLeftSprite_->param_->texture.anchor = Vector2(1.0f, 0.5f);

	hpFrameMiddleSprite_ = std::make_unique<PrefabBaseSprite>(engine_->LoadTexture("./Assets/Textures/hp_middle_frame.png"), 100, "HP_Frame_Middle_Sprite");
	hpFrameMiddleSprite_->param_->texture.anchor = Vector2(0.5f, 0.5f);

	hpFrameRightSprite_ = std::make_unique<PrefabBaseSprite>(engine_->LoadTexture("./Assets/Textures/hp_right_frame.png"), 100, "HP_Frame_Right_Sprite");
	hpFrameRightSprite_->param_->texture.anchor = Vector2(0.0f, 0.5f);

	// 体力バー
	hpLeftSprite_ = std::make_unique<PrefabBaseSprite>(engine_->LoadTexture("./Assets/Textures/hp_left.png"), 100, "HP_Left_Sprite");
	hpLeftSprite_->param_->texture.anchor = Vector2(1.0f, 0.5f);

	hpMiddleSprite_ = std::make_unique<PrefabBaseSprite>(engine_->LoadTexture("./Assets/Textures/hp_middle.png"), 100, "HP_Middle_Sprite");
	hpMiddleSprite_->param_->texture.anchor = Vector2(0.0f, 0.5f);

	hpRightSprite_ = std::make_unique<PrefabBaseSprite>(engine_->LoadTexture("./Assets/Textures/hp_right.png"), 100, "HP_Right_Sprite");
	hpRightSprite_->param_->texture.anchor = Vector2(0.0f, 0.5f);

	// 遅延体力バー
	delayHpLeftSprite_ = std::make_unique<PrefabBaseSprite>(engine_->LoadTexture("./Assets/Textures/hp_left.png"), 100, "Delay_HP_Left_Sprite");
	delayHpLeftSprite_->param_->texture.anchor = Vector2(1.0f, 0.5f);

	delayHpMiddleSprite_ = std::make_unique<PrefabBaseSprite>(engine_->LoadTexture("./Assets/Textures/hp_middle.png"), 100, "Delay_HP_Middle_Sprite");
	delayHpMiddleSprite_->param_->texture.anchor = Vector2(0.0f, 0.5f);

	delayHpRightSprite_ = std::make_unique<PrefabBaseSprite>(engine_->LoadTexture("./Assets/Textures/hp_right.png"), 100, "Delay_HP_Right_Sprite");
	delayHpRightSprite_->param_->texture.anchor = Vector2(0.0f, 0.5f);
}