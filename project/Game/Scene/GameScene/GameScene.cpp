#include "GameScene.h"
#include "BattleDirector/BattleDirector.h"

#include <algorithm>
#include <cmath>
#include <numbers>

#include "HUD/HP/BossHP/BossHP.h"
#include "HUD/Button/WeaponGetButton/WeaponGetButton.h"
#include "HUD/IntroText/IntroText.h"

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

/// @brief デストラクタ
GameScene::~GameScene()
{
	// クリア処理
	BattleDirector::GetInstance().Clear();
}

/// @brief 初期化
void GameScene::Initialize()
{
	// リストをクリアする
	npcs_.clear();
	battleAreas_.clear();
	npcModels_.clear();
	npcTrails_.clear();
	npcParticles_.clear();
	weapons_.clear();
	objects_.clear();
	huds_.clear();

	// キャラクターの終了フラグをリセットする
	Character::SetIsGameFinished(false);

	// アウトラインのポストエフェクトを読み込む
	engine_->LoadPostEffect("Outline", Engine::PostEffect::Type::DepthBasedOutline);
	auto outlineParam = engine_->GetPostEffectParam<Engine::PostEffect::DepthBasedOutline>("Outline");
	outlineParam->outlineWidth = 1.5f;

	// 演出用カメラの読み込みとカットシーンマネージャの生成
	cutsceneCamera_ = std::make_unique<MainCamera3D>("CutsceneCamera");
	cutsceneManager_ = std::make_unique<CutsceneManager>();
	cutsceneManager_->Initialize(cutsceneCamera_.get());

	// カメラの読み込みと切り替え
	mainCamera_ = std::make_unique<MainCamera3D>("MainCamera");
	mainCamera_->Switch();

	// 攻撃者の方向を示す矢印カメラの生成
	attackerArrowCamera_ = std::make_unique<MainCamera3D>("AttackerArrowCamera");
	attackerArrowCamera_->param_->transform.rotate.x = 0.6f;
	attackerArrowCamera_->param_->transform.translate = Vector3(0.0f, 35.0f, -16.0f);

	// 攻撃者の方向を示す矢印モデルの生成
	attackerArrowModel_ = std::make_unique<Render3DStaticModel>(engine_->LoadModel("./Assets/Models/attackerArrow" , "attackerArrow.obj"), "AttackerArrowModel");
	attackerArrowModel_->param_->meshMaterial[0].enableLighting = false;
	attackerArrowModel_->param_->modelTransform.rotate = Vector3(0.0f, 0.0f, 0.0f);
	attackerArrowModel_->param_->modelTransform.translate = Vector3(0.0f, 0.0f, 20.0f);
	attackerArrowModel_->param_->blendMode = BlendMode::kNormal;
	attackerArrowModel_->param_->meshMaterial[0].color.w = 0.0f;

	// AI計測用エディタの生成
	aiMetricsEditor_ = std::make_unique<AIMetricsEditor>();
	aiMetricsEditor_->Initialize(mainCamera_.get());

	// 太陽光の生成と初期化
	sunLight_ = std::make_unique<LightDirectional>("SunLight");
	sunLight_->param_->intensity = 1.0f;
	sunLight_->param_->color = Vector3(0.5f, 0.5f, 1.0f);

	// マネージャの生成と初期化
	motionManager_ = MotionManager::GetInstance();
	soundManager_ = SoundManager::GetInstance();
	effectManager_ = EffectManager::GetInstance();

	soundManager_->BgmTutorialBossPlay(false);
	soundManager_->BgmTutorialRoadPlay(false);

	// ポストエフェクトマネージャの生成と初期化
	postEffectManager_ = std::make_unique<PostEffectManager>();
	postEffectManager_->Initialize();

	// 2Dスプライトの影の生成と初期化
	spriteShadow_ = std::make_unique<PostEffectBlurShadow2D>("SpriteShadow");

	// カメラシェイクの生成と初期化
	cameraShake_ = std::make_unique<Shake>();

	// モーションマネージャのエディタの生成と初期化
	motionManagerEditor_ = std::make_unique<MotionManagerEditor>();

	// ビヘイビアツリーエディタの生成と初期化
	behaviorTreeEditor_ = std::make_unique<BehaviorTreeEditor>();

	// ビヘイビアツリービューアの生成と初期化
	behaviorTreeViewer_ = std::make_unique<BehaviorTreeViewer>();

	// コンボツリーエディタの生成と初期化
	comboTreeEditor_ = std::make_unique<ComboTreeEditor>();

	// カットシーンエディタの生成と初期化
	cutsceneEditor_ = std::make_unique<CutsceneEditor>();
	cutsceneEditor_->Initialize(cutsceneCamera_.get(), mainCamera_.get());

	// UIエディタの生成と初期化
	uiEditor_ = std::make_unique<UIEditor>();
	uiEditor_->Load("Game_Scene");

	// モデルエディタの生成と初期化
	modelEditor_ = std::make_unique<ModelEditor>();

	// ライトエディタの生成と初期化
	lightEditor_ = std::make_unique<LightEditor>();

	// ナビゲーションメッシュの生成と初期化
	navMesh_ = std::make_unique<NavMesh>();

	// ステージエディタの生成と初期化
	stageEditor_ = std::make_unique<StageEditor>(this);
	stageEditor_->Initialize();

	// エディタワークスペースマネージャの生成と初期化
	editorWorkspaceManager_ = std::make_unique<EditorWorkspaceManager>();
	editorWorkspaceManager_->Initialize(stageEditor_.get(), behaviorTreeEditor_.get(), behaviorTreeViewer_.get(), comboTreeEditor_.get(),
		cutsceneEditor_.get(), uiEditor_.get(), modelEditor_.get(), lightEditor_.get());

	// キャラクターモデルの読み込み
	hCharacterModel_ = engine_->LoadModel("./Assets/Models/Character", "bone.gltf");
	hCharacterAnimation_ = motionManager_->GetMotion(MotionType::Stand, "Standing");
	hCharacterSkeleton_ = motionManager_->GetSkeleton();


	// NPCモデルプールの生成と初期化
	npcModelPool_ = std::make_unique<Pool<Render3DSkinningModel>>([this]()
		{
			int count = npcModelPool_->GetCount() + 1;
			npcModelPool_->SetCount(count);

			std::unique_ptr<Render3DSkinningModel> model = 
				std::make_unique<Render3DSkinningModel>(hCharacterModel_, hCharacterAnimation_, hCharacterSkeleton_, "NPC_Model_" + std::to_string(count - 1));
			model->param_->isUpdate = false;

			return std::move(model); 
		}
	);
	npcModelPool_->PreAllocate(30);

	// NPCトレイルプールの生成と初期化
	npcTrailPool_ = std::make_unique<Pool<Trail3D>>([this]()
		{
			int count = npcTrailPool_->GetCount() + 1;
			npcTrailPool_->SetCount(count);

			std::unique_ptr<Trail3D> trail = std::make_unique<Trail3D>("NPC_Trail_" + std::to_string(count - 1), 0.15f, engine_->LoadTexture("./Assets/Textures/trail_000.png"));
			trail->param_->isUpdate_ = false;
			return std::move(trail);
		}
	);
	npcTrailPool_->PreAllocate(30);

	/// @brief NPCプールの生成と初期化
	npcPool_ = std::make_unique<Pool<NPC>>([this]() {return std::make_unique<NPC>(); });
	npcPool_->PreAllocate(30);

	// HUDの読み込み
	LoadHUDs();


	// プレイヤーのモデルの生成と初期化
	playerModel_ = std::make_unique<Render3DSkinningModel>(hCharacterModel_, hCharacterAnimation_, hCharacterSkeleton_, "Player_Model");
	playerModel_->param_->meshOutline[0].enableOutline = true;
	playerModel_->param_->meshOutline[0].color = Vector4(0.1f, 0.1f, 0.1f, 1.0f);

	// プレイヤーの生成と初期化
	playerTrail_ = std::make_unique<Trail3D>("Player_Trail", 0.15f, engine_->LoadTexture("./Assets/Textures/trail_000.png"));

	// 片手武器モデルの読み込み
	oneHandedWeaponModel_ = std::make_unique<PrefabBaseStaticModel>(engine_->LoadModel("./Assets/Models/weapon/PoliceBaton", "PoliceBaton.obj"), 100, "PoliceBaton");


	// コンマスプライトの生成と初期化
	commaSprite_ = std::make_unique<PrefabBaseSprite>(engine_->LoadTexture("./Assets/Textures/comma.png"), 100, "Comma_Sprite");

	// 数字スプライトの生成と初期化
	numbersSprite_ = std::make_unique<PrefabBaseSprite>(engine_->LoadTexture("./Assets/Textures/numbers.png"), 100, "Numbers_Sprite");


	MashButton::InitData xButtonInitData;
	xButtonInitData.buttonSprite = xButtonPrefab_->CreateInstance();
	xButtonInitData.buttonInSprite = buttonInSprite_->CreateInstance();
	xButtonInitData.buttonOutSprite = buttonOutSprite_->CreateInstance();
	xButtonInitData.position = Vector2(200.0f, 200.0f);
	xButtonInitData.scale = Vector2(0.3f, 0.3f);
	xButtonInitData.color = Vector3(1.0f, 1.0f, 0.5f);
	xButton_ = std::make_unique<MashButton>();
	xButton_->Initialize(xButtonInitData);

	MashButton::InitData yButtonInitData;
	yButtonInitData.buttonSprite = yButtonPrefab_->CreateInstance();
	yButtonInitData.buttonInSprite = buttonInSprite_->CreateInstance();
	yButtonInitData.buttonOutSprite = buttonOutSprite_->CreateInstance();
	yButtonInitData.position = Vector2(200.0f, 300.0f);
	yButtonInitData.scale = Vector2(0.3f, 0.3f);
	yButtonInitData.color = Vector3(1.0f, 1.0f, 0.5f);
	yButton_ = std::make_unique<MashButton>();
	yButton_->Initialize(yButtonInitData);

	MashButton::InitData aButtonInitData;
	aButtonInitData.buttonSprite = xButtonPrefab_->CreateInstance();
	aButtonInitData.buttonInSprite = buttonInSprite_->CreateInstance();
	aButtonInitData.buttonOutSprite = buttonOutSprite_->CreateInstance();
	aButtonInitData.position = Vector2(200.0f, 200.0f);
	aButtonInitData.scale = Vector2(0.3f, 0.3f);
	aButtonInitData.color = Vector3(1.0f, 1.0f, 0.5f);
	aButton_ = std::make_unique<MashButton>();
	aButton_->Initialize(aButtonInitData);

	MashButton::InitData bButtonInitData;
	bButtonInitData.buttonSprite = yButtonPrefab_->CreateInstance();
	bButtonInitData.buttonInSprite = buttonInSprite_->CreateInstance();
	bButtonInitData.buttonOutSprite = buttonOutSprite_->CreateInstance();
	bButtonInitData.position = Vector2(200.0f, 300.0f);
	bButtonInitData.scale = Vector2(0.3f, 0.3f);
	bButtonInitData.color = Vector3(1.0f, 1.0f, 0.5f);
	bButton_ = std::make_unique<MashButton>();
	bButton_->Initialize(bButtonInitData);

	TriggerButton::InitData rtTriggerButtonInitData;
	rtTriggerButtonInitData.buttonSprite = rtButtonPrefab_->CreateInstance();
	rtTriggerButtonInitData.buttonInSprite = buttonInSprite_->CreateInstance();
	rtTriggerButtonInitData.buttonOutSprite = buttonOutSprite_->CreateInstance();
	rtTriggerButtonInitData.position = Vector2(200.0f, 200.0f);
	rtTriggerButtonInitData.scale = Vector2(0.3f, 0.3f);
	rtTriggerButtonInitData.color = Vector3(1.0f, 1.0f, 0.5f);
	rtTriggerButton_ = std::make_unique<TriggerButton>();
	rtTriggerButton_->Initialize(rtTriggerButtonInitData);
	

	
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

	// カメラガードの当たり判定グループの生成と初期化
	cameraGuardCollision_ = std::make_unique<Collision3DBaseOBB>("CameraGuard_Collision");
	cameraSegmentCollision_ = std::make_unique<Collision3DBaseSegment>("CameraSegment_Collision");
	cameraSegmentInstance_ = cameraSegmentCollision_->CreateInstance();



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

	// 「カメラガード」に当たる
	cameraSegmentCollision_->SetCollisionTarget(cameraGuardCollision_->GetHandle());

	// ステージの読み込み
	stageEditor_->LoadStage(sceneManager_->GetNextStageName());

	// オブジェクトの描画レンダーパスの読み込み
	engine_->LoadRenderPass("Object", [&]()
		{
			engine_->DrawToRenderPass("Object", "PrevDraw");

			// エディタの描画
			editorWorkspaceManager_->DrawUI();

			// ステージオブジェクトの描画
			for (auto& object : objects_)object->Draw();

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

			// エディタ内のモデル描画
			modelEditor_->Draw();

			// プレハブの描画処理
			oneHandedWeaponModel_->Draw();

			// エフェクトの描画
			effectManager_->Draw();

			// HUDの描画
			for (auto& hud : huds_)hud->Draw();

			// プレイヤーの体力バーの描画
			if (playerHP_)playerHP_->Draw();

			// プレイヤーのレイジゲージの描画
			if (playerRageGage_)playerRageGage_->Draw();

			// 武器の耐久ゲージの描画
			if (weaponDurabilityGage_)weaponDurabilityGage_->Draw();

			// アウトラインの描画
			engine_->DrawOutline();
		}
	);

	// ポストエフェクトの描画レンダーパスの読み込み
	engine_->LoadRenderPass("PostEffect", [&]()
		{
			engine_->DrawToRenderPass("PostEffect", "Object");

			// ポストエフェクトの描画処理
			postEffectManager_->Draw(player_.get());
		}
	);

	// HUDの描画レンダーパスの読み込み
	engine_->LoadRenderPass("HUD", [&]()
		{
			engine_->DrawToRenderPass("HUD", "PostEffect");

			// レティクルの描画
			reticle_->Draw();

			// 武器入手ボタンの描画
			weaponGetButtonSpritePrefab_->Draw();

			// 体力バーの描画
			hpFrameMiddleSprite_->Draw();
			hpFrameRightSprite_->Draw();
			hpFrameLeftSprite_->Draw();
			hpBackMiddleSprite_->Draw();
			hpBackLeftSprite_->Draw();
			hpBackRightSprite_->Draw();
			delayHpMiddleSprite_->Draw();
			delayHpLeftSprite_->Draw();
			delayHpRightSprite_->Draw();
			hpMiddleSprite_->Draw();
			hpRightSprite_->Draw();
			hpLeftSprite_->Draw();
			delayHpFrontMiddleSprite_->Draw();
			delayHpFrontLeftSprite_->Draw();
			delayHpFrontRightSprite_->Draw();
			hpSeparatorSprite_->Draw();
			buttonInSprite_->Draw();
			buttonOutSprite_->Draw();
			textFrameMiddleSprite_->Draw();
			textFrameLeftSprite_->Draw();
			textFrameRightSprite_->Draw();
			yButtonPrefab_->Draw();
			xButtonPrefab_->Draw();
			bButtonPrefab_->Draw();
			aButtonPrefab_->Draw();
			rbButtonPrefab_->Draw();
			rtButtonPrefab_->Draw();
			lbButtonPrefab_->Draw();
			ltButtonPrefab_->Draw();

			reticleFrameSpritePrefab_->Draw();

			// ナビゲーション矢印の描画
			//navigationArrow_->Draw();

			// エディタ内のUI描画
			uiEditor_->Draw();

			stickTutorial_->Draw();
			dashTutorial_->Draw();
			attackTutorial_->Draw();
			comboTutorial_->Draw();
			grabTutorial_->Draw();
			guardTutorial_->Draw();
			avoidTutorial_->Draw();
			rageTutorial_->Draw();

			// AI計測用エディタの更新
			aiMetricsEditor_->Draw();
		}
	);

	engine_->LoadRenderPass("AttackerArrow", [&]()
		{
			// 攻撃者の方向を示す矢印カメラに切り替える
			attackerArrowCamera_->Switch();

			// 攻撃者の方向を示す矢印の描画
			attackerArrowModel_->Draw();

			// メインカメラに切り替える
			mainCamera_->Switch();
		}
	);
	engine_->GetRenderPassParam("AttackerArrow")->blendMode = BlendMode::kAdd;

	// レンダーパスの読み込み
	engine_->LoadRenderPass("MainPass", [&]()
		{
			engine_->DrawToRenderPass("MainPass", "HUD");

			engine_->DrawToRenderPass("MainPass", "AttackerArrow");

			// フェードスプライトの描画
			fadeSprite_->Draw();
		}
	);


	// フェーズマネージャの生成と初期化
	phaseManager_ = std::make_unique<PhaseManager<PhaseType>>();
	phaseManager_->SetOnEnter(PhaseType::Intro, [&]() { IntroPhaseInitialize(); });
	phaseManager_->SetOnUpdate(PhaseType::Intro, [&]() { IntroPhaseUpdate(); });
	phaseManager_->SetOnEnter(PhaseType::Battle, [&]() { BattlePhaseInitialize(); });
	phaseManager_->SetOnUpdate(PhaseType::Battle, [&]() { BattlePhaseUpdate(); });
	phaseManager_->SetOnEnter(PhaseType::Pause, [&]() { PausePhaseInitialize(); });
	phaseManager_->SetOnUpdate(PhaseType::Pause, [&]() { PausePhaseUpdate(); });
	phaseManager_->SetOnEnter(PhaseType::Finish, [&]() { FinishPhaseInitialize(); });
	phaseManager_->SetOnUpdate(PhaseType::Finish, [&]() { FinishPhaseUpdate(); });
	phaseManager_->SetOnEnter(PhaseType::Out, [&]() { OutPhaseInitialize(); });
	phaseManager_->SetOnUpdate(PhaseType::Out, [&]() { OutPhaseUpdate(); });
	phaseManager_->ChangePhase(PhaseType::Intro);
}

/// @brief 更新処理
void GameScene::Update()
{
	// デルタタイムを取得する
	const float kDt = engine_->GetDeltaTime() * engine_->GetTimeScale();

	// 各エディタの更新処理を呼び出す
#ifdef DEVELOPMENT
	cutsceneEditor_->SetActive(editorWorkspaceManager_->GetCurrentWorkspace() == WorkspaceType::CutsceneEditor ? true : false);
#endif 
	cutsceneEditor_->Update(engine_->GetDeltaTime());

	// カットシーンの更新
	if (cutsceneManager_->IsPlaying())
		cutsceneManager_->Update(kDt);

	// フェーズマネージャの更新
	phaseManager_->Update();

	// ステージエディタの更新
	stageEditor_->Update(kDt);

	// レティクルの更新
	reticle_->Update();

	// ロックオンターゲットの位置をレティクルに反映する
	if (player_)
	{
		if (auto target = player_->GetLockOnTarget())
		{
			reticle_->LockOn(target);
		}
	}

	// 攻撃者の方向を示す矢印の回転を更新する
	std::optional<Vector2> toAttacker = GetToAttacker();
	if (toAttacker)
	{
		Vector2 direction = toAttacker.value().Normalize();

		// 角度をラジアンに変換する
		float radian = std::atan2(direction.x, direction.y);
		attackerArrowModel_->param_->modelTransform.rotate.y = Lerp(attackerArrowModel_->param_->modelTransform.rotate.y, radian, 10.0f * kDt);

		// 攻撃者が一定距離以上離れている場合は矢印を白色にする
		attackerArrowModel_->param_->meshMaterial[0].color = Lerp(attackerArrowModel_->param_->meshMaterial[0].color, Vector4(1.0f, 1.0f, 1.0f, 1.0f), 10.0f * kDt);
	}
	else
	{
		// 攻撃者がいない場合は矢印を非表示にする
		attackerArrowModel_->param_->meshMaterial[0].color = Lerp(attackerArrowModel_->param_->meshMaterial[0].color, Vector4(1.0f, 1.0f, 1.0f, 0.0f), 10.0f * kDt);
	}


	// AI計測用エディタの更新
	aiMetricsEditor_->Update(kDt);
}

/// @brief 描画処理
void GameScene::Draw()
{
	// 攻撃者の方向を示す矢印の描画レンダーパスを呼び出す
	engine_->ExecuteRenderPass("AttackerArrow");

	// オブジェクトの描画レンダーパスを呼び出す
	engine_->ExecuteRenderPass("Object");

	// ポストエフェクトの描画レンダーパスを呼び出す
	engine_->ExecuteRenderPass("PostEffect");

	// HUDの描画レンダーパスを呼び出す
	engine_->ExecuteRenderPass("HUD");

	// 描画後処理のレンダーパスを呼び出す
	engine_->ExecuteRenderPass("MainPass");
}

/// @brief ステージがロードされたときの処理
/// @param fileName 
void GameScene::OnStageLoaded(const std::string& fileName)
{
	std::string baseName = fileName;

	// 拡張子を削除する
	size_t extPos = baseName.find(".json");
	if (extPos != std::string::npos)
		baseName.erase(extPos, 5);

	// 他のエディタのロードを呼び出す
	if (modelEditor_) 
		modelEditor_->Load(baseName);

	if (lightEditor_)
		lightEditor_->Load(baseName);
}

/// @brief キャラクターを生成する
/// @param initData 
/// @return 
Character* GameScene::CreateCharacter(const CharacterInitData& initData, CharacterTag tag, 
	const BehaviorTreeConfig& behaviorTreeConfig, const ComboTreeConfig& comboTreeConfig, const std::string& editorName)
{
	Character* character = nullptr;

	if (tag == CharacterTag::Player)
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

		// すでにプレイヤーのレイジゲージが存在する場合は削除する
		if (playerRageGage_)
		{
			playerRageGage_.reset();
			playerRageGage_ = nullptr;
		}

		// すでに武器の耐久力ゲージが存在する場合は削除する
		if (weaponDurabilityGage_)
		{
			weaponDurabilityGage_.reset();
			weaponDurabilityGage_ = nullptr;
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
		hpInitData.hpSeparatorSprite = hpSeparatorSprite_->CreateInstance();
		hpInitData.alpha = 1.0f;
		hpInitData.scale = Vector2(0.5f, 0.5f);
		hpInitData.width = 1200;
		hpInitData.color = Vector3(0.25f, 1.0f, 0.25f);
		playerHP_ = std::make_unique<HP>();
		playerHP_->Initialize(hpInitData);

		// レイジゲージの生成と初期化
		Gage::InitData rageGageInitData;
		rageGageInitData.position = Vector2(0.0f, 0.0f);
		rageGageInitData.hpFrameLeftSprite = hpFrameLeftSprite_->CreateInstance();
		rageGageInitData.hpFrameMiddleSprite = hpFrameMiddleSprite_->CreateInstance();
		rageGageInitData.hpFrameRightSprite = hpFrameRightSprite_->CreateInstance();
		rageGageInitData.hpLeftSprite = hpLeftSprite_->CreateInstance();
		rageGageInitData.hpMiddleSprite = hpMiddleSprite_->CreateInstance();
		rageGageInitData.hpRightSprite = hpRightSprite_->CreateInstance();
		rageGageInitData.hpSeparatorSprite = hpSeparatorSprite_->CreateInstance();
		rageGageInitData.alpha = 1.0f;
		rageGageInitData.scale = Vector2(0.35f, 0.35f);
		rageGageInitData.width = 800;
		rageGageInitData.color = Vector3(0.25f, 0.25f, 1.0f);
		playerRageGage_ = std::make_unique<Gage>();
		playerRageGage_->Initialize(rageGageInitData);

		// 武器耐久ゲージの生成と初期化
		Gage::InitData weaponDurabilityGageInitData;
		weaponDurabilityGageInitData.position = Vector2(0.0f, 0.0f);
		weaponDurabilityGageInitData.hpFrameLeftSprite = hpFrameLeftSprite_->CreateInstance();
		weaponDurabilityGageInitData.hpFrameMiddleSprite = hpFrameMiddleSprite_->CreateInstance();
		weaponDurabilityGageInitData.hpFrameRightSprite = hpFrameRightSprite_->CreateInstance();
		weaponDurabilityGageInitData.hpLeftSprite = hpLeftSprite_->CreateInstance();
		weaponDurabilityGageInitData.hpMiddleSprite = hpMiddleSprite_->CreateInstance();
		weaponDurabilityGageInitData.hpRightSprite = hpRightSprite_->CreateInstance();
		weaponDurabilityGageInitData.hpSeparatorSprite = hpSeparatorSprite_->CreateInstance();
		weaponDurabilityGageInitData.alpha = 0.0f;
		weaponDurabilityGageInitData.scale = Vector2(0.3f, 0.3f);
		weaponDurabilityGageInitData.width = 300;
		weaponDurabilityGageInitData.color = Vector3(1.0f, 1.0f, 1.0f);
		weaponDurabilityGage_ = std::make_unique<Gage>();
		weaponDurabilityGage_->Initialize(weaponDurabilityGageInitData);


		// プレイヤーの生成処理
		CharacterInitData playerInitData = initData;
		playerInitData.hurtboxGroup = playerHurtboxGroup_.get();
		playerInitData.hitboxGroup = playerHitboxGroup_.get();
		playerInitData.landingCollision = landingCollision_->CreateInstance();
		playerInitData.wallTouchCollision = wallTouchCollision_->CreateInstance();
		playerInitData.eventTriggerCollision = eventTriggerCollision_->CreateInstance();
		playerInitData.model_ = playerModel_.get();
		playerInitData.attackTrail = playerTrail_.get();
		playerInitData.hpHUD = playerHP_.get();
		playerInitData.rageGageThresholds = { 20.0f };
		player_ = std::make_unique<Player>();
		player_->InitComboTree(comboTreeConfig);
		player_->SetEditorName(editorName);
		player_->Initialize(playerInitData, playerWeapon_.get());
		player_->SetRageGageHud(playerRageGage_.get());
		player_->SetWeaponHpGageHud(weaponDurabilityGage_.get(), weaponKnifeSprite_, weaponGunSprite_);

		character = player_.get();


		// カメラ制御の初期化
		InitializeCameraControl();
	}
	else
	{
		CharacterInitData npcInitData = initData;

		// NPCのモデルの生成と初期化
		std::unique_ptr<Render3DSkinningModel> npcModel = npcModelPool_->Acquire();
		npcModel->param_->isUpdate = true;

		if (tag == CharacterTag::EnemyNormal)
		{
			npcModel->param_->meshOutline[0].enableOutline = true;
			npcModel->param_->meshOutline[0].color = Vector4(0.6f, 0.0f, 0.0f, 1.0f);
		} 
		else if (tag == CharacterTag::EnemyBoss)
		{
			npcModel->param_->meshOutline[0].enableOutline = true;
			npcModel->param_->meshOutline[0].color = Vector4(0.6f, 0.0f, 1.0f, 1.0f);
		} 
		else if (tag == CharacterTag::Ally)
		{
			npcModel->param_->meshOutline[0].enableOutline = true;
			npcModel->param_->meshOutline[0].color = Vector4(0.1f, 0.1f, 0.1f, 1.0f);
		}
		else if (tag == CharacterTag::Vip)
		{
			npcModel->param_->meshOutline[0].enableOutline = true;
			npcModel->param_->meshOutline[0].color = Vector4(1.0f, 1.0f, 0.0f, 1.0f);
		}

		// NPCのトレイルの生成と初期化
		std::unique_ptr<Trail3D> npcTrail = npcTrailPool_->Acquire();
		npcTrail->param_->isUpdate_ = true;

		// NPCのモデルを初期化データに設定する
		npcInitData.model_ = npcModel.get();
		npcInitData.attackTrail = npcTrail.get();

		// NPCの当たり判定グループの設定
		if (tag == CharacterTag::Ally || tag == CharacterTag::Vip)
		{
			npcInitData.hurtboxGroup = playerHurtboxGroup_.get();
			npcInitData.hitboxGroup = playerHitboxGroup_.get();
		}
		else if (tag == CharacterTag::EnemyNormal || tag == CharacterTag::EnemyBoss)
		{
			npcInitData.hurtboxGroup = enemyHurtboxGroup_.get();
			npcInitData.hitboxGroup = enemyHitboxGroup_.get();
		}

		// 着地判定グループの設定
		npcInitData.landingCollision = landingCollision_->CreateInstance();
		npcInitData.wallTouchCollision = wallTouchCollision_->CreateInstance();

		if (tag == CharacterTag::EnemyBoss)
		{
			BossHP::InitData bossHpInitData;
			bossHpInitData.position = Vector2(0.0f, 0.0f);
			bossHpInitData.hpFrameLeftSprite = hpFrameLeftSprite_->CreateInstance();
			bossHpInitData.hpFrameMiddleSprite = hpFrameMiddleSprite_->CreateInstance();
			bossHpInitData.hpFrameRightSprite = hpFrameRightSprite_->CreateInstance();
			bossHpInitData.hpFrontLeftSprite = hpLeftSprite_->CreateInstance();
			bossHpInitData.hpFrontMiddleSprite = hpMiddleSprite_->CreateInstance();
			bossHpInitData.hpFrontRightSprite = hpRightSprite_->CreateInstance();
			bossHpInitData.hpBackLeftSprite = hpBackLeftSprite_->CreateInstance();
			bossHpInitData.hpBackMiddleSprite = hpBackMiddleSprite_->CreateInstance();
			bossHpInitData.hpBackRightSprite = hpBackRightSprite_->CreateInstance();
			bossHpInitData.delayHpBackLeftSprite = delayHpLeftSprite_->CreateInstance();
			bossHpInitData.delayHpBackMiddleSprite = delayHpMiddleSprite_->CreateInstance();
			bossHpInitData.delayHpBackRightSprite = delayHpRightSprite_->CreateInstance();
			bossHpInitData.delayHpFrontLeftSprite = delayHpFrontLeftSprite_->CreateInstance();
			bossHpInitData.delayHpFrontMiddleSprite = delayHpFrontMiddleSprite_->CreateInstance();
			bossHpInitData.delayHpFrontRightSprite = delayHpFrontRightSprite_->CreateInstance();
			bossHpInitData.hpSeparatorSprite = hpSeparatorSprite_->CreateInstance();
			bossHpInitData.alpha = 1.0f;
			bossHpInitData.scale = Vector2(0.45f, 0.45f);
			bossHpInitData.width = 300;

			std::unique_ptr<BossHP> hp = std::make_unique<BossHP>();
			hp->Initialize(bossHpInitData);
			npcInitData.hpHUD = hp.get();

			huds_.push_back(std::move(hp));
		}
		else
		{
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
			hpInitData.hpSeparatorSprite = hpSeparatorSprite_->CreateInstance();
			hpInitData.alpha = 1.0f;

			if (tag == CharacterTag::Ally)
			{
				hpInitData.scale = Vector2(0.75f, 0.75f);
				hpInitData.width = 150;
				hpInitData.color = Vector3(0.25f, 1.0f, 0.25f);
			}
			else if (tag == CharacterTag::EnemyNormal)
			{
				hpInitData.scale = Vector2(0.25f, 0.25f);
				hpInitData.width = 200;
				hpInitData.color = Vector3(1.0f, 0.25f, 0.25f);
			}


			std::unique_ptr<HP> hp = std::make_unique<HP>();
			hp->Initialize(hpInitData);
			npcInitData.hpHUD = hp.get();

			huds_.push_back(std::move(hp));
		}

		// NPCの生成処理
		std::unique_ptr<NPC> npc = npcPool_->Acquire();
		npc->InitBehaviorTree(behaviorTreeConfig, behaviorTreeEditor_.get());
		npc->Initialize(npcInitData, tag, navMesh_.get());
		npc->SetEditorName(editorName);
		character = npc.get();

		// NPCのリストに追加する
		npcs_.push_back(std::move(npc));
		npcModels_.push_back(std::move(npcModel));
		npcTrails_.push_back(std::move(npcTrail));
	}

	return character;
}

/// @brief 武器を生成する
/// @param position 
/// @return 
Weapon* GameScene::CreateWeapon(const Weapon::InitData& initData, const BehaviorTreeConfig& behaviorTreeConfig, const ComboTreeConfig& comboTreeConfig)
{
	Weapon* weapon = nullptr;

	WeaponGetButton::InitData buttonInitData;
	buttonInitData.buttonSprite = weaponGetButtonSpritePrefab_->CreateInstance();
	std::unique_ptr<WeaponGetButton> button = std::make_unique<WeaponGetButton>(buttonInitData);

	Weapon::InitData weaponInitData = initData;
	weaponInitData.landingCollision = landingCollision_->CreateInstance();
	weaponInitData.model = oneHandedWeaponModel_->CreateInstance();
	weaponInitData.button = button.get();

	// 武器の生成処理
	std::unique_ptr<Weapon> newWeapon = std::make_unique<Weapon>(weaponInitData);
	weapon = newWeapon.get();

	// 通常状態のステートツリーを設定する
	WeaponStateTreeSet noneStateTreeSet;
	noneStateTreeSet.behaviorTree = behaviorTreeEditor_->CreateTree(behaviorTreeConfig.noneStateBT.c_str(), nullptr);
	noneStateTreeSet.comboTreeX = ComboTreeFactory::CreateTree(comboTreeConfig.noneStateCT.xName_.c_str(), nullptr);
	noneStateTreeSet.comboTreeY = ComboTreeFactory::CreateTree(comboTreeConfig.noneStateCT.yName_.c_str(), nullptr);
	noneStateTreeSet.comboTreeB = ComboTreeFactory::CreateTree(comboTreeConfig.noneStateCT.bName_.c_str(), nullptr);
	newWeapon->SetStateTreeSet("None", noneStateTreeSet);


	// ダッシュ状態のステートツリーを設定する
	WeaponStateTreeSet dashStateTreeSet;
	dashStateTreeSet.behaviorTree = behaviorTreeEditor_->CreateTree(behaviorTreeConfig.dashStateBT.c_str(), nullptr);
	dashStateTreeSet.comboTreeX = ComboTreeFactory::CreateTree(comboTreeConfig.dashStateCT.xName_.c_str(), nullptr);
	dashStateTreeSet.comboTreeY = ComboTreeFactory::CreateTree(comboTreeConfig.dashStateCT.yName_.c_str(), nullptr);
	dashStateTreeSet.comboTreeB = ComboTreeFactory::CreateTree(comboTreeConfig.dashStateCT.bName_.c_str(), nullptr);
	newWeapon->SetStateTreeSet("Dash", dashStateTreeSet);

	// 掴まれ状態のステートツリーを設定する
	WeaponStateTreeSet grabbedStateTreeSet;
	grabbedStateTreeSet.behaviorTree = behaviorTreeEditor_->CreateTree(behaviorTreeConfig.grabbedStateBT.c_str(), nullptr);
	grabbedStateTreeSet.comboTreeX = ComboTreeFactory::CreateTree(comboTreeConfig.grabbedStateCT.xName_.c_str(), nullptr);
	grabbedStateTreeSet.comboTreeY = ComboTreeFactory::CreateTree(comboTreeConfig.grabbedStateCT.yName_.c_str(), nullptr);
	grabbedStateTreeSet.comboTreeB = ComboTreeFactory::CreateTree(comboTreeConfig.grabbedStateCT.bName_.c_str(), nullptr);
	newWeapon->SetStateTreeSet("Grabbed", grabbedStateTreeSet);

	// 掴み状態のステートツリーを設定する
	WeaponStateTreeSet grabbingStateTreeSet;
	grabbingStateTreeSet.behaviorTree = behaviorTreeEditor_->CreateTree(behaviorTreeConfig.grabbingStateBT.c_str(), nullptr);
	grabbingStateTreeSet.comboTreeX = ComboTreeFactory::CreateTree(comboTreeConfig.grabbingStateCT.xName_.c_str(), nullptr);
	grabbingStateTreeSet.comboTreeY = ComboTreeFactory::CreateTree(comboTreeConfig.grabbingStateCT.yName_.c_str(), nullptr);
	grabbingStateTreeSet.comboTreeB = ComboTreeFactory::CreateTree(comboTreeConfig.grabbingStateCT.bName_.c_str(), nullptr);
	newWeapon->SetStateTreeSet("Grabbing", grabbingStateTreeSet);

	// ガード状態のステートツリーを設定する
	WeaponStateTreeSet guardStateTreeSet;
	guardStateTreeSet.behaviorTree = behaviorTreeEditor_->CreateTree(behaviorTreeConfig.guardStateBT.c_str(), nullptr);
	guardStateTreeSet.comboTreeX = ComboTreeFactory::CreateTree(comboTreeConfig.guardStateCT.xName_.c_str(), nullptr);
	guardStateTreeSet.comboTreeY = ComboTreeFactory::CreateTree(comboTreeConfig.guardStateCT.yName_.c_str(), nullptr);
	guardStateTreeSet.comboTreeB = ComboTreeFactory::CreateTree(comboTreeConfig.guardStateCT.bName_.c_str(), nullptr);
	newWeapon->SetStateTreeSet("Guard", guardStateTreeSet);

	// 軽ダメージ状態のステートツリーを設定する
	WeaponStateTreeSet lightDamageStateTreeSet;
	lightDamageStateTreeSet.behaviorTree = behaviorTreeEditor_->CreateTree(behaviorTreeConfig.lightDamageStateBT.c_str(), nullptr);
	lightDamageStateTreeSet.comboTreeX = ComboTreeFactory::CreateTree(comboTreeConfig.lightDamageStateCT.xName_.c_str(), nullptr);
	lightDamageStateTreeSet.comboTreeY = ComboTreeFactory::CreateTree(comboTreeConfig.lightDamageStateCT.yName_.c_str(), nullptr);
	lightDamageStateTreeSet.comboTreeB = ComboTreeFactory::CreateTree(comboTreeConfig.lightDamageStateCT.bName_.c_str(), nullptr);
	newWeapon->SetStateTreeSet("LightDamage", lightDamageStateTreeSet);

	// 重ダメージ状態のステートツリーを設定する
	WeaponStateTreeSet heavyDamageStateTreeSet;
	heavyDamageStateTreeSet.behaviorTree = behaviorTreeEditor_->CreateTree(behaviorTreeConfig.heavyDamageStateBT.c_str(), nullptr);
	heavyDamageStateTreeSet.comboTreeX = ComboTreeFactory::CreateTree(comboTreeConfig.heavyDamageStateCT.xName_.c_str(), nullptr);
	heavyDamageStateTreeSet.comboTreeY = ComboTreeFactory::CreateTree(comboTreeConfig.heavyDamageStateCT.yName_.c_str(), nullptr);
	heavyDamageStateTreeSet.comboTreeB = ComboTreeFactory::CreateTree(comboTreeConfig.heavyDamageStateCT.bName_.c_str(), nullptr);
	newWeapon->SetStateTreeSet("HeavyDamage", heavyDamageStateTreeSet);

	// 倒れこみ状態のステートツリーを設定する
	WeaponStateTreeSet downFallingStateTreeSet;
	downFallingStateTreeSet.behaviorTree = behaviorTreeEditor_->CreateTree(behaviorTreeConfig.downFallingStateBT.c_str(), nullptr);
	downFallingStateTreeSet.comboTreeX = ComboTreeFactory::CreateTree(comboTreeConfig.downFallingStateCT.xName_.c_str(), nullptr);
	downFallingStateTreeSet.comboTreeY = ComboTreeFactory::CreateTree(comboTreeConfig.downFallingStateCT.yName_.c_str(), nullptr);
	downFallingStateTreeSet.comboTreeB = ComboTreeFactory::CreateTree(comboTreeConfig.downFallingStateCT.bName_.c_str(), nullptr);
	newWeapon->SetStateTreeSet("DownFalling", downFallingStateTreeSet);

	// ダウン状態のステートツリーを設定する
	WeaponStateTreeSet downLyingStateTreeSet;
	downLyingStateTreeSet.behaviorTree = behaviorTreeEditor_->CreateTree(behaviorTreeConfig.downLyingStateBT.c_str(), nullptr);
	downLyingStateTreeSet.comboTreeX = ComboTreeFactory::CreateTree(comboTreeConfig.downLyingStateCT.xName_.c_str(), nullptr);
	downLyingStateTreeSet.comboTreeY = ComboTreeFactory::CreateTree(comboTreeConfig.downLyingStateCT.yName_.c_str(), nullptr);
	downLyingStateTreeSet.comboTreeB = ComboTreeFactory::CreateTree(comboTreeConfig.downLyingStateCT.bName_.c_str(), nullptr);
	newWeapon->SetStateTreeSet("DownLying", downLyingStateTreeSet);

	// 起き上がり状態のステートツリーを設定する
	WeaponStateTreeSet downGettingUpStateTreeSet;
	downGettingUpStateTreeSet.behaviorTree = behaviorTreeEditor_->CreateTree(behaviorTreeConfig.downGettingUpStateBT.c_str(), nullptr);
	downGettingUpStateTreeSet.comboTreeX = ComboTreeFactory::CreateTree(comboTreeConfig.downGettingUpStateCT.xName_.c_str(), nullptr);
	downGettingUpStateTreeSet.comboTreeY = ComboTreeFactory::CreateTree(comboTreeConfig.downGettingUpStateCT.yName_.c_str(), nullptr);
	downGettingUpStateTreeSet.comboTreeB = ComboTreeFactory::CreateTree(comboTreeConfig.downGettingUpStateCT.bName_.c_str(), nullptr);
	newWeapon->SetStateTreeSet("DownGettingUp", downGettingUpStateTreeSet);

	// ダウン怯み状態のステートツリーを設定する
	WeaponStateTreeSet downStaggerStateTreeSet;
	downStaggerStateTreeSet.behaviorTree = behaviorTreeEditor_->CreateTree(behaviorTreeConfig.downStaggerStateBT.c_str(), nullptr);
	downStaggerStateTreeSet.comboTreeX = ComboTreeFactory::CreateTree(comboTreeConfig.downStaggerStateCT.xName_.c_str(), nullptr);
	downStaggerStateTreeSet.comboTreeY = ComboTreeFactory::CreateTree(comboTreeConfig.downStaggerStateCT.yName_.c_str(), nullptr);
	downStaggerStateTreeSet.comboTreeB = ComboTreeFactory::CreateTree(comboTreeConfig.downStaggerStateCT.bName_.c_str(), nullptr);
	newWeapon->SetStateTreeSet("DownStagger", downStaggerStateTreeSet);

	// 吹き飛び状態のステートツリーを設定する
	WeaponStateTreeSet blownAwayStateTreeSet;
	blownAwayStateTreeSet.behaviorTree = behaviorTreeEditor_->CreateTree(behaviorTreeConfig.blownAwayStateBT.c_str(), nullptr);
	blownAwayStateTreeSet.comboTreeX = ComboTreeFactory::CreateTree(comboTreeConfig.blownAwayStateCT.xName_.c_str(), nullptr);
	blownAwayStateTreeSet.comboTreeY = ComboTreeFactory::CreateTree(comboTreeConfig.blownAwayStateCT.yName_.c_str(), nullptr);
	blownAwayStateTreeSet.comboTreeB = ComboTreeFactory::CreateTree(comboTreeConfig.blownAwayStateCT.bName_.c_str(), nullptr);
	newWeapon->SetStateTreeSet("BlownAway", blownAwayStateTreeSet);

	// 吹き飛び落下状態のステートツリーを設定する
	WeaponStateTreeSet blownFallingStateTreeSet;
	blownFallingStateTreeSet.behaviorTree = behaviorTreeEditor_->CreateTree(behaviorTreeConfig.blownFallingStateBT.c_str(), nullptr);
	blownFallingStateTreeSet.comboTreeX = ComboTreeFactory::CreateTree(comboTreeConfig.blownFallingStateCT.xName_.c_str(), nullptr);
	blownFallingStateTreeSet.comboTreeY = ComboTreeFactory::CreateTree(comboTreeConfig.blownFallingStateCT.yName_.c_str(), nullptr);
	blownFallingStateTreeSet.comboTreeB = ComboTreeFactory::CreateTree(comboTreeConfig.blownFallingStateCT.bName_.c_str(), nullptr);
	newWeapon->SetStateTreeSet("BlownFalling", blownFallingStateTreeSet);

	// 弾き状態のステートツリーを設定する
	WeaponStateTreeSet repelStateTreeSet;
	repelStateTreeSet.behaviorTree = behaviorTreeEditor_->CreateTree(behaviorTreeConfig.repelStateBT.c_str(), nullptr);
	repelStateTreeSet.comboTreeX = ComboTreeFactory::CreateTree(comboTreeConfig.repelStateCT.xName_.c_str(), nullptr);
	repelStateTreeSet.comboTreeY = ComboTreeFactory::CreateTree(comboTreeConfig.repelStateCT.yName_.c_str(), nullptr);
	repelStateTreeSet.comboTreeB = ComboTreeFactory::CreateTree(comboTreeConfig.repelStateCT.bName_.c_str(), nullptr);
	newWeapon->SetStateTreeSet("Repel", repelStateTreeSet);

	// 受け流し状態のステートツリーを設定する
	WeaponStateTreeSet deflectStateTreeSet;
	deflectStateTreeSet.behaviorTree = behaviorTreeEditor_->CreateTree(behaviorTreeConfig.deflectStateBT.c_str(), nullptr);
	deflectStateTreeSet.comboTreeX = ComboTreeFactory::CreateTree(comboTreeConfig.deflectStateCT.xName_.c_str(), nullptr);
	deflectStateTreeSet.comboTreeY = ComboTreeFactory::CreateTree(comboTreeConfig.deflectStateCT.yName_.c_str(), nullptr);
	deflectStateTreeSet.comboTreeB = ComboTreeFactory::CreateTree(comboTreeConfig.deflectStateCT.bName_.c_str(), nullptr);
	newWeapon->SetStateTreeSet("Deflect", deflectStateTreeSet);

	// 弾かれ状態のステートツリーを設定する
	WeaponStateTreeSet repelledStateTreeSet;
	repelledStateTreeSet.behaviorTree = behaviorTreeEditor_->CreateTree(behaviorTreeConfig.repelledStateBT.c_str(), nullptr);
	repelledStateTreeSet.comboTreeX = ComboTreeFactory::CreateTree(comboTreeConfig.repelledStateCT.xName_.c_str(), nullptr);
	repelledStateTreeSet.comboTreeY = ComboTreeFactory::CreateTree(comboTreeConfig.repelledStateCT.yName_.c_str(), nullptr);
	repelledStateTreeSet.comboTreeB = ComboTreeFactory::CreateTree(comboTreeConfig.repelledStateCT.bName_.c_str(), nullptr);
	newWeapon->SetStateTreeSet("Repelled", repelledStateTreeSet);

	// 受け流され状態のステートツリーを設定する
	WeaponStateTreeSet deflectedStateTreeSet;
	deflectedStateTreeSet.behaviorTree = behaviorTreeEditor_->CreateTree(behaviorTreeConfig.deflectedStateBT.c_str(), nullptr);
	deflectedStateTreeSet.comboTreeX = ComboTreeFactory::CreateTree(comboTreeConfig.deflectedStateCT.xName_.c_str(), nullptr);
	deflectedStateTreeSet.comboTreeY = ComboTreeFactory::CreateTree(comboTreeConfig.deflectedStateCT.yName_.c_str(), nullptr);
	deflectedStateTreeSet.comboTreeB = ComboTreeFactory::CreateTree(comboTreeConfig.deflectedStateCT.bName_.c_str(), nullptr);
	newWeapon->SetStateTreeSet("Deflected", deflectedStateTreeSet);

	// 回避状態のステートツリーを設定する
	WeaponStateTreeSet avoidStateTreeSet;
	avoidStateTreeSet.behaviorTree = behaviorTreeEditor_->CreateTree(behaviorTreeConfig.avoidStateBT.c_str(), nullptr);
	avoidStateTreeSet.comboTreeX = ComboTreeFactory::CreateTree(comboTreeConfig.avoidStateCT.xName_.c_str(), nullptr);
	avoidStateTreeSet.comboTreeY = ComboTreeFactory::CreateTree(comboTreeConfig.avoidStateCT.yName_.c_str(), nullptr);
	avoidStateTreeSet.comboTreeB = ComboTreeFactory::CreateTree(comboTreeConfig.avoidStateCT.bName_.c_str(), nullptr);
	newWeapon->SetStateTreeSet("Avoid", avoidStateTreeSet);

	// 死亡状態のステートツリーを設定する
	WeaponStateTreeSet deadStateTreeSet;
	deadStateTreeSet.behaviorTree = behaviorTreeEditor_->CreateTree(behaviorTreeConfig.deadStateBT.c_str(), nullptr);
	deadStateTreeSet.comboTreeX = ComboTreeFactory::CreateTree(comboTreeConfig.deadStateCT.xName_.c_str(), nullptr);
	deadStateTreeSet.comboTreeY = ComboTreeFactory::CreateTree(comboTreeConfig.deadStateCT.yName_.c_str(), nullptr);
	deadStateTreeSet.comboTreeB = ComboTreeFactory::CreateTree(comboTreeConfig.deadStateCT.bName_.c_str(), nullptr);
	newWeapon->SetStateTreeSet("Dead", deadStateTreeSet);

	// 武器のリストに追加する
	weapons_.push_back(std::move(newWeapon));

	// HUDのリストに追加する
	huds_.push_back(std::move(button));

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
	triggerInitData.onTriggerCallback = [this](int eventType, const char* param, bool isStartBattleArea, bool isGameClear, int navMeshGroupId, bool isNavMeshEnabled) -> bool
		{
			return HandleTriggerEvent(eventType, param, isStartBattleArea, isGameClear, navMeshGroupId, isNavMeshEnabled); 
		};

	std::unique_ptr<StaticEventTrigger> newTrigger = std::make_unique<StaticEventTrigger>();
	newTrigger->Initialize(triggerInitData);
	StaticEventTrigger* trigger = newTrigger.get();

	objects_.push_back(std::move(newTrigger));

	return trigger;
}

/// @brief カメラガードオブジェクトを生成する
/// @param initData 
/// @return 
CameraGuard* GameScene::CreateCameraGuard(const CameraGuard::InitData& initData)
{
	CameraGuard::InitData guardInitData = initData;
	guardInitData.collision = cameraGuardCollision_->CreateInstance();

	std::unique_ptr<CameraGuard> newGuard = std::make_unique<CameraGuard>();
	newGuard->Initialize(guardInitData);
	CameraGuard* guard = newGuard.get();

	objects_.push_back(std::move(newGuard));

	return guard;
}

/// @brief リセットする
void GameScene::Reset()
{
	// プレイヤーをリセットする
	if (player_)player_ = nullptr;

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

	// ピボットポイントの初期化
	Vector3 targetPivotPos = player_->GetWorldPosition();
	targetPivotPos += kPivotCenterOffset;

	pivotPoint_->GetData()->center = targetPivotPos;
	pivotPoint_->GetData()->radius = 8.0f;
	pivotPoint_->GetData()->phi = -std::numbers::pi_v<float> / 2.0f;
	pivotPoint_->GetData()->theta = 0.0f;

	// カメラ回転入力の生成
	inputCameraRotate_ = std::make_unique<InputGamepadRightStick>("Camera_Rotate", InputState::Press, 0, Vector2(0.0f, 0.0f), 0.5f);

	// 補間係数を初期化する
	cameraCurrentT_ = 1.0f;

	pivotPoint_->Update();

	PivotPoint::Data* pivotData = pivotPoint_->GetData();

	// カメラ座標の即時適用
	mainCamera_->param_->transform.translate = pivotData->sphericalCoordinates;

	// カメラ回転（オイラー角）の即時適用
	const Vector3 kLookDirection = pivotData->toCenter;
	const float kYaw = std::atan2(kLookDirection.x, kLookDirection.z);
	const float kHorizontal = std::sqrt(kLookDirection.x * kLookDirection.x + kLookDirection.z * kLookDirection.z);
	const float kPitch = std::atan2(-kLookDirection.y, kHorizontal);
	mainCamera_->param_->transform.rotate = Vector3(kPitch, kYaw, 0.0f);

	// 次フレームの当たり判定（カメラセグメント）用に初期値をセット
	cameraSegmentInstance_->param_->start = pivotData->center;
	cameraSegmentInstance_->param_->diff = pivotData->sphericalCoordinates - pivotData->center;

	// バトル制御用（カメラの前方向）の即時適用
	Vector3 cameraForward = kLookDirection;
	cameraForward.y = 0.0f;
	if (cameraForward.LengthSq() > 0.0f)
		cameraForward = cameraForward.Normalize();

	// バトル制御用（カメラの前方向）の即時適用
	BattleDirector::GetInstance().SetCameraForward(cameraForward);
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

	if (deltaTime > 0.1f) {
		deltaTime = 0.1f;
	}

	// ピボット中心の追従更新
	UpdatePivotFollow(deltaTime);

	// ピボット回転入力の更新
	UpdatePivotRotateInput(deltaTime);

	// ピボットの球面座標と注視方向を更新する
	pivotPoint_->Update();

	// ピボットからカメラ姿勢を更新する
	ApplyCameraFromPivot(deltaTime);
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
	const bool kIsKeyCameraRotate =
		engine_->GetKeyPress(DIK_LEFT) || engine_->GetKeyPress(DIK_RIGHT) ||
		engine_->GetKeyPress(DIK_DOWN) || engine_->GetKeyPress(DIK_UP);

	// ゲームパッドの右スティック入力を取得する
	Vector2 rightStick(0.0f, 0.0f);
	if (inputCameraRotate_ && inputCameraRotate_->param_)
	{
		rightStick = engine_->GetGamepadRightStick(inputCameraRotate_->param_->controller);
	}

	// キー入力または右スティック入力がある場合は手動でカメラ回転しているとみなす
	bool isManualCameraControl = kIsKeyCameraRotate || (rightStick.Length() > 0.01f);
	
	// 手動でカメラ回転入力がある場合はピボットを回転させる
	if (isManualCameraControl)
	{
		// プレイヤーに手動でカメラ回転していることを通知する
		player_->SetIsOperationCamera(true);

		// 手動でカメラ回転入力がある場合はピボットを回転させる
		if (!kIsKeyCameraRotate)
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
void GameScene::ApplyCameraFromPivot(float deltaTime)
{
	// プレイヤーがいない場合は更新しない
	if (!player_)return;

	// ピボットのデータを取得する
	PivotPoint::Data* pivotData = pivotPoint_->GetData();

	// カメラの位置をピボットの球面座標から計算する
	Vector3 finalCameraPos = pivotData->sphericalCoordinates;

	// カメラセグメントのパラメータを更新する
	cameraSegmentInstance_->param_->start = pivotData->center;
	cameraSegmentInstance_->param_->diff = finalCameraPos - pivotData->center;

	// 係数Tの初期値を1.0fに設定する 
	float targetT = 1.0f;

	// カメラセグメントの衝突判定が有効な場合は、カメラの位置を調整する
	if (cameraSegmentInstance_->isCollision_)
	{
		// 複数の壁（OBB）と衝突している可能性があるため、最も近い交点を探す
		for (auto* opponent : cameraSegmentInstance_->hitOpponents_)
		{
			auto* obb = dynamic_cast<Collision3DInstanceOBB*>(opponent);
			float t = Engine::IntersectSegmentOBB(*cameraSegmentInstance_->param_, *obb->param_);

			// 最もピボット中心に近い交点を採用する
			if (t >= 0.0f && t < targetT)
			{
				targetT = t;
			}
		}
	}

	// カメラの位置を補間する速度を設定する
	constexpr float kCameraShrinkSpeed = 25.0f; // 壁に近づくときの速度
	constexpr float kCameraExpandSpeed = 5.0f;  // 元の位置に戻るときの速度

	// 現在よりターゲットが近い（縮む）か、遠い（戻る）かで速度を分岐
	float lerpSpeed = (targetT < cameraCurrentT_) ? kCameraShrinkSpeed : kCameraExpandSpeed;

	// 線形補間（Lerp）で現在のTを目標のTへ近づける
	cameraCurrentT_ += (targetT - cameraCurrentT_) * lerpSpeed * deltaTime;
	cameraCurrentT_ = std::clamp(cameraCurrentT_, 0.0f, 1.0f);

	// 補間された割合を使って、最終的なカメラ位置を計算
	Vector3 hitPosition = pivotData->center + (cameraSegmentInstance_->param_->diff * cameraCurrentT_);

	// 壁に近づいている（Tが1.0未満）場合はニアクリップ対策のオフセットを適用
	if (cameraCurrentT_ < 0.99f)
	{
		constexpr float kCameraOffset = 0.2f;
		Vector3 dir = cameraSegmentInstance_->param_->diff;
		dir = dir.Normalize();
		finalCameraPos = hitPosition - (dir * kCameraOffset);
	}
	else
	{
		finalCameraPos = hitPosition;
	}

	// カメラシェイクのオフセットを加算する
	if (cameraShake_) finalCameraPos += cameraShake_->GetShakeOffset();

	// カメラの位置を更新する
	mainCamera_->param_->transform.translate = finalCameraPos;

	// center方向を向くようにオイラー角を計算する
	const Vector3 kLookDirection = pivotData->toCenter;
	const float kYaw = std::atan2(kLookDirection.x, kLookDirection.z);
	const float kHorizontal = std::sqrt(kLookDirection.x * kLookDirection.x + kLookDirection.z * kLookDirection.z);
	const float kPitch = std::atan2(-kLookDirection.y, kHorizontal);
	mainCamera_->param_->transform.rotate = Vector3(kPitch, kYaw, 0.0f);

	// カメラの前方向ベクトルを計算する
	Vector3 cameraForward = kLookDirection;
	cameraForward.y = 0.0f; // 高さの影響をなくすためにYを0にする
	if (cameraForward.LengthSq() > 0.0f)
		cameraForward = cameraForward.Normalize();

	// バトル制御にカメラの前方向を通知する
	BattleDirector::GetInstance().SetCameraForward(cameraForward);
}

/// @brief ターゲットへの方向を取得する
/// @param character 
/// @return 
std::optional<Vector2> GameScene::GetToAttacker() const
{
	std::optional<Vector2> toAttacker;

	if (!player_) return toAttacker;

	// プレイヤーを攻撃しているキャラクターを取得する
	Character* attacker = player_->GetAttacker();
	if (!attacker) return toAttacker;

	// カメラの前方向と右方向を取得する（高さを無視）
	Vector3 forward = mainCamera_->GetDirection();
	forward = Vector3(forward.x, 0.0f, forward.z).Normalize();

	Vector3 right = Vector3(forward.z, 0.0f, -forward.x).Normalize();

	// プレイヤーから攻撃者へのベクトル（高さを無視）
	Vector3 toAttackerDirection = attacker->GetPosition() - player_->GetPosition();
	toAttackerDirection.y = 0.0f;
	float length = toAttackerDirection.Length();
	toAttackerDirection = toAttackerDirection.Normalize();

	// カメラの前方向と右方向に対する攻撃者の位置を計算する
	float dotRight = (toAttackerDirection.x * right.x) + (toAttackerDirection.z * right.z);   // X成分（左右）
	float dotForward = (toAttackerDirection.x * forward.x) + (toAttackerDirection.z * forward.z); // Y成分（前後）

	// ベクトルを作成する
	toAttacker = Vector2(dotRight, dotForward) * length;

	return toAttacker;
}

/// @brief イベントトリガーに触れたときの処理
/// @param eventType 
/// @param param 
/// @param isStartBattleArea 
/// @param isGameClear 
/// @param navMeshGroupId 
/// @param isNavMeshEnabled 
bool GameScene::HandleTriggerEvent(int eventType, const char* param, bool isStartBattleArea, bool isGameClear, int navMeshGroupId, bool isNavMeshEnabled)
{
	// イベントタイプを列挙型に変換する
	StaticEventTrigger::EventType type = static_cast<StaticEventTrigger::EventType>(eventType);

	if (type == StaticEventTrigger::EventType::None)
	{
		return true;
	}
	else if (type == StaticEventTrigger::EventType::ObjectSpawn)
	{
		try
		{
			// param が空文字列の場合は何もしない
			std::string fileName = param;
			if (fileName.empty()) return true;

			// JSONファイルのパスを作成する
			std::string filePath = "./Assets/Parameter/StageData/" + fileName + ".json";

			// ファイルストリームを開く
			std::ifstream ifs(filePath);
			if (!ifs.is_open())return false;

			// ファイルからJSONを読み込んで解析
			json j;
			ifs >> j;
			ifs.close();

			// JSON配列をループして、記述された各種オブジェクトを生成する
			if (j.contains("objects") && j["objects"].is_array())
			{
				if (isStartBattleArea)
				{
					std::unique_ptr<BattleArea> battleArea = std::make_unique<BattleArea>();
					battleArea->isGameClear = isGameClear;

					// 最後のバトルエリアであれば、BGMを切り替える
					if (isGameClear)
					{
						soundManager_->BgmTutorialRoadPlay(false);
						soundManager_->BgmTutorialBossPlay(true);

						// イントロテキストの生成
						IntroText::InitData introTextInitData;
						introTextInitData.buttonSprite = bossTextSprite_;
						std::unique_ptr<IntroText> introText = std::make_unique<IntroText>();
						introText->Initialize(introTextInitData);

						huds_.push_back(std::move(introText));
					}

					for (const auto& objectDataJson : j["objects"])
					{
						PlacementData initData;
						fromJson(objectDataJson, initData);

						// 解析したデータをもとにオブジェクトを生成する
						if (stageEditor_->SpawnObject(initData, battleArea.get()))
							stageEditor_->SetPlacementList(initData);
					}

					battleAreas_.push_back(std::move(battleArea));
				}
				else
				{
					for (const auto& objectDataJson : j["objects"])
					{
						PlacementData initData;
						fromJson(objectDataJson, initData);

						// 解析したデータをもとにオブジェクトを生成する
						if (stageEditor_->SpawnObject(initData))
							stageEditor_->SetPlacementList(initData);
					}
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
	}
	else if (type == StaticEventTrigger::EventType::PlayCutscene)
	{
		// すでにカットシーンが再生中の場合は何もしない
		if (cutsceneManager_->IsPlaying())
			return false;

		// キャラクターの操作を無効化する
		Character::SetIsCutsceneActive(true);

		// カットシーン用のカメラに切り替える
		cutsceneCamera_->Switch();

		// カットシーン名を取得する
		std::string cutsceneName = param;

		// カットシーン再生が終了したら、コールバックで元のカメラに戻し、プレイヤー操作を解放する
		cutsceneManager_->Play(cutsceneName, [this]()
			{
				// メインカメラに戻す
				mainCamera_->Switch();

				// キャラクターの操作を有効化する
				Character::SetIsCutsceneActive(false);
			}
		);

		return true; // トリガーを削除
	}
	else if (type == StaticEventTrigger::EventType::NavMeshStateChange)
	{
		if (navMesh_ != nullptr)
		{
			navMesh_->SetGroupActive(navMeshGroupId, isNavMeshEnabled);
		}

		return true; // トリガーを削除
	}
	else if (type == StaticEventTrigger::EventType::StickTutorial)
	{
		stickTutorial_->SetEnable(true);
	}
	else if (type == StaticEventTrigger::EventType::DashTutorial)
	{
		dashTutorial_->SetEnable(true);
	}
	else if (type == StaticEventTrigger::EventType::AttackTutorial)
	{
		attackTutorial_->SetEnable(true);
	}
	else if (type == StaticEventTrigger::EventType::ComboTutorial)
	{
		comboTutorial_->SetEnable(true);
	}
	else if (type == StaticEventTrigger::EventType::GrabTutorial)
	{
		if (player_->IsGrabbing())
		{
			attackTutorial_->SetEnable(true);
		}
		else
		{
			grabTutorial_->SetEnable(true);
		}
	}
	else if (type == StaticEventTrigger::EventType::GuardTutorial)
	{
		guardTutorial_->SetEnable(true);
	}
	else if (type == StaticEventTrigger::EventType::AvoidTutorial)
	{
		avoidTutorial_->SetEnable(true);
	}
	else if (type == StaticEventTrigger::EventType::RageModeTutorial)
	{
		rageTutorial_->SetEnable(true);
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

	// 後ろ側の体力バー
	hpBackLeftSprite_ = std::make_unique<PrefabBaseSprite>(engine_->LoadTexture("./Assets/Textures/hp_left.png"), 100, "HP_Back_Left_Sprite");
	hpBackLeftSprite_->param_->texture.anchor = Vector2(1.0f, 0.5f);

	hpBackMiddleSprite_ = std::make_unique<PrefabBaseSprite>(engine_->LoadTexture("./Assets/Textures/hp_middle.png"), 100, "HP_Back_Middle_Sprite");
	hpBackMiddleSprite_->param_->texture.anchor = Vector2(0.0f, 0.5f);

	hpBackRightSprite_ = std::make_unique<PrefabBaseSprite>(engine_->LoadTexture("./Assets/Textures/hp_right.png"), 100, "HP_Back_Right_Sprite");
	hpBackRightSprite_->param_->texture.anchor = Vector2(0.0f, 0.5f);

	// 遅延体力バー
	delayHpLeftSprite_ = std::make_unique<PrefabBaseSprite>(engine_->LoadTexture("./Assets/Textures/hp_left.png"), 100, "Delay_HP_Left_Sprite");
	delayHpLeftSprite_->param_->texture.anchor = Vector2(1.0f, 0.5f);

	delayHpMiddleSprite_ = std::make_unique<PrefabBaseSprite>(engine_->LoadTexture("./Assets/Textures/hp_middle.png"), 100, "Delay_HP_Middle_Sprite");
	delayHpMiddleSprite_->param_->texture.anchor = Vector2(0.0f, 0.5f);

	delayHpRightSprite_ = std::make_unique<PrefabBaseSprite>(engine_->LoadTexture("./Assets/Textures/hp_right.png"), 100, "Delay_HP_Right_Sprite");
	delayHpRightSprite_->param_->texture.anchor = Vector2(0.0f, 0.5f);

	// 前側の体力バー
	delayHpFrontLeftSprite_ = std::make_unique<PrefabBaseSprite>(engine_->LoadTexture("./Assets/Textures/hp_left.png"), 100, "Delay_HP_Front_Left_Sprite");
	delayHpFrontLeftSprite_->param_->texture.anchor = Vector2(1.0f, 0.5f);

	delayHpFrontMiddleSprite_ = std::make_unique<PrefabBaseSprite>(engine_->LoadTexture("./Assets/Textures/hp_middle.png"), 100, "Delay_HP_Front_Middle_Sprite");
	delayHpFrontMiddleSprite_->param_->texture.anchor = Vector2(0.0f, 0.5f);

	delayHpFrontRightSprite_ = std::make_unique<PrefabBaseSprite>(engine_->LoadTexture("./Assets/Textures/hp_right.png"), 100, "Delay_HP_Front_Right_Sprite");
	delayHpFrontRightSprite_->param_->texture.anchor = Vector2(0.0f, 0.5f);

	// 体力区切り
	hpSeparatorSprite_ = std::make_unique<PrefabBaseSprite>(engine_->LoadTexture("./Assets/Textures/hp_separator.png"), 100, "HP_Separator_Sprite");
	hpSeparatorSprite_->param_->texture.anchor = Vector2(0.5f, 0.5f);
	hpSeparatorSprite_->param_->transform.scale = Vector2(0.1f, 0.1f);
	hpSeparatorSprite_->param_->transform.rotate = -0.5f;

	// ボタン
	aButtonPrefab_ = std::make_unique<PrefabBaseSprite>(engine_->LoadTexture("./Assets/Textures/a_button.png"), 50, "Button_A_Sprite");
	bButtonPrefab_ = std::make_unique<PrefabBaseSprite>(engine_->LoadTexture("./Assets/Textures/b_button.png"), 50, "Button_B_Sprite");
	xButtonPrefab_ = std::make_unique<PrefabBaseSprite>(engine_->LoadTexture("./Assets/Textures/x_button.png"), 50, "Button_X_Sprite");
	yButtonPrefab_ = std::make_unique<PrefabBaseSprite>(engine_->LoadTexture("./Assets/Textures/y_button.png"), 50, "Button_Y_Sprite");
	rbButtonPrefab_ = std::make_unique<PrefabBaseSprite>(engine_->LoadTexture("./Assets/Textures/rb_button.png"), 50, "Button_RB_Sprite");
	lbButtonPrefab_ = std::make_unique<PrefabBaseSprite>(engine_->LoadTexture("./Assets/Textures/lb_button.png"), 50, "Button_LB_Sprite");
	rtButtonPrefab_ = std::make_unique<PrefabBaseSprite>(engine_->LoadTexture("./Assets/Textures/rt_button.png"), 50, "Button_RT_Sprite");
	ltButtonPrefab_ = std::make_unique<PrefabBaseSprite>(engine_->LoadTexture("./Assets/Textures/lt_button.png"), 50, "Button_LT_Sprite");

	buttonInSprite_ = std::make_unique<PrefabBaseSprite>(engine_->LoadTexture("./Assets/Textures/button_in_circle.png"), 50, "Button_In_Sprite");
	buttonInSprite_->param_->transform.scale = Vector2(0.7f, 0.7f);

	buttonOutSprite_ = std::make_unique<PrefabBaseSprite>(engine_->LoadTexture("./Assets/Textures/button_out_circle.png"), 50, "Button_Out_Sprite");

	// テキスト枠
	textFrameMiddleSprite_ = std::make_unique<PrefabBaseSprite>(engine_->LoadTexture("./Assets/Textures/text_frame_middle.png"), 50, "Text_Frame_Middle_Sprite");
	textFrameMiddleSprite_->param_->texture.anchor = Vector2(0.5f, 0.5f);

	textFrameRightSprite_ = std::make_unique<PrefabBaseSprite>(engine_->LoadTexture("./Assets/Textures/text_frame_right.png"), 50, "Text_Frame_Right_Sprite");
	textFrameRightSprite_->param_->texture.anchor = Vector2(0.0f, 0.5f);

	textFrameLeftSprite_ = std::make_unique<PrefabBaseSprite>(engine_->LoadTexture("./Assets/Textures/text_frame_left.png"), 50, "Text_Frame_Left_Sprite");
	textFrameLeftSprite_->param_->texture.anchor = Vector2(1.0f, 0.5f);

	// ボタンの画像
	rbButtonSprite_ = std::make_unique<Sprite>(engine_->LoadTexture("./Assets/Textures/rb_button.png"), "RB_Button_Sprite");
	lbButtonSprite_ = std::make_unique<Sprite>(engine_->LoadTexture("./Assets/Textures/lb_button.png"), "LB_Button_Sprite");

	weaponGetButtonSpritePrefab_ = std::make_unique<PrefabBaseSprite>(engine_->LoadTexture("./Assets/Textures/button_get_weapon.png"), 100, "Weapon_Get_Button_Sprite");
	weaponGetButtonSpritePrefab_->param_->transform.scale = Vector2(0.3f, 0.3f);



	// スティック操作のチュートリアルを生成する
	stickTutorial_ = std::make_unique<Tutorial>();
	stickTutorial_->AddSprite(uiEditor_->GetSprite("StickL"));
	stickTutorial_->AddSprite(uiEditor_->GetSprite("StickR"));
	stickTutorial_->AddSprite(uiEditor_->GetSprite("Camera"));
	stickTutorial_->AddSprite(uiEditor_->GetSprite("Move"));

	// ダッシュ操作のチュートリアルを生成する
	dashTutorial_ = std::make_unique<Tutorial>();
	dashTutorial_->AddSprite(uiEditor_->GetSprite("Dash"));
	dashTutorial_->AddSprite(uiEditor_->GetSprite("ButtonRB"));

	// 攻撃操作のチュートリアルを生成する
	attackTutorial_ = std::make_unique<Tutorial>();
	attackTutorial_->AddSprite(uiEditor_->GetSprite("Attack"));
	attackTutorial_->AddSprite(uiEditor_->GetSprite("ButtonX"));
	attackTutorial_->AddSprite(uiEditor_->GetSprite("StrongAttack"));
	attackTutorial_->AddSprite(uiEditor_->GetSprite("ButtonY"));

	// コンボ操作のチュートリアルを生成する
	comboTutorial_ = std::make_unique<Tutorial>();
	comboTutorial_->AddSprite(uiEditor_->GetSprite("Combo"));
	comboTutorial_->AddSprite(uiEditor_->GetSprite("ButtonCombo"));

	// 掴み操作のチュートリアルを生成する
	grabTutorial_ = std::make_unique<Tutorial>();
	grabTutorial_->AddSprite(uiEditor_->GetSprite("Grab"));
	grabTutorial_->AddSprite(uiEditor_->GetSprite("ButtonB"));

	// ガード操作のチュートリアルを生成する
	guardTutorial_ = std::make_unique<Tutorial>();
	guardTutorial_->AddSprite(uiEditor_->GetSprite("Guard"));
	guardTutorial_->AddSprite(uiEditor_->GetSprite("ButtonLB"));

	// 回避操作のチュートリアルを生成する
	avoidTutorial_ = std::make_unique<Tutorial>();
	avoidTutorial_->AddSprite(uiEditor_->GetSprite("Avoid"));
	avoidTutorial_->AddSprite(uiEditor_->GetSprite("ButtonA"));

	rageTutorial_ = std::make_unique<Tutorial>();

	// 照準枠スプライトを生成する
	reticleFrameSpritePrefab_ = std::make_unique<PrefabBaseSprite>(engine_->LoadTexture("./Assets/Textures/reticle_frame.png"), 100, "Reticle_Frame_Sprite");
	reticleFrameSpritePrefab_->param_->transform.scale = Vector2(0.25f, 0.25f);
	reticleSprite_ = uiEditor_->GetSprite("Reticle");
	reticleSprite_->param_->material.color.w = 1.0f; // 初期状態では透明にする
	reticle_ = std::make_unique<Reticle>();
	reticle_->Initialize(reticleFrameSpritePrefab_->CreateInstance(), reticleFrameSpritePrefab_->CreateInstance(),
		reticleFrameSpritePrefab_->CreateInstance(), reticleFrameSpritePrefab_->CreateInstance(), reticleSprite_);


	// 矢印スプライトを生成する
	navigationArrowSprite_ = std::make_unique<Sprite>(engine_->LoadTexture("./Assets/Textures/arrow.png"), "Arrow_Sprite");
	navigationArrow_ = std::make_unique<NavigationArrow>();
	navigationArrow_->Initialize(navigationArrowSprite_.get());

	
	startTextSprite_ = uiEditor_->GetSprite("Start_Text");
	if (startTextSprite_)
	{
		startTextSprite_->param_->material.color.w = 0.0f; // 初期状態では透明にする
	}

	bossTextSprite_ = uiEditor_->GetSprite("Boss_Text");
	if (bossTextSprite_)
	{
		bossTextSprite_->param_->material.color.w = 0.0f; // 初期状態では透明にする
	}

	winTextSprite_ = uiEditor_->GetSprite("Win_Text");
	if (winTextSprite_)
	{
		winTextSprite_->param_->material.color.w = 0.0f; // 初期状態では透明にする
	}

	loseTextSprite_ = uiEditor_->GetSprite("Lose_Text");
	if (loseTextSprite_)
	{
		loseTextSprite_->param_->material.color.w = 0.0f; // 初期状態では透明にする
	}

	weaponKnifeSprite_ = uiEditor_->GetSprite("WeaponKnife");
	if(weaponKnifeSprite_)
	{
		weaponKnifeSprite_->param_->material.color.w = 0.0f; // 初期状態では透明にする
	}

	weaponGunSprite_ = uiEditor_->GetSprite("WeaponGun");
	if (weaponGunSprite_)
	{
		weaponGunSprite_->param_->material.color.w = 0.0f; // 初期状態では透明にする
	}

	// フェード用スプライトを作成する
	fadeSprite_ = std::make_unique<Sprite>(engine_->LoadTexture("./Assets/Textures/white2x2.png"), "Fade");
	fadeSprite_->param_->texture.anchor = Vector2(0.0f, 1.0f);
	fadeSprite_->param_->screenAnchor = Engine::Render2D::ScreenAnchor::LeftBottom;
	fadeSprite_->param_->transform.scale = Vector2(static_cast<float>(engine_->GetScreenWidth()), static_cast<float>(engine_->GetScreenHeight()));
	fadeSprite_->param_->material.color = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
}