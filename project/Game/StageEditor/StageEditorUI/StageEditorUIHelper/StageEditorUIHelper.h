#pragma once
#include <vector>
#include <string>
#include "StageEditor/StageData/StageData.h"

class StageSpawner;
class StageEditorHistory;
class GameScene;
class MotionManager;

namespace StageEditorUIHelper
{
	/// @brief プレハブの名前を取得する
	/// @return 
	std::vector<std::string> GetPrefabNames();

	/// @brief プレハブとして配置データを保存する
	/// @param name 
	/// @param data 
	void SavePrefab(const std::string& name, const PlacementData& data);

	/// @brief プレハブから配置データを読み込む
	/// @param name 
	/// @param data 
	void LoadPrefab(const std::string& name, PlacementData& data);

	/// @brief テーブルレイアウトの開始
	/// @param str_id 
	/// @return 
	bool BeginPropertyTable(const char* str_id);

	/// @brief テーブルレイアウトの終了
	void EndPropertyTable();

	/// @brief プロパティラベルを描画する
	/// @param label 
	void PropertyLabel(const char* label);

	/// @brief ビヘイビアツリーのセレクターを描画する
	/// @param label 
	/// @param currentBT 
	/// @param behaviorTreeNames 
	/// @return 
	bool DrawBehaviorTreeSelector(const char* label, std::string& currentBT, const std::vector<std::string>& behaviorTreeNames);

	/// @brief ビヘイビアツリーの設定を描画する
	/// @param btConfig 
	/// @param behaviorTreeNames 
	/// @param isDirty 
	/// @return 
	bool DrawBehaviorTreeSettings(BehaviorTreeConfig& btConfig, const std::vector<std::string>& behaviorTreeNames, bool& isDirty);

	/// @brief コンボツリーのセレクターを描画する
	/// @param label 
	/// @param currentCT 
	/// @param comboTreeNames 
	/// @return 
	bool DrawComboTreeSelector(const char* label, ComboTreeInputName& currentCT, const std::vector<std::string>& comboTreeNames);

	/// @brief コンボツリーの設定を描画する
	/// @param ctConfig 
	/// @param comboTreeNames 
	/// @param isDirty 
	/// @return 
	bool DrawComboTreeSettings(ComboTreeConfig& ctConfig, const std::vector<std::string>& comboTreeNames, bool& isDirty);

	/// @brief モーションのセレクターを描画する
	/// @param label 
	/// @param motionType 
	/// @param motionConfig 
	/// @param motionManager 
	/// @return 
	bool DrawMotionSelector(const char* label, MotionType motionType, MotionConfig& motionConfig, MotionManager* motionManager);

	/// @brief モーションのセレクターを描画する（履歴対応版）
	/// @param label 
	/// @param motionType 
	/// @param motionConfig 
	/// @param placementList 
	/// @param isDirty 
	/// @param history 
	/// @param motionManager 
	/// @return 
	bool DrawMotionSelectorWithHistory(const char* label, MotionType motionType, MotionConfig& motionConfig,
		std::vector<PlacementData>& placementList, bool& isDirty, StageEditorHistory* history, MotionManager* motionManager);

	/// @brief キャラクターモーションの設定を描画する
	/// @param target 
	/// @param placementList 
	/// @param isDirty 
	/// @param history 
	/// @param motionManager 
	/// @param useHistory 
	/// @return 
	bool DrawCharacterMotionSettings(PlacementData& target, std::vector<PlacementData>& placementList,
		bool& isDirty, StageEditorHistory* history, MotionManager* motionManager, bool useHistory);

	/// @brief キャラクターの基本設定を描画する
	/// @param target 
	/// @param placementList 
	/// @param isDirty 
	/// @param history 
	/// @param useHistory 
	void DrawCharacterBaseSettings(PlacementData& target, std::vector<PlacementData>& placementList,
		bool& isDirty, StageEditorHistory* history, bool useHistory);

	/// @brief イベントトリガーの設定を描画する
	/// @param target 
	/// @param placementList 
	/// @param isDirty 
	/// @param history 
	/// @param spawner 
	/// @param scene 
	/// @param eventStageDataFileNames 
	void DrawEventTriggerSettings(PlacementData& target, std::vector<PlacementData>& placementList,
		bool& isDirty, StageEditorHistory* history, StageSpawner* spawner, GameScene* scene,
		const std::vector<std::string>& eventStageDataFileNames);
}