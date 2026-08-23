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
	void SavePrefab(const std::string& name, const TemplateData& data);

	/// @brief プレハブから配置データを読み込む
	/// @param name 
	/// @param data 
	bool LoadPrefab(const std::string& name, TemplateData& data);

	/// @brief テーブルレイアウトの開始
	/// @param strId 
	/// @return 
	bool BeginPropertyTable(const char* strId);

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

	/// @brief キャラクターのモーション設定を描画する
	/// @param target 
	/// @param motionManager 
	/// @return 
	bool DrawCharacterMotionSettings(TemplateData& target, MotionManager* motionManager);

	/// @brief キャラクターの基本設定を描画する
	/// @param target 
	/// @param placementList 
	/// @param isDirty 
	/// @param history 
	/// @param useHistory 
	void DrawCharacterPlacementSettings(PlacementData& target, bool& isDirty);

	/// @brief キャラクターのテンプレート設定を描画する
	/// @param target 
	/// @param isDirty 
	void DrawCharacterTemplateSettings(TemplateData& target, bool& isDirty);

	/// @brief 武器の基本設定を描画する
	/// @param target 
	/// @param isDirty 
	void DrawWeaponPlacementSettings(PlacementData& target, bool& isDirty);

	/// @brief 武器のテンプレート設定を描画する
	/// @param target 
	/// @param isDirty 
	void DrawWeaponTemplateSettings(TemplateData& target, bool& isDirty);

	/// @brief ステージオブジェクトの基本設定を描画する
	/// @param target 
	/// @param placementList 
	/// @param isDirty 
	/// @param history 
	/// @param useHistory 
	void DrawStageObjectPlacementSettings(PlacementData& target, bool& isDirty);

	/// @brief イベントトリガーの設定を描画する
	/// @param target 
	/// @param placementList 
	/// @param isDirty 
	/// @param history 
	/// @param spawner 
	/// @param scene 
	/// @param eventStageDataFileNames 
	/// @param cutsceneNames 
	void DrawEventTriggerSettings(TemplateData& target, bool& isDirty, GameScene* scene,
		const std::vector<std::string>& eventStageDataFileNames, const std::vector<std::string>& cutsceneNames);
}