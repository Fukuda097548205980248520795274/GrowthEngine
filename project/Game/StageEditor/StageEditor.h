#pragma once
#include "StageData/StageData.h"
#include "StageEditorUI/StageEditorUI.h"
#include "StageFileManager/StageFileManager.h"
#include "StageSpawner/StageSpawner.h"
#include "StageEditorHistory/StageEditorHistory.h"
#include "StageEditorNavMeshController/StageEditorNavMeshController.h"

class GameScene;
class NavMesh;

// ステージデータの保存先ディレクトリ
constexpr const char* kStageDataDir = "./Assets/Parameter/StageData/";

class StageEditor
{
public:

	/// @brief コンストラクタ
	/// @param scene 
	StageEditor(GameScene* scene) : scene_(scene) {}

	/// @brief デストラクタ
	~StageEditor() = default;

	/// @brief 初期化
	void Initialize();

	/// @brief 更新処理
	/// @param dt 
	void Update(float dt);

	/// @brief ステージを読み込む
	/// @param fileName 
	void LoadStage(const std::string& fileName);

	/// @brief 描画処理（デバッグ用）
	void DrawUI();

	/// @brief オブジェクトを配置する
	/// @param data 
	void SpawnObject(PlacementData& data) { spawner_->SpawnActualEntity(data); }

	/// @brief オブジェクトを配置する（戦闘エリアの情報も渡す）
	/// @param data 
	/// @param battleAreas 
	void SpawnObject(PlacementData& data, BattleArea* battleAreas) { spawner_->SpawnActualEntity(data, battleAreas); }

	/// @brief オブジェクトを削除する
	/// @param data 
	void DeleteObject(PlacementData& data) { spawner_->DeleteActualEntity(data); }

	/// @brief 配置リストにデータを追加する
	/// @param data 
	void SetPlacementList(PlacementData& data) { placementList_.push_back(data); }


private:

	/// @brief シーン
	GameScene* scene_ = nullptr;

	// 配置するオブジェクトのリスト
	std::vector<PlacementData> placementList_;


private:

	// エディタUIを担当するクラス
	std::unique_ptr<StageEditorUI> editorUI_;

	// ステージファイルの読み書きを担当するクラス
	std::unique_ptr<StageFileManager> fileManager_;

	// ステージ上にオブジェクトを配置・削除するためのクラス
	std::unique_ptr<StageSpawner> spawner_;

	/// @brief 編集の履歴を管理するクラス
	std::unique_ptr<StageEditorHistory> history_;

	/// @brief ナビメッシュ編集を担当するクラス
	std::unique_ptr<StageEditorNavMeshController> navMeshController_;


private:

	/// @brief 現在編集中のファイル名
	std::string currentFileName_ = "";

	/// @brief 実行中かどうか
	bool isPlaying_ = false;

	// 変更があったかどうか
	bool isDirty_ = false;
};

