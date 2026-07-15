#pragma once
#include "GrowthEngine.h"
#include "MotionManager/MotionManager.h"
#include "NavMesh/NavMesh.h"

#include "Entity/Character/Character.h"
#include "Entity/Weapon/Weapon.h"
#include "StageObject/StageObject.h"
#include "HUD/HUD.h"

// JSONライブラリ
using json = nlohmann::json;

// 配置するオブジェクトの種類
enum class EditCategory
{
	Character,
	Object,
	Weapon
};

// 大分類と小分類の表示用文字列
inline const char* categoryNames[] = { "キャラクター", "オブジェクト", "武器","HUD"};
inline const char* characterTagNames[] = { "None", "プレイヤー", "味方", "重要参考人", "敵", "ボス" };
inline const char* stageObjectTagNames[] = { "None", "床", "壁", "イベントトリガー" };
inline const char* weaponCategoryNames[] = { "None", "片手武器", "両手武器" };
inline const char* eventTypeNames[] = 
{
	"None", 
	"オブジェクト生成",
	"カットシーン再生",
	"スティック操作チュートリアル", 
	"ダッシュ操作チュートリアル",
	"攻撃操作チュートリアル", 
	"コンボ操作チュートリアル",
	"掴み操作チュートリアル",
	"防御操作チュートリアル", 
	"回避操作チュートリアル", 
	"レイジモードチュートリアル" 
};

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

	// イベントトリガーの種類 (イベントトリガーの場合)
	int eventType = 0;

	// イベントトリガーのステージデータファイル名 (イベントトリガーの場合)
	char eventStageDataFileName[256] = "";

	/// @brief イベントトリガーのカットシーン名 (イベントトリガーの場合)
	char eventCutsceneName[256] = "";

	// モーション設定 (キャラクターの場合)
	MotionConfig standMotion;
	MotionConfig stanceMotion;
	MotionConfig walkMotion;
	MotionConfig dashMotion;
	MotionConfig avoidFrontMotion;
	MotionConfig avoidBackMotion;
	MotionConfig avoidLeftMotion;
	MotionConfig avoidRightMotion;
	MotionConfig guardMotion;

	/// @brief ビヘイビアツリーの設定 (キャラクターの場合)
	BehaviorTreeConfig behaviorTrees;

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
	j["category"] = static_cast<int>(s.category);
	j["subType"] = s.subType;
	j["name"] = s.name;
	j["posX"] = s.position.x;j["posY"] = s.position.y;j["posZ"] = s.position.z;
	j["rotX"] = s.rotate_.x; j["rotY"] = s.rotate_.y; j["rotZ"] = s.rotate_.z;
	j["scaleX"] = s.scale.x; j["scaleY"] = s.scale.y; j["scaleZ"] = s.scale.z;


	if (s.category == EditCategory::Character)
	{
		j["hp"] = s.hp;
		j["standMotionName"] = s.standMotion.name;
		j["stanceMotionName"] = s.stanceMotion.name;
		j["walkMotionName"] = s.walkMotion.name;
		j["dashMotionName"] = s.dashMotion.name;
		j["avoidFrontMotionName"] = s.avoidFrontMotion.name;
		j["avoidBackMotionName"] = s.avoidBackMotion.name;
		j["avoidLeftMotionName"] = s.avoidLeftMotion.name;
		j["avoidRightMotionName"] = s.avoidRightMotion.name;
		j["guardMotionName"] = s.guardMotion.name;

		// プレイヤーとNone以外はビヘイビアスクリプトを保存する
		if (s.subType != static_cast<int32_t>(CharacterTag::Player) && s.subType != static_cast<int32_t>(CharacterTag::None))
		{
			j["noneStateBT"] = s.behaviorTrees.noneStateBT;
			j["dashStateBT"] = s.behaviorTrees.dashStateBT;
			j["grabbedStateBT"] = s.behaviorTrees.grabbedStateBT;
			j["grabbingStateBT"] = s.behaviorTrees.grabbingStateBT;
			j["guardStateBT"] = s.behaviorTrees.guardStateBT;
			j["lightDamageStateBT"] = s.behaviorTrees.lightDamageStateBT;
			j["heavyDamageStateBT"] = s.behaviorTrees.heavyDamageStateBT;
			j["downFallingStateBT"] = s.behaviorTrees.downFallingStateBT;
			j["downLyingStateBT"] = s.behaviorTrees.downLyingStateBT;
			j["downGettingUpStateBT"] = s.behaviorTrees.downGettingUpStateBT;
			j["downStaggerStateBT"] = s.behaviorTrees.downStaggerStateBT;
			j["blownAwayStateBT"] = s.behaviorTrees.blownAwayStateBT;
			j["blownFallingStateBT"] = s.behaviorTrees.blownFallingStateBT;
			j["repelStateBT"] = s.behaviorTrees.repelStateBT;
			j["deflectStateBT"] = s.behaviorTrees.deflectStateBT;
			j["repelledStateBT"] = s.behaviorTrees.repelledStateBT;
			j["deflectedStateBT"] = s.behaviorTrees.deflectedStateBT;
			j["avoidStateBT"] = s.behaviorTrees.avoidStateBT;
			j["deadStateBT"] = s.behaviorTrees.deadStateBT;
		}
	}
	else if (s.category == EditCategory::Object)
	{
		if (s.subType == static_cast<int32_t>(StageObject::StageObjectTag::StaticEventTrigger))
		{
			j["eventType"] = s.eventType;
			j["eventStageDataFileName"] = s.eventStageDataFileName;
			j["eventCutsceneName"] = s.eventCutsceneName;
		}
	}
	else if (s.category == EditCategory::Weapon)
	{
		j["durability"] = s.durability;
		j["attackPower"] = s.attackPower;
		j["isUnbreakable"] = s.isUnbreakable;
	}
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


	// ビヘイビアツリーの設定を読み込む
	s.behaviorTrees.noneStateBT = j.value("noneStateBT", "");
	s.behaviorTrees.dashStateBT = j.value("dashStateBT", "");
	s.behaviorTrees.grabbedStateBT = j.value("grabbedStateBT", "");
	s.behaviorTrees.grabbingStateBT = j.value("grabbingStateBT", "");
	s.behaviorTrees. guardStateBT = j.value("guardStateBT", "");
	s.behaviorTrees.lightDamageStateBT = j.value("lightDamageStateBT", "");
	s.behaviorTrees.heavyDamageStateBT = j.value("heavyDamageStateBT", "");
	s.behaviorTrees.downFallingStateBT = j.value("downFallingStateBT", "");
	s.behaviorTrees.downLyingStateBT = j.value("downLyingStateBT", "");
	s.behaviorTrees.downGettingUpStateBT = j.value("downGettingUpStateBT", "");
	s.behaviorTrees.downStaggerStateBT = j.value("downStaggerStateBT", "");
	s.behaviorTrees.blownAwayStateBT = j.value("blownAwayStateBT", "");
	s.behaviorTrees.blownFallingStateBT = j.value("blownFallingStateBT", "");
	s.behaviorTrees.repelStateBT = j.value("repelStateBT", "");
	s.behaviorTrees.deflectStateBT = j.value("deflectStateBT", "");
	s.behaviorTrees.repelledStateBT = j.value("repelledStateBT", "");
	s.behaviorTrees.deflectedStateBT = j.value("deflectedStateBT", "");
	s.behaviorTrees.avoidStateBT = j.value("avoidStateBT", "");
	s.behaviorTrees.deadStateBT = j.value("deadStateBT", "");


	s.eventType = j.value("eventType", 0);

	std::string eventStageDataFileNameStr = j.value("eventStageDataFileName", "");
	strncpy_s(s.eventStageDataFileName, eventStageDataFileNameStr.c_str(), sizeof(s.eventStageDataFileName) - 1);

	std::string eventCutsceneNameStr = j.value("eventCutsceneName", "");
	strncpy_s(s.eventCutsceneName, eventCutsceneNameStr.c_str(), sizeof(s.eventCutsceneName) - 1);


	MotionManager* motionManager = MotionManager::GetInstance();

	s.standMotion.name = j.value("standMotionName", "Stand");
	s.stanceMotion.name = j.value("stanceMotionName", "Fighter");
	s.walkMotion.name = j.value("walkMotionName", "Walk");
	s.dashMotion.name = j.value("dashMotionName", "Dash");
	s.avoidFrontMotion.name = j.value("avoidFrontMotionName", "Front");
	s.avoidBackMotion.name = j.value("avoidBackMotionName", "Back");
	s.avoidLeftMotion.name = j.value("avoidLeftMotionName", "Front");
	s.avoidRightMotion.name = j.value("avoidRightMotionName", "Back");
	s.guardMotion.name = j.value("guardMotionName", "BothHands");

	s.standMotion.handle = motionManager->GetMotion(MotionType::Stand, s.standMotion.name);
	s.stanceMotion.handle = motionManager->GetMotion(MotionType::Stance, s.stanceMotion.name);
	s.walkMotion.handle = motionManager->GetMotion(MotionType::Walk, s.walkMotion.name);
	s.dashMotion.handle = motionManager->GetMotion(MotionType::Dash, s.dashMotion.name);
	s.avoidFrontMotion.handle = motionManager->GetMotion(MotionType::Avoid, s.avoidFrontMotion.name);
	s.avoidBackMotion.handle = motionManager->GetMotion(MotionType::Avoid, s.avoidBackMotion.name);
	s.avoidLeftMotion.handle = motionManager->GetMotion(MotionType::Avoid, s.avoidLeftMotion.name);
	s.avoidRightMotion.handle = motionManager->GetMotion(MotionType::Avoid, s.avoidRightMotion.name);
	s.guardMotion.handle = motionManager->GetMotion(MotionType::Guard, s.guardMotion.name);
}