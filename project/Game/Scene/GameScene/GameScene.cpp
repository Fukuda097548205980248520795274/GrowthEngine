#include "GameScene.h"
#include "BattleDirector/BattleDirector.h"

#include <algorithm>
#include <cmath>
#include <numbers>

#include "SharedTreeEditorClipboard/SharedTreeEditorClipboard.h"

#include "HUD/HP/BossHP/BossHP.h"
#include "HUD/Button/WeaponGetButton/WeaponGetButton.h"
#include "HUD/IntroText/IntroText.h"

/// @brief デストラクタ
GameScene::~GameScene()
{
	// クリア処理
	BattleDirector::GetInstance().Clear();
	SharedTreeEditorClipboard::GetInstance().Clear();
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

	// UIエディタの生成と初期化
	uiEditor_ = std::make_unique<UIEditor>();
	uiEditor_->Load("Game_Scene");

	// モデルエディタの生成と初期化
	modelEditor_ = std::make_unique<ModelEditor>();

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

	// ライトエディタの生成と初期化
	lightEditor_ = std::make_unique<LightEditor>();

	// ナビゲーションメッシュの生成と初期化
	navMesh_ = std::make_unique<NavMesh>();

	// ステージエディタの生成と初期化
	stageEditor_ = std::make_unique<StageEditor>(this);
	stageEditor_->Initialize();

	// 設定エディタの生成と初期化
	configEditor_ = std::make_unique<ConfigEditor>(this);

	// エディタワークスペースマネージャの生成と初期化
	editorWorkspaceManager_ = std::make_unique<EditorWorkspaceManager>();
	editorWorkspaceManager_->Initialize(stageEditor_.get(), configEditor_.get(), behaviorTreeEditor_.get(), behaviorTreeViewer_.get(), comboTreeEditor_.get(),
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
	phaseManager_->SetOnEnter(PhaseType::Exploration, [&]() { ExplorationPhaseInitialize(); });
	phaseManager_->SetOnUpdate(PhaseType::Exploration, [&]() { ExplorationPhaseUpdate(); });
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
}

/// @brief リセットする
void GameScene::Reset()
{
	// プレイヤーをリセットする
	if (player_)player_ = nullptr;

	// バトル制御をリセットする
	BattleDirector::GetInstance().Clear();
}

/// @brief 設定を反映させる
void GameScene::ApplyConfig(const GameConfig& config)
{
	// 設定を反映させる
	pivotFollowSpeed_ = config.pivotFollowSpeed;
	pivotRotateSpeed_ = config.pivotRotateSpeed;
	pivotMaxPitch_ = config.pivotMaxPitch;
	pivotCenterOffset_ = config.pivotCenterOffset;
	lockOnMaxPitch_ = config.lockOnMaxPitch;
	lockOnCameraFollowSpeed_ = config.lockOnCameraFollowSpeed;
	cameraShrinkSpeed_ = config.cameraShrinkSpeed;
	cameraExpandSpeed_ = config.cameraExpandSpeed;
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