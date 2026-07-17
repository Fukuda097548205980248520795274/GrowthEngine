#pragma once
#include <vector>
#include <string>
#include <set>
#include "StageEditor/StageData/StageData.h"

class StageSpawner;
class StageEditorHistory;
class NavMesh;
class BehaviorTreeEditor;
class GameScene;

class StageEditorUIObjectList
{
public:

	/// @brief コンストラクタ
	/// @param spawner 
	/// @param history 
	StageEditorUIObjectList(StageSpawner* spawner, StageEditorHistory* history, GameScene* scene, BehaviorTreeEditor* behaviorTreeEditor);

	/// @brief 配置されたオブジェクトのリストを描画する
	/// @param placementList 
	/// @param selectedIndex 
	/// @param isDirty 
	/// @param navMesh 
	void DrawWindow(std::vector<PlacementData>& placementList, int& selectedIndex, bool& isDirty,
		bool& hasCopiedData, PlacementData& copiedData, NavMesh* navMesh,
		const std::vector<std::string> behaviorTreeNames, const std::vector<std::string>& comboTreeNames, 
		const std::vector<std::string> eventStageDataFileNames, const std::vector<std::string>& cutsceneNames);

private:

	/// @brief 配置されたオブジェクトのリストを描画する
	StageSpawner* spawner_;

	/// @brief 編集の履歴を管理するクラスへのポインタ
	StageEditorHistory* history_;

	/// @brief 
	GameScene* scene_ = nullptr;

	/// @brief ビヘイビアツリーエディタ
	BehaviorTreeEditor* behaviorTreeEditor_ = nullptr;

	/// @brief モーションマネージャ
	MotionManager* motionManager_ = nullptr;

	/// @brief エンジンのインスタンス
	const GrowthEngine* engine_ = GrowthEngine::GetInstance();

	/// @brief アニメーションをチェンジしたかどうか
	bool isChangeAnimation_ = false;


private:
	
	// オブジェクトのフィルタリング用のImGuiTextFilterオブジェクト
	ImGuiTextFilter objectFilter_;

	// カテゴリのフィルタリング用のインデックス
	int categoryFilterIdx_ = 0;


private:

	// マルチ選択されたインデックスのセット
	std::set<int> multiSelectedIndices_;
};

