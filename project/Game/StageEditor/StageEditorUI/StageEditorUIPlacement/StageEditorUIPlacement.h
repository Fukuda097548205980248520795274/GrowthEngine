#pragma once
#include <vector>
#include "StageEditor/StageData/StageData.h"

class StageSpawner;
class StageEditorHistory;
class GameScene;

class StageEditorUIPlacement
{
public:

	/// @brief コンストラクタ
	/// @param spawner 
	/// @param history 
	StageEditorUIPlacement(StageSpawner* spawner, StageEditorHistory* history, GameScene* scene);

	/// @brief UIの描画
	/// @param placementList 
	/// @param selectedIndex 
	/// @param isDirty 
	/// @param behaviorTreeNames
	void DrawUI(std::vector<PlacementData>& placementList, int& selectedIndex, bool& isDirty,
		const std::vector<std::string>& behaviorTreeNames, const std::vector<std::string>& eventStageDataFileNames);

private:

	/// @brief 配置されたオブジェクトのリストを描画する
	StageSpawner* spawner_;

	/// @brief 編集の履歴を管理するクラスへのポインタ
	StageEditorHistory* history_;

	// ゲームシーン
	GameScene* scene_ = nullptr;

	/// @brief モーションマネージャ
	MotionManager* motionManager_ = nullptr;


private:

	// Prefabの名前のリスト
	std::vector<std::string> prefabNames_;

	// Prefabの名前を入力するためのバッファ
	char newPrefabName_[64] = "";

	// Prefabの選択インデックス
	int selectedPrefabIdx_ = -1;
};

