#pragma once
#include "StageData/StageData.h"
#include "StageEditorUI/StageEditorUI.h"
#include "StageFileManager/StageFileManager.h"
#include "StageSpawner/StageSpawner.h"

class GameScene;

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

    /// @brief 描画処理（デバッグ用）
    void DrawUI();


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


private:

    /// @brief 現在編集中のファイル名
    std::string currentFileName_ = "";

    /// @brief 実行中かどうか
    bool isPlaying_ = false;
};

