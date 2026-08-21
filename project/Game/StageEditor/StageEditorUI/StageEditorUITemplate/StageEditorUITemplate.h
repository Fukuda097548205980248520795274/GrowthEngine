#pragma once
#include <vector>
#include <string>
#include "StageEditor/StageData/StageData.h"
#include "MotionManager/MotionManager.h"

class StageSpawner;
class StageEditorHistory;
class GameScene;

class StageEditorUITemplate
{
public:

	/// @brief コンストラクタ
	/// @param spawner 
	/// @param history 
	/// @param scene 
	StageEditorUITemplate(StageSpawner* spawner, StageEditorHistory* history, GameScene* scene);

	/// @brief デストラクタ
	~StageEditorUITemplate() = default;

	/// @brief UIを描画する
	/// @param isDirty 
	/// @param behaviorTreeNames 
	/// @param comboTreeNames 
	/// @param eventStageDataFileNames 
	/// @param cutsceneNames 
	void DrawUI(std::vector<PlacementData>& placementList, bool& isDirty,
		const std::vector<std::string>& behaviorTreeNames, const std::vector<std::string>& comboTreeNames,
		const std::vector<std::string>& eventStageDataFileNames, const std::vector<std::string>& cutsceneNames);


private:

	/// @brief プレビュー用データの読み込み
	/// @param fileName 
	void LoadPreviewData(const std::string& fileName);

	/// @brief プレビュー用データの保存
	void SavePreviewData();


private:

	// ステージ生成
	StageSpawner* spawner_ = nullptr;

	// 履歴管理へ
	StageEditorHistory* history_ = nullptr;

	// ゲームシーンへ
	GameScene* scene_ = nullptr;

	// モーションマネージャー
	MotionManager* motionManager_ = nullptr;


private:

	// 編集中のテンプレートデータ
	TemplateData currentData_;

	// 選択中のテンプレートインデックス
	int selectedTemplateIdx_ = -1;

	// 保存用のテンプレート名バッファ
	char templateNameBuffer_[64] = "";


	/// @brief プレビュー用の配置データのキャッシュ
	std::vector<PlacementData> cachedPreviewData_;

	/// @brief プレビュー用の選択中インデックス
	int32_t selectedPreviewIndex_ = -1;

	/// @brief 現在ロードされているファイル名
	std::string currentLoadedFileName_ = "";
};

