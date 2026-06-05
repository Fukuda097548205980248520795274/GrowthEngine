#pragma once
#include <string>
#include <vector>
#include "../StageData/StageData.h"

class StageFileManager;
class StageSpawner;

class StageEditorUI
{
public:

	/// @brief コンストラクタ
    /// @param fileManager 
    /// @param spawner 
	StageEditorUI(StageFileManager* fileManager, StageSpawner* spawner) : fileManager_(fileManager), spawner_(spawner) {}

    /// @brief 初期化
    void Initialize();

	/// @brief UIの描画
    /// @param placementList 
    /// @param currentFileName 
    /// @param isPlaying 
    void DrawUI(std::vector<PlacementData>& placementList, std::string& currentFileName, bool& isPlaying);

	/// @brief アセットウィンドウの描画
    /// @param placementList 
    /// @param currentFileName 
	/// @param isPlaying
    void DrawAssetWindow(std::vector<PlacementData>& placementList, std::string& currentFileName, bool& isPlaying);

	/// @brief オブジェクトリストウィンドウの描画
    /// @param placementList 
    void DrawObjectListWindow(std::vector<PlacementData>& placementList);

private:

	// ステージファイルの読み書きを担当するクラス
    StageFileManager* fileManager_ = nullptr;

	// ステージ上にオブジェクトを配置・削除するためのクラス
    StageSpawner* spawner_ = nullptr;

	// 現在選択中のオブジェクトのインデックス
    int selectedIndex_ = -1;

	/// @brief モーションマネージャのインスタンス
	MotionManager* motionManager_ = nullptr;

	/// @brief ビヘイビアツリーデータの名前リスト
    std::vector<std::string> behaviorTreeNames_;

private:

	/// @brief モーションの選択UIを表示する
    /// @param label 
    /// @param motionType 
    /// @param motionName 
    void MotionSelecter(const char* label, MotionType& motionType, std::string& motionName);

	/// @brief ビヘイビアツリーデータの名前を読み込む
    void LoadBehaviorTreeNames();
};

