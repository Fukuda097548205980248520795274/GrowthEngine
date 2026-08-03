#pragma once
#include "GrowthEngine.h"
#include "MotionManager/MotionManager.h"
#include "NavMesh/NavMesh.h"

#include "Entity/Character/Character.h"
#include "Entity/Character/NPC/NPC.h"
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
inline const char* stageObjectTagNames[] = { "None", "床", "壁", "イベントトリガー", "カメラガード"};
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

	// 攻撃性（NPCの場合）
	float aggressiveness = 1.0f;

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

	/// @brief コンボツリーの設定 (キャラクターの場合)
	ComboTreeConfig comboTrees;

	/// @brief 戦闘エリア開始フラグ（イベントトリガーの場合）
	bool battleAreaStart = false;

	// 生成された実体へのポインタ
	void* instancePtr = nullptr;
};

/// @brief 戦闘エリアのデータ構造
struct BattleArea
{
	/// @brief 敵のリスト
	std::list<NPC*> enemies;

	/// @brief ステージオブジェクトのリスト
	std::list<StageObject*> stageObjects;

	/// @brief 戦闘エリアがクリアされたかどうかを判定する
	/// @return 
	bool IsCleared()
	{
		// 死亡または終了した敵をリストから削除する
		enemies.remove_if([](NPC* enemy)
			{
				if (!enemy || enemy->IsDead() || enemy->IsFinished())
				{
					return true;
				}

				return false;
			}
		);

		// 敵がすべて倒された場合、戦闘エリアはクリアされたとみなす
		if (enemies.size() == 0)
		{
			// すべてのステージオブジェクトを削除する
			for (auto& stageObject : stageObjects)
				if (stageObject) { stageObject->Delete(); }

			return true;
		}
		
		return false;
	}
};

// イベントチェーンのデータ構造
struct ChainEventData
{
	// イベントの種類
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
		j["aggressiveness"] = s.aggressiveness;
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
		else if (s.subType == static_cast<int32_t>(CharacterTag::Player))
		{
			j["noneStateCT_X"] = s.comboTrees.noneStateCT.xName_;
			j["noneStateCT_Y"] = s.comboTrees.noneStateCT.yName_;
			j["noneStateCT_B"] = s.comboTrees.noneStateCT.bName_;

			j["dashStateCT_X"] = s.comboTrees.dashStateCT.xName_;
			j["dashStateCT_Y"] = s.comboTrees.dashStateCT.yName_;
			j["dashStateCT_B"] = s.comboTrees.dashStateCT.bName_;

			j["grabbedStateCT_X"] = s.comboTrees.grabbedStateCT.xName_;
			j["grabbedStateCT_Y"] = s.comboTrees.grabbedStateCT.yName_;
			j["grabbedStateCT_B"] = s.comboTrees.grabbedStateCT.bName_;

			j["grabbingStateCT_X"] = s.comboTrees.grabbingStateCT.xName_;
			j["grabbingStateCT_Y"] = s.comboTrees.grabbingStateCT.yName_;
			j["grabbingStateCT_B"] = s.comboTrees.grabbingStateCT.bName_;

			j["guardStateCT_X"] = s.comboTrees.guardStateCT.xName_;
			j["guardStateCT_Y"] = s.comboTrees.guardStateCT.yName_;
			j["guardStateCT_B"] = s.comboTrees.guardStateCT.bName_;

			j["lightDamageStateCT_X"] = s.comboTrees.lightDamageStateCT.xName_;
			j["lightDamageStateCT_Y"] = s.comboTrees.lightDamageStateCT.yName_;
			j["lightDamageStateCT_B"] = s.comboTrees.lightDamageStateCT.bName_;

			j["heavyDamageStateCT_X"] = s.comboTrees.heavyDamageStateCT.xName_;
			j["heavyDamageStateCT_Y"] = s.comboTrees.heavyDamageStateCT.yName_;
			j["heavyDamageStateCT_B"] = s.comboTrees.heavyDamageStateCT.bName_;

			j["downFallingStateCT_X"] = s.comboTrees.downFallingStateCT.xName_;
			j["downFallingStateCT_Y"] = s.comboTrees.downFallingStateCT.yName_;
			j["downFallingStateCT_B"] = s.comboTrees.downFallingStateCT.bName_;

			j["downLyingStateCT_X"] = s.comboTrees.downLyingStateCT.xName_;
			j["downLyingStateCT_Y"] = s.comboTrees.downLyingStateCT.yName_;
			j["downLyingStateCT_B"] = s.comboTrees.downLyingStateCT.bName_;

			j["downGettingUpStateCT_X"] = s.comboTrees.downGettingUpStateCT.xName_;
			j["downGettingUpStateCT_Y"] = s.comboTrees.downGettingUpStateCT.yName_;
			j["downGettingUpStateCT_B"] = s.comboTrees.downGettingUpStateCT.bName_;

			j["downStaggerStateCT_X"] = s.comboTrees.downStaggerStateCT.xName_;
			j["downStaggerStateCT_Y"] = s.comboTrees.downStaggerStateCT.yName_;
			j["downStaggerStateCT_B"] = s.comboTrees.downStaggerStateCT.bName_;

			j["blownAwayStateCT_X"] = s.comboTrees.blownAwayStateCT.xName_;
			j["blownAwayStateCT_Y"] = s.comboTrees.blownAwayStateCT.yName_;
			j["blownAwayStateCT_B"] = s.comboTrees.blownAwayStateCT.bName_;

			j["blownFallingStateCT_X"] = s.comboTrees.blownFallingStateCT.xName_;
			j["blownFallingStateCT_Y"] = s.comboTrees.blownFallingStateCT.yName_;
			j["blownFallingStateCT_B"] = s.comboTrees.blownFallingStateCT.bName_;

			j["repelStateCT_X"] = s.comboTrees.repelStateCT.xName_;
			j["repelStateCT_Y"] = s.comboTrees.repelStateCT.yName_;
			j["repelStateCT_B"] = s.comboTrees.repelStateCT.bName_;

			j["deflectStateCT_X"] = s.comboTrees.deflectStateCT.xName_;
			j["deflectStateCT_Y"] = s.comboTrees.deflectStateCT.yName_;
			j["deflectStateCT_B"] = s.comboTrees.deflectStateCT.bName_;

			j["repelledStateCT_X"] = s.comboTrees.repelledStateCT.xName_;
			j["repelledStateCT_Y"] = s.comboTrees.repelledStateCT.yName_;
			j["repelledStateCT_B"] = s.comboTrees.repelledStateCT.bName_;

			j["deflectedStateCT_X"] = s.comboTrees.deflectedStateCT.xName_;
			j["deflectedStateCT_Y"] = s.comboTrees.deflectedStateCT.yName_;
			j["deflectedStateCT_B"] = s.comboTrees.deflectedStateCT.bName_;

			j["avoidStateCT_X"] = s.comboTrees.avoidStateCT.xName_;
			j["avoidStateCT_Y"] = s.comboTrees.avoidStateCT.yName_;
			j["avoidStateCT_B"] = s.comboTrees.avoidStateCT.bName_;

			j["deadStateCT_X"] = s.comboTrees.deadStateCT.xName_;
			j["deadStateCT_Y"] = s.comboTrees.deadStateCT.yName_;
			j["deadStateCT_B"] = s.comboTrees.deadStateCT.bName_;
		}
	}
	else if (s.category == EditCategory::Object)
	{
		if (s.subType == static_cast<int32_t>(StageObject::StageObjectTag::StaticEventTrigger))
		{
			j["eventType"] = s.eventType;
			j["eventStageDataFileName"] = s.eventStageDataFileName;
			j["eventCutsceneName"] = s.eventCutsceneName;
			j["battleAreaStart"] = s.battleAreaStart;
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
	s.aggressiveness = j.value("aggressiveness", 1.0f);
	s.durability = j.value("durability", 100);
	s.attackPower = j.value("attackPower", 1.0f);
	s.isUnbreakable = j.value("isUnbreakable", false);
	s.battleAreaStart = j.value("battleAreaStart", false);


	// プレイヤー以外のキャラクターはビヘイビアスクリプトを読み込む
	if (s.subType != static_cast<int32_t>(CharacterTag::Player) && s.subType != static_cast<int32_t>(CharacterTag::None))
	{
		s.behaviorTrees.noneStateBT = j.value("noneStateBT", "");
		s.behaviorTrees.stanceStateBT = j.value("stanceStateBT", "");
		s.behaviorTrees.dashStateBT = j.value("dashStateBT", "");
		s.behaviorTrees.grabbedStateBT = j.value("grabbedStateBT", "");
		s.behaviorTrees.grabbingStateBT = j.value("grabbingStateBT", "");
		s.behaviorTrees.guardStateBT = j.value("guardStateBT", "");
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
	}
	else if (s.subType == static_cast<int32_t>(CharacterTag::Player))
	{
		// プレイヤーキャラクターの場合はコンボツリーの入力名を読み込む
		s.comboTrees.noneStateCT.xName_ = j.value("noneStateCT_X", "");
		s.comboTrees.noneStateCT.yName_ = j.value("noneStateCT_Y", "");
		s.comboTrees.noneStateCT.bName_ = j.value("noneStateCT_B", "");

		s.comboTrees.stanceStateCT.xName_ = j.value("stanceStateCT_X", "");
		s.comboTrees.stanceStateCT.yName_ = j.value("stanceStateCT_Y", "");
		s.comboTrees.stanceStateCT.bName_ = j.value("stanceStateCT_B", "");

		s.comboTrees.dashStateCT.xName_ = j.value("dashStateCT_X", "");
		s.comboTrees.dashStateCT.yName_ = j.value("dashStateCT_Y", "");
		s.comboTrees.dashStateCT.bName_ = j.value("dashStateCT_B", "");

		s.comboTrees.grabbedStateCT.xName_ = j.value("grabbedStateCT_X", "");
		s.comboTrees.grabbedStateCT.yName_ = j.value("grabbedStateCT_Y", "");
		s.comboTrees.grabbedStateCT.bName_ = j.value("grabbedStateCT_B", "");

		s.comboTrees.grabbingStateCT.xName_ = j.value("grabbingStateCT_X", "");
		s.comboTrees.grabbingStateCT.yName_ = j.value("grabbingStateCT_Y", "");
		s.comboTrees.grabbingStateCT.bName_ = j.value("grabbingStateCT_B", "");

		s.comboTrees.guardStateCT.xName_ = j.value("guardStateCT_X", "");
		s.comboTrees.guardStateCT.yName_ = j.value("guardStateCT_Y", "");
		s.comboTrees.guardStateCT.bName_ = j.value("guardStateCT_B", "");

		s.comboTrees.lightDamageStateCT.xName_ = j.value("lightDamageStateCT_X", "");
		s.comboTrees.lightDamageStateCT.yName_ = j.value("lightDamageStateCT_Y", "");
		s.comboTrees.lightDamageStateCT.bName_ = j.value("lightDamageStateCT_B", "");

		s.comboTrees.heavyDamageStateCT.xName_ = j.value("heavyDamageStateCT_X", "");
		s.comboTrees.heavyDamageStateCT.yName_ = j.value("heavyDamageStateCT_Y", "");
		s.comboTrees.heavyDamageStateCT.bName_ = j.value("heavyDamageStateCT_B", "");

		s.comboTrees.downFallingStateCT.xName_ = j.value("downFallingStateCT_X", "");
		s.comboTrees.downFallingStateCT.yName_ = j.value("downFallingStateCT_Y", "");
		s.comboTrees.downFallingStateCT.bName_ = j.value("downFallingStateCT_B", "");

		s.comboTrees.downLyingStateCT.xName_ = j.value("downLyingStateCT_X", "");
		s.comboTrees.downLyingStateCT.yName_ = j.value("downLyingStateCT_Y", "");
		s.comboTrees.downLyingStateCT.bName_ = j.value("downLyingStateCT_B", "");

		s.comboTrees.downGettingUpStateCT.xName_ = j.value("downGettingUpStateCT_X", "");
		s.comboTrees.downGettingUpStateCT.yName_ = j.value("downGettingUpStateCT_Y", "");
		s.comboTrees.downGettingUpStateCT.bName_ = j.value("downGettingUpStateCT_B", "");

		s.comboTrees.downStaggerStateCT.xName_ = j.value("downStaggerStateCT_X", "");
		s.comboTrees.downStaggerStateCT.yName_ = j.value("downStaggerStateCT_Y", "");
		s.comboTrees.downStaggerStateCT.bName_ = j.value("downStaggerStateCT_B", "");

		s.comboTrees.blownAwayStateCT.xName_ = j.value("blownAwayStateCT_X", "");
		s.comboTrees.blownAwayStateCT.yName_ = j.value("blownAwayStateCT_Y", "");
		s.comboTrees.blownAwayStateCT.bName_ = j.value("blownAwayStateCT_B", "");

		s.comboTrees.blownFallingStateCT.xName_ = j.value("blownFallingStateCT_X", "");
		s.comboTrees.blownFallingStateCT.yName_ = j.value("blownFallingStateCT_Y", "");
		s.comboTrees.blownFallingStateCT.bName_ = j.value("blownFallingStateCT_B", "");

		s.comboTrees.repelStateCT.xName_ = j.value("repelStateCT_X", "");
		s.comboTrees.repelStateCT.yName_ = j.value("repelStateCT_Y", "");
		s.comboTrees.repelStateCT.bName_ = j.value("repelStateCT_B", "");

		s.comboTrees.deflectStateCT.xName_ = j.value("deflectStateCT_X", "");
		s.comboTrees.deflectStateCT.yName_ = j.value("deflectStateCT_Y", "");
		s.comboTrees.deflectStateCT.bName_ = j.value("deflectStateCT_B", "");

		s.comboTrees.repelledStateCT.xName_ = j.value("repelledStateCT_X", "");
		s.comboTrees.repelledStateCT.yName_ = j.value("repelledStateCT_Y", "");
		s.comboTrees.repelledStateCT.bName_ = j.value("repelledStateCT_B", "");

		s.comboTrees.deflectedStateCT.xName_ = j.value("deflectedStateCT_X", "");
		s.comboTrees.deflectedStateCT.yName_ = j.value("deflectedStateCT_Y", "");
		s.comboTrees.deflectedStateCT.bName_ = j.value("deflectedStateCT_B", "");

		s.comboTrees.avoidStateCT.xName_ = j.value("avoidStateCT_X", "");
		s.comboTrees.avoidStateCT.yName_ = j.value("avoidStateCT_Y", "");
		s.comboTrees.avoidStateCT.bName_ = j.value("avoidStateCT_B", "");

		s.comboTrees.deadStateCT.xName_ = j.value("deadStateCT_X", "");
		s.comboTrees.deadStateCT.yName_ = j.value("deadStateCT_Y", "");
		s.comboTrees.deadStateCT.bName_ = j.value("deadStateCT_B", "");

	}


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