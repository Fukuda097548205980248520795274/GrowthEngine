#pragma once
#include <string>
#include <vector>
#include "../StageData/StageData.h"
#include "StageEditorUIPlacement/StageEditorUIPlacement.h"
#include "StageEditorUINavMesh/StageEditorUINavMesh.h"
#include "StageEditorUIObjectList/StageEditorUIObjectList.h"
#include "StageEditorGuizmo/StageEditorGuizmo.h"

class StageFileManager;
class StageSpawner;
class StageEditorHistory;
class NavMesh;
class GameScene;

class StageEditorUI
{
public:

	// エディタのモード
	enum class EditorMode
	{
		ObjectPlacement,// オブジェクト配置モード
		NavMeshEdit, // ナビメッシュ編集モード
	};

	/// @brief 保留中のアクションの種類
	enum class PendingAction
	{
		None,
		Load,
		New
	};


public:

	/// @brief コンストラクタ
	/// @param fileManager 
	/// @param spawner 
	/// @param history 
	/// @param gameScene 
	StageEditorUI(StageFileManager* fileManager, StageSpawner* spawner, StageEditorHistory* history, GameScene* scene) :
		fileManager_(fileManager), spawner_(spawner), history_(history), scene_(scene) {}

	/// @brief 初期化
	void Initialize();

	/// @brief 更新処理
	void Update();

	/// @brief UIの描画
	/// @param placementList 
	/// @param currentFileName 
	/// @param isPlaying 
	void DrawUI(std::vector<PlacementData>& placementList, std::string& currentFileName, bool& isPlaying, NavMesh* navMesh, bool& isDirty, bool canExtrude, bool canBridge);

	/// @brief アセットウィンドウの描画
	/// @param placementList 
	/// @param currentFileName 
	/// @param isPlaying
	void DrawAssetWindow(std::vector<PlacementData>& placementList, std::string& currentFileName, bool& isPlaying, NavMesh* navMesh, bool& isDirty);

	/// @brief オブジェクトリストウィンドウの描画
	/// @param placementList 
	/// @param navMesh 
	void DrawObjectListWindow(std::vector<PlacementData>& placementList, NavMesh* navMesh, bool& isDirty);

	/// @brief 現在のエディタモードを取得する
	/// @return 
	EditorMode GetCurrentMode() const { return currentMode_; }

	/// @brief 現在選択中のオブジェクトのインデックスを取得する
	/// @return 
	int GetSelectedIndex() const { return selectedIndex_; }

	/// @brief 現在選択中のオブジェクトのインデックスを設定する
	/// @param index 
	void SetSelectedIndex(int index) { selectedIndex_ = index; }


private:

	/// @brief オブジェクト配置モードUI
	std::unique_ptr<StageEditorUIPlacement> placementUI_ = nullptr;

	/// @brief ナビメッシュ編集モードUI
	std::unique_ptr<StageEditorUINavMesh> navMeshUI_ = nullptr;

	/// @brief オブジェクトリストUI
	std::unique_ptr<StageEditorUIObjectList> objectListUI_ = nullptr;

	/// @brief ギズモUI
	std::unique_ptr<StageEditorGuizmo> guizmo_ = nullptr;


private:

	// ステージファイルの読み書きを担当するクラス
	StageFileManager* fileManager_ = nullptr;

	// ステージ上にオブジェクトを配置・削除するためのクラス
	StageSpawner* spawner_ = nullptr;

	// ゲームシーンへのポインタ
	GameScene* scene_ = nullptr;

	// 編集の履歴を管理するクラス
	StageEditorHistory* history_ = nullptr;

	// 現在選択中のオブジェクトのインデックス
	int selectedIndex_ = -1;

	/// @brief モーションマネージャのインスタンス
	MotionManager* motionManager_ = nullptr;

	/// @brief ビヘイビアツリーデータの名前リスト
	std::vector<std::string> behaviorTreeNames_;


private:

	/// @brief 保留中のアクション
	PendingAction pendingAction_ = PendingAction::None;

	/// @brief 保留中のファイル名
	std::string pendingFileName_ = "";


private:

	/// @brief モード切替の入力キー
	std::unique_ptr<InputKey> inputModelChange_ = nullptr;

	/// @brief 現在のエディタモード
	EditorMode currentMode_ = EditorMode::ObjectPlacement;


private:

	/// @brief コピーされたオブジェクトのデータバッファ（ファイル間移動用）
	PlacementData copiedData_;

	/// @brief コピーされたデータがバッファに存在するかどうか
	bool hasCopiedData_ = false;


private:

	/// @brief モーション選択UIでモーションが変更されたかどうか
	bool isChangeAnimation_ = false;


private:

	/// @brief 保留中のアクションを実行する
	/// @param placementList 
	/// @param currentFileName 
	/// @param navMesh 
	void ExecutePendingAction(std::vector<PlacementData>& placementList, std::string& currentFileName, NavMesh* navMesh);

	/// @brief ショートカットキーの処理
	/// @param placementList 
	/// @param currentFileName 
	/// @param isPlaying 
	/// @param navMesh 
	void HandleShortcuts(std::vector<PlacementData>& placementList, std::string& currentFileName, bool& isPlaying, NavMesh* navMesh, bool& isDirty);

	/// @brief ビヘイビアツリーデータの名前を読み込む
	void LoadBehaviorTreeNames();
};

