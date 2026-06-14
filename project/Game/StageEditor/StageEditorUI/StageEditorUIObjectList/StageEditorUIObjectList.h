#pragma once
#include <vector>
#include <string>
#include "StageEditor/StageData/StageData.h"

class StageSpawner;
class StageEditorHistory;
class NavMesh;
class BehaviorTreeEditor;

class StageEditorUIObjectList
{
public:

	/// @brief コンストラクタ
	/// @param spawner 
	/// @param history 
	StageEditorUIObjectList(StageSpawner* spawner, StageEditorHistory* history, BehaviorTreeEditor* behaviorTreeEditor);

	/// @brief 配置されたオブジェクトのリストを描画する
	/// @param placementList 
	/// @param selectedIndex 
	/// @param isDirty 
	/// @param navMesh 
	void DrawWindow(std::vector<PlacementData>& placementList, int& selectedIndex, bool& isDirty,
		bool& hasCopiedData, PlacementData& copiedData, NavMesh* navMesh, const std::vector<std::string> behaviorTreeNames);

private:

	/// @brief 配置されたオブジェクトのリストを描画する
	StageSpawner* spawner_;

	/// @brief 編集の履歴を管理するクラスへのポインタ
	StageEditorHistory* history_;

	/// @brief ビヘイビアツリーエディタ
	BehaviorTreeEditor* behaviorTreeEditor_ = nullptr;

	/// @brief モーションマネージャ
	MotionManager* motionManager_ = nullptr;

	/// @brief エンジンのインスタンス
	const GrowthEngine* engine_ = GrowthEngine::GetInstance();

	/// @brief アニメーションをチェンジしたかどうか
	bool isChangeAnimation_ = false;


private:


	/// @brief モーションの選択UIを表示する
	/// @param motionType 
	/// @param motionName 
	bool MotionSelecter(const char* label, MotionType motionType, MotionConfig& motionConfig);

	/// @brief モーションの選択UIを表示する
	/// @param motionType 
	/// @param motionName 
	/// @param placementList
	void MotionSelecter(const char* label, MotionType motionType, MotionConfig& motionConfig, std::vector<PlacementData>& placementList, bool& isDirty);
};

