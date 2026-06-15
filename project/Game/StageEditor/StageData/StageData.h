#pragma once
#include "GrowthEngine.h"
#include "MotionManager/MotionManager.h"
#include "NavMesh/NavMesh.h"

// JSONライブラリ
using json = nlohmann::json;

// 配置するオブジェクトの種類
enum class EditCategory
{
	Character,
	Object,
	Weapon,
	HUD
};

// 大分類と小分類の表示用文字列
inline const char* categoryNames[] = { "キャラクター", "オブジェクト", "武器","HUD"};
inline const char* characterTagNames[] = { "None", "プレイヤー", "味方", "重要参考人", "敵", "ボス" };
inline const char* stageObjectTagNames[] = { "None", "床", "壁", "イベントトリガー" };
inline const char* weaponCategoryNames[] = { "None", "片手武器", "両手武器" };
inline const char* eventTypeNames[] = { "None", "敵生成" };
inline const char* hudTagNames[] = { "None", "攻撃チュートリアル", "ガードチュートリアル" };

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

	// 練習時間 (チュートリアルの場合)
	float practiceTime = 0.0f;

	// 攻撃の最大回数 (攻撃チュートリアルの場合)
	int maxAttackCount = 1;

	// ガードの最大回数 (ガードチュートリアルの場合)
	int maxGuardCount = 1;

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

// イベントチェーンのデータ構造
struct ChainEventData
{
	// イベントの種類（例：敵生成、アイテム生成、トリガー発動など）
	int eventType = 0;

	// イベントのパラメータ（イベントの種類によって内容が異なる）
	int subType = 0;

	// イベントのパラメータ（イベントの種類によって内容が異なる）
	char name[256] = "";

	// 生成されるオブジェクトの位置（イベントトリガーの場合は、トリガーの中心からの相対位置）
	Vector3 position = Vector3(0.0f, 0.0f, 0.0f);

	// イベントが発生するトリガーの半径（イベントトリガーの場合）
	float triggerRadius = 1.0f;

	// イベントが発生したときに生成されるオブジェクトの配置データ
	std::vector<ChainEventData> childEvents;
};

/// @brief PlacementDataをJSONに変換（シリアライズ）
/// @param j 
/// @param s 
inline void toJson(json& j, const PlacementData& s)
{
	j = json{
		{"category", static_cast<int>(s.category)},
		{"subType", s.subType},
		{"name", s.name},
		{"posX", s.position.x}, {"posY", s.position.y}, {"posZ", s.position.z},
		{"rotX", s.rotate_.x}, {"rotY", s.rotate_.y}, {"rotZ", s.rotate_.z},
		{"scaleX", s.scale.x}, {"scaleY", s.scale.y}, {"scaleZ", s.scale.z},
		{"hp", s.hp},
		{"durability", s.durability},
		{"attackPower", s.attackPower},
		{"isUnbreakable", s.isUnbreakable},
		{"behaviorScriptName", s.behaviorScriptName},
		{"eventType", s.eventType},
		{"eventStringParam", s.eventStringParam},
		{"practiceTime", s.practiceTime },
		{"maxAttackCount", s.maxAttackCount },
		{"maxGuardCount", s.maxGuardCount },
		{"standMotionName", s.standMotion.name},
		{"stanceMotionName", s.stanceMotion.name},
		{"walkMotionName", s.walkMotion.name},
		{"dashMotionName", s.dashMotion.name},
		{"avoidFrontMotionName", s.avoidFrontMotion.name},
		{"avoidBackMotionName", s.avoidBackMotion.name},
		{"avoidLeftMotionName", s.avoidLeftMotion.name},
		{"avoidRightMotionName", s.avoidRightMotion.name}
	};
}

/// @brief PlacementDataのリストをJSONに変換（シリアライズ）
/// @param j 
/// @param v 
inline void toJson(json& j, const std::vector<PlacementData>& v)
{
	json arrayJson = json::array();
	for (const auto& item : v)
	{
		json itemJson;
		toJson(itemJson, item);

		arrayJson.push_back(itemJson);
	}
	j["objects"] = arrayJson;
}

/// @brief NavMeshをJSONに変換（シリアライズ）
/// @param j 
/// @param navMesh 
inline void toJson(json& j, const NavMesh& navMesh)
{
	json navMeshJson = json::array();
	for (const auto& poly : navMesh.GetPolygons())
	{
		json polyJson;
		polyJson["id"] = poly.id;

		// 頂点配列の保存
		json vertsJson = json::array();
		for (const auto& v : poly.vertices) 
		{
			vertsJson.push_back({ v.x, v.y, v.z });
		}
		polyJson["vertices"] = vertsJson;

		// 隣接IDの保存
		json neighborsJson = json::array();
		for (int neighborId : poly.neighborIds)
		{
			neighborsJson.push_back(neighborId);
		}
		polyJson["neighborIds"] = neighborsJson;

		navMeshJson.push_back(polyJson);
	}
	j["navMesh"] = navMeshJson;
}



/// @brief JSONからPlacementDataに変換（デシリアライズ）
/// @param j 
/// @param s 
inline void fromJson(const json& j, PlacementData& s)
{
	s.category = static_cast<EditCategory>(j.value("category", 0));
	s.subType = j.value("subType", 0);
	std::string nameStr = j.value("name", "");
	strncpy_s(s.name, nameStr.c_str(), sizeof(s.name) - 1);

	s.position = Vector3(j.value("posX", 0.0f), j.value("posY", 0.0f), j.value("posZ", 0.0f));
	s.rotate_ = Vector3(j.value("rotX", 0.0f), j.value("rotY", 0.0f), j.value("rotZ", 0.0f));
	s.scale = Vector3(j.value("scaleX", 1.0f), j.value("scaleY", 1.0f), j.value("scaleZ", 1.0f));

	s.hp = j.value("hp", 100);
	s.durability = j.value("durability", 100);
	s.attackPower = j.value("attackPower", 1.0f);
	s.isUnbreakable = j.value("isUnbreakable", false);

	std::string behaviorScriptStr = j.value("behaviorScriptName", "");
	strncpy_s(s.behaviorScriptName, behaviorScriptStr.c_str(), sizeof(s.behaviorScriptName) - 1);


	s.eventType = j.value("eventType", 0);
	std::string eventStringParamStr = j.value("eventStringParam", "");
	strncpy_s(s.eventStringParam, eventStringParamStr.c_str(), sizeof(s.eventStringParam) - 1);


	MotionManager* motionManager = MotionManager::GetInstance();

	s.practiceTime = j.value("practiceTime", 0.0f);
	s.maxAttackCount = j.value("maxAttackCount", 1);
	s.maxGuardCount = j.value("maxGuardCount", 1);

	s.standMotion.name = j.value("standMotionName", "Stand");
	s.stanceMotion.name = j.value("stanceMotionName", "Fighter");
	s.walkMotion.name = j.value("walkMotionName", "Walk");
	s.dashMotion.name = j.value("dashMotionName", "Dash");
	s.avoidFrontMotion.name = j.value("avoidFrontMotionName", "Front");
	s.avoidBackMotion.name = j.value("avoidBackMotionName", "Back");
	s.avoidLeftMotion.name = j.value("avoidLeftMotionName", "Front");
	s.avoidRightMotion.name = j.value("avoidRightMotionName", "Back");

	s.standMotion.handle = motionManager->GetMotion(MotionType::Stand, s.standMotion.name);
	s.stanceMotion.handle = motionManager->GetMotion(MotionType::Stance, s.stanceMotion.name);
	s.walkMotion.handle = motionManager->GetMotion(MotionType::Walk, s.walkMotion.name);
	s.dashMotion.handle = motionManager->GetMotion(MotionType::Dash, s.dashMotion.name);
	s.avoidFrontMotion.handle = motionManager->GetMotion(MotionType::Avoid, s.avoidFrontMotion.name);
	s.avoidBackMotion.handle = motionManager->GetMotion(MotionType::Avoid, s.avoidBackMotion.name);
	s.avoidLeftMotion.handle = motionManager->GetMotion(MotionType::Avoid, s.avoidLeftMotion.name);
	s.avoidRightMotion.handle = motionManager->GetMotion(MotionType::Avoid, s.avoidRightMotion.name);
}