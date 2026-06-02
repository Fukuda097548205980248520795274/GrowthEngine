#pragma once
#include "GrowthEngine.h"
#include "MotionManager/MotionManager.h"

class GameScene;

// 配置するオブジェクトの種類
enum class EditCategory
{
    Character,
    Object,
    Weapon
};

// ステージエディターで配置するオブジェクトのデータ構造
struct PlacementData 
{
	// 配置するオブジェクトの種類
    EditCategory category;

    // キャラクターならCharacterTag、オブジェクトならStageObjectTag、武器ならWeaponCategoryを格納
	int subType;

    // 位置
    Vector3 position = Vector3(0.0f, 0.0f, 0.0f);

	// 回転（Y軸のみ）
	float rotateY = 0.0f;

	// 拡縮
	Vector3 scale = Vector3(1.0f, 1.0f, 1.0f);

	// HP (キャラクターの場合)
	int32_t hp = 100;

	// 耐久力 (武器の場合)
	int32_t durability = 100;

	float attackPower = 1.0f; // 攻撃力 (武器の場合)

	// モーションのハンドル（キャラクターの場合）
    AnimationHandle hStandMotion = 0;
    AnimationHandle hStanceMotion = 0;
    AnimationHandle hWalkMotion = 0;
    AnimationHandle hDashMotion = 0;
    AnimationHandle hAvoidFrontMotion = 0;
    AnimationHandle hAvoidBackMotion = 0;
    AnimationHandle hAvoidLeftMotion = 0;
    AnimationHandle hAvoidRightMotion = 0;

    // 生成された実体へのポインタ
    void* instancePtr = nullptr;
};

using json = nlohmann::json;

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

    // モーションマネージャ
	MotionManager* motionManager_ = nullptr;

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

    /// @brief モーションの選択UIを表示する
    /// @param motionType 
    /// @param motionName 
    void MotionSelecter(const char* label, MotionType& motionType, std::string& motionName);
};

