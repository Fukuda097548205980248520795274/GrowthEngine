#pragma once
#include "GrowthEngine.h"

class HP;
class Weapon;

enum class FightStyle
{
	None,
	Tempest, // 旋嵐
	Hammer, // 撃鉄
};

/// @brief キャラクターのタグ
enum class CharacterTag
{
	None,
	Player,
	Ally,
	Vip,
	EnemyNormal,
	EnemyBoss,
};

/// @brief キャラクターの行動パターンを定義するビヘイビアツリーの設定
struct BehaviorTreeConfig
{
	std::string noneStateBT{};
	std::string dashStateBT{};
	std::string grabbedStateBT{};
	std::string grabbingStateBT{};
	std::string guardStateBT{};
	std::string lightDamageStateBT{};
	std::string heavyDamageStateBT{};
	std::string downFallingStateBT{};
	std::string downLyingStateBT{};
	std::string downGettingUpStateBT{};
	std::string downStaggerStateBT{};
	std::string blownAwayStateBT{};
	std::string blownFallingStateBT{};
	std::string repelStateBT{};
	std::string deflectStateBT{};
	std::string repelledStateBT{};
	std::string deflectedStateBT{};
	std::string avoidStateBT{};
	std::string deadStateBT{};
};

// / @brief キャラクターの行動パターンを定義するコンボツリーの入力名
struct ComboTreeInputName
{
	std::string xName_{};
	std::string yName_{};
	std::string bName_{};
};

// / @brief キャラクターの行動パターンを定義するコンボツリーの設定
struct ComboTreeConfig
{
	ComboTreeInputName noneStateCT{};
	ComboTreeInputName dashStateCT{};
	ComboTreeInputName grabbedStateCT{};
	ComboTreeInputName grabbingStateCT{};
	ComboTreeInputName guardStateCT{};
	ComboTreeInputName lightDamageStateCT{};
	ComboTreeInputName heavyDamageStateCT{};
	ComboTreeInputName downFallingStateCT{};
	ComboTreeInputName downLyingStateCT{};
	ComboTreeInputName downGettingUpStateCT{};
	ComboTreeInputName downStaggerStateCT{};
	ComboTreeInputName blownAwayStateCT{};
	ComboTreeInputName blownFallingStateCT{};
	ComboTreeInputName repelStateCT{};
	ComboTreeInputName deflectStateCT{};
	ComboTreeInputName repelledStateCT{};
	ComboTreeInputName deflectedStateCT{};
	ComboTreeInputName avoidStateCT{};
	ComboTreeInputName deadStateCT{};
};

/// @brief 初期化用データ
struct CharacterInitData
{
	/// @brief 位置
	Vector3 position = Vector3(0.0f, 0.0f, 0.0f);

	/// @brief 回転
	float rotateY = 0.0f;

	/// @brief 体力
	int hp = 0;

	/// @brief レイジゲージの閾値
	std::vector<float> rageGageThresholds;

	/// @brief モデル
	Render3DSkinningModel* model_ = nullptr;

	/// @brief 武器
	Weapon* weapon = nullptr;

	/// @brief 攻撃用トレイル
	Trail3D* attackTrail = nullptr;

	/// @brief 体力HUD
	HP* hpHUD = nullptr;


	/// @brief 立ちモーション
	AnimationHandle hStandMotion = 0;

	/// @brief 構えモーション
	AnimationHandle hStanceMotion = 0;

	/// @brief 歩きモーション
	AnimationHandle hWalkMotion = 0;

	/// @brief ダッシュモーション
	AnimationHandle hDashMotion = 0;

	/// @brief 前回避モーション
	AnimationHandle hAvoidFrontMotion = 0;

	/// @brief 後ろ回避モーション
	AnimationHandle hAvoidBackMotion = 0;

	/// @brief 左回避モーション
	AnimationHandle hAvoidLeftMotion = 0;

	/// @brief 右回避モーション
	AnimationHandle hAvoidRightMotion = 0;

	/// @brief 防御モーション
	AnimationHandle hGuardMotion = 0;

	/// @brief 防御成功モーション
	AnimationHandle hGuardHitMotion = 0;


	/// @brief 行動パターンを定義したスクリプトファイル名 
	std::string behaviorTreeName = "";


	/// @brief 当たり判定グループ
	Collision3DBaseSphere* hurtboxGroup = nullptr;

	/// @brief 攻撃判定グループ
	Collision3DBaseSphere* hitboxGroup = nullptr;

	/// @brief 着地判定グループ
	Collision3DInstanceCapsule* landingCollision = nullptr;

	/// @brief 壁接触の当たり判定グループ
	Collision3DInstanceCapsule* wallTouchCollision = nullptr;

	// / @brief イベントトリガーの当たり判定グループ
	Collision3DInstanceCapsule* eventTriggerCollision = nullptr;
};

/// @brief アニメーションのハンドルをまとめた構造体
struct AnimationHandleData
{
	AnimationHandle hStandMotion = 0;
	AnimationHandle hStanceMotion = 0;
	AnimationHandle hWalkMotion = 0;
	AnimationHandle hDashMotion = 0;
	AnimationHandle hAvoidFrontMotion = 0;
	AnimationHandle hAvoidBackMotion = 0;
	AnimationHandle hAvoidLeftMotion = 0;
	AnimationHandle hAvoidRightMotion = 0;
	AnimationHandle hGuardMotion = 0;
	AnimationHandle hGuardHitMotion = 0;
};