#pragma once
#include <string>
#include <vector>
#include "../StageData/StageData.h"

class StageFileManager;
class StageSpawner;
class StageEditorHistory;
class NavMesh;

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
	StageEditorUI(StageFileManager* fileManager, StageSpawner* spawner, StageEditorHistory* history) : fileManager_(fileManager), spawner_(spawner), history_(history) {}

    /// @brief 初期化
    void Initialize();

	/// @brief 更新処理
    void Update();

	/// @brief UIの描画
    /// @param placementList 
    /// @param currentFileName 
    /// @param isPlaying 
    void DrawUI(std::vector<PlacementData>& placementList, std::string& currentFileName, bool& isPlaying, NavMesh* navMesh);

	/// @brief アセットウィンドウの描画
    /// @param placementList 
    /// @param currentFileName 
	/// @param isPlaying
    void DrawAssetWindow(std::vector<PlacementData>& placementList, std::string& currentFileName, bool& isPlaying, NavMesh* navMesh);

	/// @brief オブジェクトリストウィンドウの描画
    /// @param placementList 
    /// @param navMesh 
    void DrawObjectListWindow(std::vector<PlacementData>& placementList, NavMesh* navMesh);

	/// @brief 現在のエディタモードを取得する
	/// @return 
	EditorMode GetCurrentMode() const { return currentMode_; }

private:

	// ステージファイルの読み書きを担当するクラス
    StageFileManager* fileManager_ = nullptr;

	// ステージ上にオブジェクトを配置・削除するためのクラス
    StageSpawner* spawner_ = nullptr;

	// 編集の履歴を管理するクラス
	StageEditorHistory* history_ = nullptr;

	// 現在選択中のオブジェクトのインデックス
    int selectedIndex_ = -1;

	/// @brief モーションマネージャのインスタンス
	MotionManager* motionManager_ = nullptr;

	/// @brief ビヘイビアツリーデータの名前リスト
    std::vector<std::string> behaviorTreeNames_;


private:

    /// @brief 未保存の変更があるかどうか
    bool isDirty_ = false;

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

    /// @brief モーションの選択UIを表示する
    /// @param motionType 
    /// @param motionName 
    void MotionSelecter(const char* label, MotionType motionType, MotionConfig& motionConfig);

	/// @brief ビヘイビアツリーデータの名前を読み込む
    void LoadBehaviorTreeNames();

	/// @brief 保留中のアクションを実行する
    /// @param placementList 
    /// @param currentFileName 
    /// @param navMesh 
    void ExecutePendingAction(std::vector<PlacementData>& placementList, std::string& currentFileName, NavMesh* navMesh);
};

