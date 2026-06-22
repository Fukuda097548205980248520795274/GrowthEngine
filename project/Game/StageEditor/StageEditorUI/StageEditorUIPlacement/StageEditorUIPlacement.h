#pragma once
#include <vector>
#include "StageEditor/StageData/StageData.h"

class StageSpawner;
class StageEditorHistory;

class StageEditorUIPlacement
{
public:

	/// @brief コンストラクタ
	/// @param spawner 
	/// @param history 
	StageEditorUIPlacement(StageSpawner* spawner, StageEditorHistory* history);

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

	/// @brief モーションマネージャ
	MotionManager* motionManager_ = nullptr;


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

