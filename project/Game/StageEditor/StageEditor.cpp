#include "StageEditor.h"
#include <numbers>
#include <json.hpp>
#include "Scene/GameScene/GameScene.h"

const char* categoryNames[] = { "Character (NPC)", "StageObject", "Weapon" };
const char* characterTagNames[] = { "None", "Player", "Ally", "Vip", "EnemyNormal", "EnemyBoss" };
const char* stageObjectTagNames[] = { "None", "Floor", "Wall" };
const char* weaponCategoryNames[] = { "None", "OneHanded", "TwoHanded" };

/// @brief 初期化
void StageEditor::Initialize()
{
	// モーションマネージャのインスタンスを取得
	motionManager_ = MotionManager::GetInstance();
}

/// @brief 更新処理
/// @param dt 
void StageEditor::Update(float dt)
{

}

/// @brief 描画処理（デバッグ用）
void StageEditor::DrawUI()
{
	ImGui::Begin("Stage Editor");

    // ==========================================
    // 1. 新規オブジェクト追加エリア
    // ==========================================
    ImGui::Text("--- Add New Object ---");

    static PlacementData currentData;
    static bool isInitialized = false;
    if (!isInitialized)
    {
        currentData.category = EditCategory::Character;
        currentData.subType = 0;
        currentData.position = Vector3(0.0f, 0.0f, 0.0f);
        currentData.rotateY = 0.0f;
        currentData.scale = Vector3(1.0f, 1.0f, 1.0f);
        currentData.hp = 100;
        isInitialized = true;
    }

    MotionType motionTypeStand = MotionType::Stand;
    std::string motionNameStand = "Standing";

    MotionType motionTypeStance = MotionType::Stance;
    std::string motionNameStance = "Fighter";

    MotionType motionTypeWalk = MotionType::Walk;
    std::string motionNameWalk = "Walk";

    MotionType motionTypeDash = MotionType::Dash;
    std::string motionNameDash = "Dash";

    MotionType motionTypeAvoidFront = MotionType::Avoid;
    std::string motionNameAvoidFront = "Front";

    MotionType motionTypeAvoidBack = MotionType::Avoid;
    std::string motionNameAvoidBack = "Back";

    MotionType motionTypeAvoidLeft = MotionType::Avoid;
    std::string motionNameAvoidLeft = "Front";

    MotionType motionTypeAvoidRight = MotionType::Avoid;
    std::string motionNameAvoidRight = "Back";


    ImGui::Text("--- Save / Load ---");

    if (ImGui::Button("Save Stage", ImVec2(120, 30)))
    {
        // プロジェクトの実行ディレクトリからの相対パス
        SaveToFile("./Assets/Parameter/StageData/Stage1.json");
    }
    ImGui::SameLine();
    if (ImGui::Button("Load Stage", ImVec2(120, 30)))
    {
        LoadFromFile("./Assets/Parameter/StageData/Stage1.json");
    }

    ImGui::Separator();

    // 大分類の選択
    int intCat = static_cast<int>(currentData.category);
    if (ImGui::Combo("Category", &intCat, categoryNames, IM_ARRAYSIZE(categoryNames))) 
    {
        currentData.category = static_cast<EditCategory>(intCat);
        currentData.subType = 0; // 大分類が変わったら小分類のリセット
    }

    ImGui::Separator();

    // 大分類に応じて、小分類のコンボボックスの中身を切り替える
    if (currentData.category == EditCategory::Character) 
    {
        ImGui::Combo("Type (NPC)", &currentData.subType, characterTagNames, IM_ARRAYSIZE(characterTagNames));

        // 位置
        ImGui::DragFloat3("Spawn Position", &currentData.position.x, 0.1f);

		// HP
		ImGui::DragInt("HP", &currentData.hp, 1, 0, 10000);

		// 回転
		ImGui::DragFloat("Rotation (Y)", &currentData.rotateY, 0.01f, -std::numbers::pi_v<float>, std::numbers::pi_v<float>);

		// もしNPCが選ばれていたら、モーションの選択UIも表示する
		MotionSelecter("Standing Motion", motionTypeStand, motionNameStand);
		MotionSelecter("Fighting Motion", motionTypeStance, motionNameStance);
		MotionSelecter("Walking Motion", motionTypeWalk, motionNameWalk);
		MotionSelecter("Dashing Motion", motionTypeDash, motionNameDash);
		MotionSelecter("Avoiding Front Motion", motionTypeAvoidFront, motionNameAvoidFront);
		MotionSelecter("Avoiding Back Motion", motionTypeAvoidBack, motionNameAvoidBack);
		MotionSelecter("Avoiding Left Motion", motionTypeAvoidLeft, motionNameAvoidLeft);
		MotionSelecter("Avoiding Right Motion", motionTypeAvoidRight, motionNameAvoidRight);
    }
    else if (currentData.category == EditCategory::Object) 
    {
        ImGui::Combo("Type (Object)", &currentData.subType, stageObjectTagNames, IM_ARRAYSIZE(stageObjectTagNames));

        // 位置
        ImGui::DragFloat3("Spawn Position", &currentData.position.x, 0.1f);

        // 拡縮
		ImGui::DragFloat3("Scale", &currentData.scale.x, 0.1f, 0.0f, 10000.0f);
    }
    else if (currentData.category == EditCategory::Weapon) 
    {
        ImGui::Combo("Type (Weapon)", &currentData.subType, weaponCategoryNames, IM_ARRAYSIZE(weaponCategoryNames));

        // 位置
        ImGui::DragFloat3("Spawn Position", &currentData.position.x, 0.1f);

		// 耐久力
		ImGui::DragInt("Durability", &currentData.durability, 1, 1, 10000);

		// 攻撃力
		ImGui::DragFloat("Attack Power", &currentData.attackPower, 0.1f, 0.0f, 10000.0f);
    }

    ImGui::Separator();

    // 生成ボタン
    if (ImGui::Button("Spawn Object"))
    {
        PlacementData newData;
        newData.category = currentData.category;
        newData.subType = currentData.subType;
		newData.position = currentData.position;
		newData.rotateY = currentData.rotateY;
		newData.scale = currentData.scale;
		newData.hp = currentData.hp;
		newData.durability = currentData.durability;
		newData.attackPower = currentData.attackPower;
		newData.hStandMotion = motionManager_->GetMotion(motionTypeStand, motionNameStand);
		newData.hStanceMotion = motionManager_->GetMotion(motionTypeStance, motionNameStance);
		newData.hWalkMotion = motionManager_->GetMotion(motionTypeWalk, motionNameWalk);
		newData.hDashMotion = motionManager_->GetMotion(motionTypeDash, motionNameDash);
		newData.hAvoidFrontMotion = motionManager_->GetMotion(motionTypeAvoidFront, motionNameAvoidFront);
		newData.hAvoidBackMotion = motionManager_->GetMotion(motionTypeAvoidBack, motionNameAvoidBack);
		newData.hAvoidLeftMotion = motionManager_->GetMotion(motionTypeAvoidLeft, motionNameAvoidLeft);
		newData.hAvoidRightMotion = motionManager_->GetMotion(motionTypeAvoidRight, motionNameAvoidRight);

        // 実際のゲーム画面に生成してリストに追加
        SpawnActualEntity(newData);
        placementList_.push_back(newData);
        selectedIndex_ = static_cast<int>(placementList_.size()) - 1;
    }

    ImGui::Separator();

    // ==========================================
    // 2. 選択中オブジェクトのパラメータ編集エリア
    // ==========================================
    if (selectedIndex_ >= 0 && selectedIndex_ < placementList_.size()) 
    {
        auto& target = placementList_[selectedIndex_];
        ImGui::Text("--- Edit Selected Object ---");

        // 配置「後」でも Enemy か Ally かを切り替えられるようにする
        bool typeChanged = false;
        if (target.category == EditCategory::Character) 
        {
            typeChanged = ImGui::Combo("NPC Role", &target.subType, characterTagNames, IM_ARRAYSIZE(characterTagNames));
        }
        else if (target.category == EditCategory::Object)
        {
            typeChanged = ImGui::Combo("Object Type", &target.subType, stageObjectTagNames, IM_ARRAYSIZE(stageObjectTagNames));
        }

        // もし途中で Enemy から Ally に変えられたら、実体を一度消して再生成する
        if (typeChanged) 
        {
            DeleteActualEntity(target); // 古い実体を消去
            SpawnActualEntity(target);  // 新しい設定（新タグ）で再生成
        }

        // 座標などの編集UI...
        ImGui::DragFloat3("Position", &target.position.x, 0.1f);
    }

	ImGui::End();
}

/// @brief ファイルにステージデータを保存する
/// @param filename 
void StageEditor::SaveToFile(const std::string& filename)
{
    json rootJson;
    json arrayJson = json::array(); // 配置データの配列

    for (const auto& data : placementList_)
    {
        json itemJson;
        itemJson["category"] = static_cast<int>(data.category);
        itemJson["subType"] = data.subType;

        // Vector3は配列として保存すると扱いやすいです
        itemJson["position"] = { data.position.x, data.position.y, data.position.z };
        itemJson["rotateY"] = data.rotateY;
        itemJson["scale"] = { data.scale.x, data.scale.y, data.scale.z };

        // 各種パラメータ
        itemJson["hp"] = data.hp;
        itemJson["durability"] = data.durability;
        itemJson["attackPower"] = data.attackPower;

        // モーションハンドルの保存（モーション名で保存するのが理想ですが、今回はハンドル値として保存する例）
        itemJson["hStandMotion"] = data.hStandMotion;
        itemJson["hStanceMotion"] = data.hStanceMotion;
        itemJson["hWalkMotion"] = data.hWalkMotion;
        itemJson["hDashMotion"] = data.hDashMotion;
        itemJson["hAvoidFrontMotion"] = data.hAvoidFrontMotion;
        itemJson["hAvoidBackMotion"] = data.hAvoidBackMotion;
        itemJson["hAvoidLeftMotion"] = data.hAvoidLeftMotion;
        itemJson["hAvoidRightMotion"] = data.hAvoidRightMotion;

        arrayJson.push_back(itemJson);
    }

    rootJson["StageObjects"] = arrayJson;

    // ファイルに書き出し
    std::ofstream ofs(filename);
    if (ofs.is_open())
    {
        ofs << rootJson.dump(4); // 4文字インデントで見やすく整形
        ofs.close();
    }
}

/// @brief ファイルからステージデータを読み込む
/// @param filename 
void StageEditor::LoadFromFile(const std::string& filename)
{
    std::ifstream ifs(filename);
    if (!ifs.is_open()) return; // ファイルがなければ何もしない

    json rootJson;
    ifs >> rootJson;
    ifs.close();

    // ロードする前に、現在の配置リストと実体を全てクリアする
    // ※ 既存の実体を削除する関数(DeleteActualEntityなど)を作って呼ぶのが安全です
    placementList_.clear();

    if (rootJson.contains("StageObjects") && rootJson["StageObjects"].is_array())
    {
        for (const auto& itemJson : rootJson["StageObjects"])
        {
            PlacementData data;

            // データの復元
            data.category = static_cast<EditCategory>(itemJson.value("category", 0));
            data.subType = itemJson.value("subType", 0);

            if (itemJson.contains("position"))
            {
                data.position.x = itemJson["position"][0];
                data.position.y = itemJson["position"][1];
                data.position.z = itemJson["position"][2];
            }
            data.rotateY = itemJson.value("rotateY", 0.0f);

            if (itemJson.contains("scale"))
            {
                data.scale.x = itemJson["scale"][0];
                data.scale.y = itemJson["scale"][1];
                data.scale.z = itemJson["scale"][2];
            }

            data.hp = itemJson.value("hp", 100);
            data.durability = itemJson.value("durability", 100);
            data.attackPower = itemJson.value("attackPower", 1.0f);

            data.hStandMotion = itemJson.value("hStandMotion", 0);
            data.hStanceMotion = itemJson.value("hStanceMotion", 0);
            data.hWalkMotion = itemJson.value("hWalkMotion", 0);
            data.hDashMotion = itemJson.value("hDashMotion", 0);
            data.hAvoidFrontMotion = itemJson.value("hAvoidFrontMotion", 0);
            data.hAvoidBackMotion = itemJson.value("hAvoidBackMotion", 0);
            data.hAvoidLeftMotion = itemJson.value("hAvoidLeftMotion", 0);
            data.hAvoidRightMotion = itemJson.value("hAvoidRightMotion", 0);

            // ★ ここが超重要！ 読み込んだデータをもとに、ゲーム内に実体を生成する
            SpawnActualEntity(data);

            // リストに追加
            placementList_.push_back(data);
        }
    }
}

/// @brief 実際のゲーム内エンティティを生成する
/// @param data 
void StageEditor::SpawnActualEntity(PlacementData& data)
{
	// キャラクター
    if (data.category == EditCategory::Character) 
    {
        // int から CharacterTag にキャスト
        Character::CharacterTag tag = static_cast<Character::CharacterTag>(data.subType);

        // NPCの初期化データの作成
        Character::InitData initData;
        initData.position = data.position;
		initData.hp = data.hp;
		initData.rotateY = data.rotateY;
		initData.model_ = nullptr; // モデルは後で設定する
		initData.hStandMotion = data.hStandMotion;
		initData.hStanceMotion = data.hStanceMotion;
		initData.hWalkMotion = data.hWalkMotion;
		initData.hDashMotion = data.hDashMotion;
		initData.hAvoidFrontMotion = data.hAvoidFrontMotion;
		initData.hAvoidBackMotion = data.hAvoidBackMotion;
		initData.hAvoidLeftMotion = data.hAvoidLeftMotion;
		initData.hAvoidRightMotion = data.hAvoidRightMotion;

		// タグに応じて、NPCの初期化データを設定する
        Character* newCharacter = scene_->CreateCharacter(initData, tag);

        // ポインタを保存しておく
        data.instancePtr = newCharacter;
    }
    else if (data.category == EditCategory::Object) 
    {
        // オブジェクト

        StageObject::StageObjectTag tag = static_cast<StageObject::StageObjectTag>(data.subType);

        if (tag == StageObject::StageObjectTag::Floor) 
        {
			// 床の生成処理
            Floor::InitData initData;
            initData.position = data.position;
			initData.scale = data.scale;

            Floor* newFloor = scene_->CreateFloorObject(initData);
            data.instancePtr = newFloor;
        }
    }
	else if (data.category == EditCategory::Weapon)
	{
		// 武器
		Weapon::InitData initData;
		initData.position = data.position;
		initData.durability = data.durability;
		initData.attackPower = data.attackPower;
		initData.category = static_cast<WeaponCategory>(data.subType);
		initData.model = nullptr; // モデルは後で設定する

		WeaponCategory tag = static_cast<WeaponCategory>(data.subType);
	}
}

/// @brief 実際のゲーム内エンティティを削除する
/// @param data 
void StageEditor::DeleteActualEntity(PlacementData& data)
{

}

/// @brief モーションの選択UIを表示する
/// @param label 
/// @param motionType 
/// @param motionName 
void StageEditor::MotionSelecter(const char* label, MotionType& motionType, std::string& motionName)
{
    if (ImGui::TreeNode(label))
    {
        // モーションタイプ選択用のコンボボックス
        const char* typeNames[] = { "Stand", "Stance", "Walk", "Dash", "Attack", "Avoid", "Stagger", "Grab", "Grabbed",
            "DownFall", "DownLying", "DowoGetUp", "Guard", "styleChange", "grabStrikeAttacker", "grabStrikeTarget" };
        int currentType = static_cast<int>(motionType);

        // コンボボックスを描画し、変更があったらEnumにキャストして戻す
        if (ImGui::Combo("Motion Type", &currentType, typeNames, IM_ARRAYSIZE(typeNames)))
        {
            motionType = static_cast<MotionType>(currentType);
            motionName = "";
        }

        // 選択されたモーションタイプに応じたモーション名のリストをMotionManagerから取得
        std::vector<std::string> motionNames = MotionManager::GetInstance()->GetMotionNames(motionType);

        // モーション名のリストが空の場合はエラーメッセージを表示
        if (motionNames.empty())
        {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "No motions loaded.");
        }
        else
        {
            // 現在選択されているモーション名をプレビュー用の文字列として設定
            const char* previewValue = motionName.empty() ? "Select Motion..." : motionName.c_str();

            // モーション名選択用のコンボボックスを描画
            if (ImGui::BeginCombo("Motion Name", previewValue))
            {
                for (const auto& name : motionNames)
                {
                    // 現在のモーション名と同じものが選択されている状態にする
                    bool isSelected = (motionName == name);
                    if (ImGui::Selectable(name.c_str(), isSelected))
                    {
                        motionName = name;
                    }
                    if (isSelected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
        }

        ImGui::TreePop();
    }
}