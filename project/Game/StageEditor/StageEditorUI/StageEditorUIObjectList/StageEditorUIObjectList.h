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

	/// @brief プレビュー用の配置データを読み込む
	/// @param fileName 
	void LoadPreviewData(const std::string& fileName);

	/// @brief プレビュー用の配置データを保存する
	void SavePreviewData();


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


	// プレビュー用の配置データをキャッシュする
	std::vector<PlacementData> cachedPreviewData_;

	// プレビュー用のファイル名
	std::string currentPreviewFileName_ = "";

	// プレビュー用の選択中のインデックス
	int selectedPreviewIndex_ = -1;
};

/// @brief 指定された名前が既に存在する場合、唯一の名前を生成する
/// @param baseName 
/// @param ignoreIndex 
/// @param placementList 
/// @return 
static std::string GenerateUniqueName(const std::string& baseName, int ignoreIndex, const std::vector<PlacementData>& placementList)
{
	// 名前が未設定の場合はそのまま返す
	if (baseName.empty()) return baseName;

	std::string newName = baseName;
	int counter = 1;
	bool isUnique = false;

	while (!isUnique)
	{
		isUnique = true;
		for (int i = 0; i < placementList.size(); ++i)
		{
			// 自分自身はチェックから除外する
			if (i == ignoreIndex) continue;

			// 被りを発見した場合
			if (std::string(placementList[i].name) == newName)
			{
				isUnique = false;
				// "ベース名_連番" の形にして再チェック
				newName = baseName + "_" + std::to_string(counter);
				counter++;
				break;
			}
		}
	}

	return newName;
}