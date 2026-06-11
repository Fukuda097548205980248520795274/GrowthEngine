#pragma once
#include "GrowthEngine.h"
#include "MotionManager/MotionManager.h"

// 配置するオブジェクトの種類
enum class EditCategory
{
	Character,
	Object,
	Weapon
};

// 大分類と小分類の表示用文字列
inline const char* categoryNames[] = { "キャラクター", "オブジェクト", "武器" };
inline const char* characterTagNames[] = { "None", "プレイヤー", "味方", "重要参考人", "敵", "ボス" };
inline const char* stageObjectTagNames[] = { "None", "床", "壁", "イベントトリガー" };
inline const char* weaponCategoryNames[] = { "None", "片手武器", "両手武器" };
inline const char* eventTypeNames[] = { "None", "敵生成" };

struct MotionConfig
{
	std::string name;
	AnimationHandle handle = 0;
};


// ステージエディターで配置するオブジェクトのデータ構造
struct PlacementData
{
	// 配置するオブジェクトの種類
	EditCategory category = EditCategory::Character;

	// キャラクターならCharacterTag、オブジェクトならStageObjectTag、武器ならWeaponCategoryを格納
	int subType = 0;

	// オブジェクトの名前（任意）
	char name[256] = "";

	// 位置
	Vector3 position = Vector3(0.0f, 0.0f, 0.0f);

	// 回転
	Vector3 rotate_ = Vector3(0.0f, 0.0f, 0.0f);

	// 拡縮
	Vector3 scale = Vector3(1.0f, 1.0f, 1.0f);

	// HP (キャラクターの場合)
	int32_t hp = 100;

	// 耐久力 (武器の場合)
	int32_t durability = 100;

	// 攻撃力 (武器の場合)
	float attackPower = 1.0f;

	// 壊れない武器かどうか (武器の場合)
	bool isUnbreakable = false;

	// 行動パターンを定義したスクリプトファイル名 (キャラクターの場合)
	char behaviorScriptName[256] = "";

	// イベントトリガーの種類 (イベントトリガーの場合)
	int eventType = 0;

	// イベントトリガーのパラメータ (イベントの種類によって内容が異なる)
	char eventStringParam[1024] = "";

	// モーション設定 (キャラクターの場合)
	MotionConfig standMotion;
	MotionConfig stanceMotion;
	MotionConfig walkMotion;
	MotionConfig dashMotion;
	MotionConfig avoidFrontMotion;
	MotionConfig avoidBackMotion;
	MotionConfig avoidLeftMotion;
	MotionConfig avoidRightMotion;

	// 生成された実体へのポインタ
	void* instancePtr = nullptr;
};

/// @brief ゲーム中のイベントトリガーからオブジェクトを生成するためのデータ構造
struct SpawnData
{
	// 配置するオブジェクトの種類
	EditCategory category = EditCategory::Character;

	// キャラクターならCharacterTag、オブジェクトならStageObjectTag、武器ならWeaponCategoryを格納
	int subType = 0;

	// オブジェクトの名前（任意）
	char name[256] = "";

	// 位置
	Vector3 position = Vector3(0.0f, 0.0f, 0.0f);

	// 回転
	Vector3 rotate_ = Vector3(0.0f, 0.0f, 0.0f);

	// 拡縮
	Vector3 scale = Vector3(1.0f, 1.0f, 1.0f);

	// HP (キャラクターの場合)
	int32_t hp = 100;

	// 耐久力 (武器の場合)
	int32_t durability = 100;

	// 攻撃力 (武器の場合)
	float attackPower = 1.0f;

	// 壊れない武器かどうか (武器の場合)
	bool isUnbreakable = false;

	// 行動パターンを定義したスクリプトファイル名 (キャラクターの場合)
	char behaviorScriptName[256] = "";

	// モーション設定 (キャラクターの場合)
	MotionConfig standMotion;
	MotionConfig stanceMotion;
	MotionConfig walkMotion;
	MotionConfig dashMotion;
	MotionConfig avoidFrontMotion;
	MotionConfig avoidBackMotion;
	MotionConfig avoidLeftMotion;
	MotionConfig avoidRightMotion;
};