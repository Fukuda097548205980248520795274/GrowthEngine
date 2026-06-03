#pragma once
#pragma once
#include "GrowthEngine.h"

#include "Entity/Character/Player/Player.h"
#include "Entity/Character/NPC/NPC.h"

#include "Entity/Weapon/Weapon.h"

#include "StageObject/Floor/Floor.h"
#include "PivotPoint/PivotPoint.h"
#include "MotionManager/MotionManager.h"
#include "PostEffectManager/PostEffectManager.h"
#include "EffectManager/EffectManager.h"

#include "MotionManager/MotionManagerEditor/MotionManagerEditor.h"
#include "BehaviorTree/BehaviorTreeEditor/BehaviorTreeEditor.h"
#include "StageEditor/StageEditor.h"

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
	SoundManager* soundManager_ = nullptr;
	EffectManager* effectManager_ = nullptr;

	/// @brief ポストエフェクトマネージャ
	std::unique_ptr<PostEffectManager> postEffectManager_ = nullptr;


	// モーションマネージャエディタ
	std::unique_ptr<MotionManagerEditor> motionManagerEditor_ = nullptr;

	// ビヘイビアツリーエディタ
	std::unique_ptr<BehaviorTreeEditor> behaviorTreeEditor_ = nullptr;

	// ステージエディタ
	std::unique_ptr<StageEditor> stageEditor_ = nullptr;


	/// @brief プレイヤーの当たり判定グループ
	std::unique_ptr<Collision3DBaseSphere> playerHurtboxGroup_;

	/// @brief プレイヤーの攻撃判定グループ
	std::unique_ptr<Collision3DBaseSphere> playerHitboxGroup_;


	/// @brief 敵の当たり判定グループ
	std::unique_ptr<Collision3DBaseSphere> enemyHurtboxGroup_;

	/// @brief 敵の攻撃判定グループ
	std::unique_ptr<Collision3DBaseSphere> enemyHitboxGroup_;


	/// @brief 着地の当たり判定グループ
	std::unique_ptr<Collision3DBaseAABB> landingCollision_ = nullptr;

	/// @brief 床の当たり判定グループ
	std::unique_ptr<Collision3DBaseAABB> floorCollision_ = nullptr;



	/// @brief プレイヤーのモデル
	std::unique_ptr<Render3DSkinningModel> playerModel_ = nullptr;

	/// @brief プレイヤー
	std::unique_ptr<Player> player_ = nullptr;

	/// @brief プレイヤーの武器
	std::unique_ptr<Weapon> playerWeapon_ = nullptr;


	// キャラクターのモデル用ハンドル
	ModelHandle hCharacterModel_ = 0;
	AnimationHandle hCharacterAnimation_ = 0;
	SkeletonHandle hCharacterSkeleton_ = 0;


	// 片手武器モデル
	std::unique_ptr<PrefabBaseStaticModel> oneHandedWeaponModel_ = nullptr;

	/// @brief NPCのモデルリスト
	std::list<std::unique_ptr<Render3DSkinningModel>> npcModels_;

	int npcCount_ = 0;



	/// @brief NPCのリスト
	std::list<std::unique_ptr<NPC>> npcs_;

	/// @brief 武器のリスト
	std::list<std::unique_ptr<Weapon>> weapons_;

	/// @brief ステージオブジェクトのリスト
	std::list<std::unique_ptr<StageObject>> objects_;


	/// @brief カメラのピボットポイント
	std::unique_ptr<PivotPoint> pivotPoint_ = nullptr;

	/// @brief カメラ回転入力
	std::unique_ptr<InputGamepadRightStick> inputCameraRotate_ = nullptr;



	/// @brief 太陽光
	std::unique_ptr<LightDirectional> sunLight_ = nullptr;
};

