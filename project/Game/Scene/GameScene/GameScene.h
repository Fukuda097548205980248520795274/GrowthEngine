#pragma once
#pragma once
#include "GrowthEngine.h"

#include "Entity/Character/Player/Player.h"
#include "Entity/Character/NPC/NPC.h"

#include "Entity/Weapon/Weapon.h"

#include "StageObject/Floor/Floor.h"
#include "StageObject/Wall/Wall.h"
#include "StageObject/StaticEventTrigger/StaticEventTrigger.h"

#include "HUD/Timer/Timer.h"
#include "HUD/Tutorial/AttackTutorial/AttackTutorial.h"
#include "HUD/Tutorial/GuardTutorial/GuardTutorial.h"
#include "HUD/HP/HP.h"

#include "PivotPoint/PivotPoint.h"
#include "Shake/Shake.h"

#include "MotionManager/MotionManager.h"
#include "PostEffectManager/PostEffectManager.h"
#include "EffectManager/EffectManager.h"

#include "EditorWorkspaceManager/EditorWorkspaceManager.h"
#include "MotionManager/MotionManagerEditor/MotionManagerEditor.h"
#include "BehaviorTree/BehaviorTreeEditor/BehaviorTreeEditor.h"
#include "BehaviorTree/BehaviorTreeViewer/BehaviorTreeViewer.h"
#include "StageEditor/StageEditor.h"

#include "HUD/Button/TriggerButton/TriggerButton.h"
#include "HUD/Button/MashButton/MashButton.h"

#include "Pool/Pool.h"

#include "NavMesh/NavMesh.h"

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


public:

	/// @brief キャラクターを生成する
	/// @param initData 
	/// @return 
	Character* CreateCharacter(const Character::InitData& initData, Character::CharacterTag tag);

	/// @brief 武器を生成する
	/// @param position 
	/// @return 
	Weapon* CreateWeapon(const Weapon::InitData& initData);

	/// @brief 床オブジェクトを生成する
	/// @param position 
	/// @param scale 
	/// @return 
	Floor* CreateFloorObject(const Floor::InitData& initData);

	/// @brief 壁オブジェクトを生成する
	/// @param initData 
	/// @return 
	Wall* CreateWallObject(const Wall::InitData& initData);

	/// @brief 静的イベントトリガーオブジェクトを生成する
	/// @param initData 
	/// @return 
	StaticEventTrigger* CreateStaticEventTrigger(const StaticEventTrigger::InitData& initData);

	/// @brief タイマーHUDを生成する
	/// @param initData 
	/// @return 
	Timer* CreateTimer(const Timer::InitData& initData);

	/// @brief 攻撃チュートリアルを生成する
	/// @param initData 
	/// @return 
	AttackTutorial* CreateAttackTutorial(const AttackTutorial::InitData& initData);

	/// @brief ガードチュートリアルを生成する
	/// @param initData 
	/// @return 
	GuardTutorial* CreateGuardTutorial(const GuardTutorial::InitData& initData);

	/// @brief ナビゲーションメッシュを取得する
	/// @return 
	NavMesh* GetNavMesh() const { return navMesh_.get(); }

	/// @brief ビヘイビアツリーエディタを取得する
	/// @return 
	BehaviorTreeEditor* GetBehaviorTreeEditor() const { return behaviorTreeEditor_.get(); }

	/// @brief リセットする
	void Reset();


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


	/// @brief イベントトリガーに触れたときの処理
	/// @param eventType 
	/// @param param 
	bool HandleTriggerEvent(int eventType, const char* param);


private:

	// モーションマネージャ
	MotionManager* motionManager_ = nullptr;
	SoundManager* soundManager_ = nullptr;
	EffectManager* effectManager_ = nullptr;

	/// @brief ポストエフェクトマネージャ
	std::unique_ptr<PostEffectManager> postEffectManager_ = nullptr;

	// モーションマネージャエディタ
	std::unique_ptr<MotionManagerEditor> motionManagerEditor_ = nullptr;

	// ビヘイビアツリーエディタ
	std::unique_ptr<BehaviorTreeEditor> behaviorTreeEditor_ = nullptr;

	// ビヘイビアツリービューア
	std::unique_ptr<BehaviorTreeViewer> behaviorTreeViewer_ = nullptr;

	// ステージエディタ
	std::unique_ptr<StageEditor> stageEditor_ = nullptr;

	/// @brief エディタワークスペースマネージャ
	std::unique_ptr< EditorWorkspaceManager> editorWorkspaceManager_ = nullptr;


	/// @brief プレイヤーの当たり判定グループ
	std::unique_ptr<Collision3DBaseSphere> playerHurtboxGroup_;

	/// @brief プレイヤーの攻撃判定グループ
	std::unique_ptr<Collision3DBaseSphere> playerHitboxGroup_;


	/// @brief 敵の当たり判定グループ
	std::unique_ptr<Collision3DBaseSphere> enemyHurtboxGroup_;

	/// @brief 敵の攻撃判定グループ
	std::unique_ptr<Collision3DBaseSphere> enemyHitboxGroup_;


	/// @brief 着地の当たり判定グループ
	std::unique_ptr<Collision3DBaseCapsule> landingCollision_ = nullptr;

	/// @brief 床の当たり判定グループ
	std::unique_ptr<Collision3DBaseAABB> floorCollision_ = nullptr;


	/// @brief 壁接触の当たり判定グループ
	std::unique_ptr<Collision3DBaseCapsule> wallTouchCollision_ = nullptr;

	/// @brief 壁の当たり判定グループ
	std::unique_ptr<Collision3DBaseOBB> wallCollision_ = nullptr;


	/// @brief イベントトリガーに触れたかどうか
	std::unique_ptr<Collision3DBaseCapsule> eventTriggerCollision_ = nullptr;

	/// @brief イベントトリガー自体の当たり判定グループ
	std::unique_ptr<Collision3DBaseAABB> eventTriggerAABBCollision_ = nullptr;


	/// @brief プレイヤーのモデル
	std::unique_ptr<Render3DSkinningModel> playerModel_ = nullptr;

	/// @brief プレイヤーのトレイル
	std::unique_ptr<Trail3D> playerTrail_ = nullptr;

	/// @brief プレイヤー
	std::unique_ptr<Player> player_ = nullptr;

	/// @brief プレイヤーの武器
	std::unique_ptr<Weapon> playerWeapon_ = nullptr;

	/// @brief プレイヤーの体力バー
	std::unique_ptr<HP> playerHP_ = nullptr;


	// キャラクターのモデル用ハンドル
	ModelHandle hCharacterModel_ = 0;
	AnimationHandle hCharacterAnimation_ = 0;
	SkeletonHandle hCharacterSkeleton_ = 0;


	/// @brief ナビゲーションメッシュ
	std::unique_ptr<NavMesh> navMesh_ = nullptr;


	// 片手武器モデル
	std::unique_ptr<PrefabBaseStaticModel> oneHandedWeaponModel_ = nullptr;

	/// @brief コンマスプライト
	std::unique_ptr<PrefabBaseSprite> commaSprite_ = nullptr;

	/// @brief 数字スプライト
	std::unique_ptr<PrefabBaseSprite> numbersSprite_ = nullptr;

	/// @brief 攻撃ボタンのスプライト
	std::unique_ptr<Sprite> attackButtonSprite = nullptr;



	/// @brief NPCのモデルリスト
	std::list<std::unique_ptr<Render3DSkinningModel>> npcModels_;

	/// @brief NPCのモデルのプール
	std::unique_ptr<Pool<Render3DSkinningModel>> npcModelPool_ = nullptr;

	// npcの数
	int npcCount_ = 0;



	/// @brief NPCのリスト
	std::list<std::unique_ptr<NPC>> npcs_;

	/// @brief NPCのプール
	std::unique_ptr<Pool<NPC>> npcPool_ = nullptr;


	/// @brief NPCのトレイルのリスト
	std::list<std::unique_ptr<Trail3D>> npcTrails_;

	/// @brief NPCのパーティクルのリスト
	std::list<std::unique_ptr<Particle3D>> npcParticles_;

	/// @brief 武器のリスト
	std::list<std::unique_ptr<Weapon>> weapons_;

	/// @brief ステージオブジェクトのリスト
	std::list<std::unique_ptr<StageObject>> objects_;

	/// @brief HUDのリスト
	std::list<std::unique_ptr<HUD>> huds_;

	/// @brief 試験的な立方体
	std::unique_ptr<PrefabBaseCube> trialCube_ = nullptr;

	/// @brief Xボタン
	std::unique_ptr<MashButton> xButton_ = nullptr;


	/// @brief カメラのピボットポイント
	std::unique_ptr<PivotPoint> pivotPoint_ = nullptr;

	/// @brief カメラシェイク
	std::unique_ptr<Shake> cameraShake_ = nullptr;

	/// @brief カメラ回転入力
	std::unique_ptr<InputGamepadRightStick> inputCameraRotate_ = nullptr;



	/// @brief 太陽光
	std::unique_ptr<LightDirectional> sunLight_ = nullptr;


private:

	/// @brief HUDらの読み込み
	void LoadHUDs();

	// 体力バーの枠
	std::unique_ptr<PrefabBaseSprite> hpFrameLeftSprite_ = nullptr;
	std::unique_ptr<PrefabBaseSprite> hpFrameMiddleSprite_ = nullptr;
	std::unique_ptr<PrefabBaseSprite> hpFrameRightSprite_ = nullptr;

	// 体力バー
	std::unique_ptr<PrefabBaseSprite> hpLeftSprite_ = nullptr;
	std::unique_ptr<PrefabBaseSprite> hpMiddleSprite_ = nullptr;
	std::unique_ptr<PrefabBaseSprite> hpRightSprite_ = nullptr;

	// 後ろ側の体力バー
	std::unique_ptr<PrefabBaseSprite> hpBackLeftSprite_ = nullptr;
	std::unique_ptr<PrefabBaseSprite> hpBackMiddleSprite_ = nullptr;
	std::unique_ptr<PrefabBaseSprite> hpBackRightSprite_ = nullptr;

	// 遅延体力バー
	std::unique_ptr<PrefabBaseSprite> delayHpLeftSprite_ = nullptr;
	std::unique_ptr<PrefabBaseSprite> delayHpMiddleSprite_ = nullptr;
	std::unique_ptr<PrefabBaseSprite> delayHpRightSprite_ = nullptr;

	// 前側の遅延体力バー
	std::unique_ptr<PrefabBaseSprite> delayHpFrontLeftSprite_ = nullptr;
	std::unique_ptr<PrefabBaseSprite> delayHpFrontMiddleSprite_ = nullptr;
	std::unique_ptr<PrefabBaseSprite> delayHpFrontRightSprite_ = nullptr;

	/// @brief 体力ゲージの区切りのスプライト
	std::unique_ptr<PrefabBaseSprite> hpSeparatorSprite_ = nullptr;


private:

	/// @brief ボタンのスプライト
	std::unique_ptr<PrefabBaseSprite> aButtonSprite_ = nullptr;
	std::unique_ptr<PrefabBaseSprite> bButtonSprite_ = nullptr;
	std::unique_ptr<PrefabBaseSprite> xButtonSprite_ = nullptr;
	std::unique_ptr<PrefabBaseSprite> yButtonSprite_ = nullptr;

	// ボタンの内側と外側のスプライト
	std::unique_ptr<PrefabBaseSprite> buttonInSprite_ = nullptr;
	std::unique_ptr<PrefabBaseSprite> buttonOutSprite_ = nullptr;

	// テキストの枠のスプライト
	std::unique_ptr<PrefabBaseSprite> textFrameRightSprite_ = nullptr;
	std::unique_ptr<PrefabBaseSprite> textFrameMiddleSprite_ = nullptr;
	std::unique_ptr<PrefabBaseSprite> textFrameLeftSprite_ = nullptr;
};

