#pragma once
#include "GrowthEngine.h"

class GameScene;

enum class PlacementType 
{
    Player,
    NPC,
    Object,
    Weapon
};

// ステージエディターで配置するオブジェクトのデータ構造
struct PlacementData 
{
    PlacementType type;
    Vector3 position = Vector3(0.0f, 0.0f, 0.0f);

    // エディターのUI表示用一時変数（実機インスタンスへのポインタなどがあると便利）
    void* instancePtr = nullptr;
};

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

	/// @brief ファイルにステージデータを保存する
    /// @param filename 
    void SaveToFile(const std::string& filename);

	/// @brief ファイルからステージデータを読み込む
    /// @param filename 
    void LoadFromFile(const std::string& filename);


private:
    
    /// @brief シーン
    GameScene* scene_ = nullptr;

	// 配置するオブジェクトのリスト
    std::vector<PlacementData> placementList_;

    // 現在選択中のオブジェクトのインデックス
    int selectedIndex_ = -1;


private:

	/// @brief 実際のゲーム内エンティティを生成する
    /// @param data 
    void SpawnActualEntity(PlacementData& data);

	/// @brief 実際のゲーム内エンティティを削除する
    /// @param data 
    void DeleteActualEntity(PlacementData& data);
};

