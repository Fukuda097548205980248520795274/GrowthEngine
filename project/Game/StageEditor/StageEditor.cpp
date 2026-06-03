#include "StageEditor.h"
#include <numbers>
#include <json.hpp>
#include "Scene/GameScene/GameScene.h"

// 大分類と小分類の表示用文字列
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
#ifdef _DEVELOPMENT

	ImGui::Begin("Stage Editor");

	// ファイルが選択されていない場合は、警告を表示してUIの描画を終了する
    if (currentFileName_.empty())
    {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "No Stage File Selected.");
        ImGui::Text("Please select or create a stage from 'Stage Project Assets' window.");
        ImGui::End();
        return;
    }

    // 現在編集中のファイル名を表示
    ImGui::Text("Current Stage: %s", currentFileName_.c_str());
    if (ImGui::Button("Save Stage", ImVec2(120, 30)))
    {
        SaveToFile(stageDataDir_ + currentFileName_);
    }

    ImGui::Separator();

    
	// ここにオブジェクト配置のUI
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
		currentData.durability = 100;
		currentData.attackPower = 1.0f;

		currentData.standMotion.type = MotionType::Stand;
		currentData.standMotion.name = "Standing";
		currentData.stanceMotion.type = MotionType::Stance;
		currentData.stanceMotion.name = "Fighter";
		currentData.walkMotion.type = MotionType::Walk;
		currentData.walkMotion.name = "Walk";
		currentData.dashMotion.type = MotionType::Dash;
		currentData.dashMotion.name = "Dash";
		currentData.avoidFrontMotion.type = MotionType::Avoid;
		currentData.avoidFrontMotion.name = "Front";
		currentData.avoidBackMotion.type = MotionType::Avoid;
		currentData.avoidBackMotion.name = "Back";
		currentData.avoidLeftMotion.type = MotionType::Avoid;
		currentData.avoidLeftMotion.name = "Front";
		currentData.avoidRightMotion.type = MotionType::Avoid;
		currentData.avoidRightMotion.name = "Back";

        isInitialized = true;
    }

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
		MotionSelecter("Standing Motion", currentData.standMotion.type, currentData.standMotion.name);
		MotionSelecter("Fighting Motion", currentData.stanceMotion.type, currentData.stanceMotion.name);
		MotionSelecter("Walking Motion", currentData.walkMotion.type, currentData.walkMotion.name);
		MotionSelecter("Dashing Motion", currentData.dashMotion.type, currentData.dashMotion.name);
		MotionSelecter("Avoiding Front Motion", currentData.avoidFrontMotion.type, currentData.avoidFrontMotion.name);
		MotionSelecter("Avoiding Back Motion", currentData.avoidBackMotion.type, currentData.avoidBackMotion.name);
		MotionSelecter("Avoiding Left Motion", currentData.avoidLeftMotion.type, currentData.avoidLeftMotion.name);
		MotionSelecter("Avoiding Right Motion", currentData.avoidRightMotion.type, currentData.avoidRightMotion.name);
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
		// 生成前にモーションのハンドルを取得しておく
        currentData.standMotion.handle = motionManager_->GetMotion(currentData.standMotion.type, currentData.standMotion.name);
		currentData.stanceMotion.handle = motionManager_->GetMotion(currentData.stanceMotion.type, currentData.stanceMotion.name);
		currentData.walkMotion.handle = motionManager_->GetMotion(currentData.walkMotion.type, currentData.walkMotion.name);
		currentData.dashMotion.handle = motionManager_->GetMotion(currentData.dashMotion.type, currentData.dashMotion.name);
		currentData.avoidFrontMotion.handle = motionManager_->GetMotion(currentData.avoidFrontMotion.type, currentData.avoidFrontMotion.name);
		currentData.avoidBackMotion.handle = motionManager_->GetMotion(currentData.avoidBackMotion.type, currentData.avoidBackMotion.name);
		currentData.avoidLeftMotion.handle = motionManager_->GetMotion(currentData.avoidLeftMotion.type, currentData.avoidLeftMotion.name);
		currentData.avoidRightMotion.handle = motionManager_->GetMotion(currentData.avoidRightMotion.type, currentData.avoidRightMotion.name);

        // 新しい配置データを初期化
        PlacementData newData;
        newData.category = currentData.category;
        newData.subType = currentData.subType;
		newData.position = currentData.position;
		newData.rotateY = currentData.rotateY;
		newData.scale = currentData.scale;
		newData.hp = currentData.hp;
		newData.durability = currentData.durability;
		newData.attackPower = currentData.attackPower;
		newData.standMotion = currentData.standMotion;
		newData.stanceMotion = currentData.stanceMotion;
		newData.walkMotion = currentData.walkMotion;
		newData.dashMotion = currentData.dashMotion;
		newData.avoidFrontMotion = currentData.avoidFrontMotion;
		newData.avoidBackMotion = currentData.avoidBackMotion;
		newData.avoidLeftMotion = currentData.avoidLeftMotion;
		newData.avoidRightMotion = currentData.avoidRightMotion;

        // 実際のゲーム画面に生成してリストに追加
        SpawnActualEntity(newData);
        placementList_.push_back(newData);
        selectedIndex_ = static_cast<int>(placementList_.size()) - 1;
    }

    ImGui::Separator();


	// 配置したオブジェクトのリストと編集UI
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

#endif
}

/// @brief アセットウィンドウの描画
void StageEditor::DrawAssetWindow()
{
#ifdef _DEVELOPMENT
    ImGui::Begin("Stage Project Assets");

    // 新規ステージの作成
    if (ImGui::Button("New Stage"))
    {
        ImGui::OpenPopup("New Stage Popup");
    }

	// 新規ステージ作成のポップアップ
    if (ImGui::BeginPopup("New Stage Popup"))
    {
		// 新しいファイル名の入力
        static char newFileName[64] = "";
        ImGui::InputText("File Name", newFileName, 64);

		// 作成ボタン
        if (ImGui::Button("Create"))
        {
            std::string fileName = newFileName;

            // 拡張子がなければ追加する
            if (fileName.find(".json") == std::string::npos) 
            {
                fileName += ".json";
            }
            currentFileName_ = fileName;

            // 必要であればここで配置リストなどをクリアする
            placementList_.clear();

            // 空のファイルを作成・保存
            SaveToFile(stageDataDir_ + currentFileName_);
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    ImGui::Separator();
    ImGui::Text("Saved Stages:");
    ImGui::Spacing();

    // フォルダ内のjsonファイルを取得
    std::vector<std::string> files;
    if (std::filesystem::exists(stageDataDir_))
    {
        for (const auto& entry : std::filesystem::directory_iterator(stageDataDir_))
        {
            if (entry.is_regular_file() && entry.path().extension() == ".json")
            {
                files.push_back(entry.path().filename().string());
            }
        }
    }

    // グリッドレイアウトの計算
    float thumbnailSize = 64.0f;
    float padding = 16.0f;
    float cellSize = thumbnailSize + padding;
    float panelWidth = ImGui::GetContentRegionAvail().x;
    int columnCount = std::max(1, static_cast<int>(panelWidth / cellSize));

	// テーブルを開始
    if (ImGui::BeginTable("StageAssetGrid", columnCount))
    {
        for (const auto& file : files)
        {
            ImGui::TableNextColumn();
            ImGui::PushID(file.c_str());

            // 選択中のファイルは背景色を変える
            bool isSelected = (currentFileName_ == file);
            if (isSelected)
            {
                ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(ImVec4(0.3f, 0.5f, 0.8f, 0.5f)));
            }

			// ファイル名をボタンとして表示
            if (ImGui::Button(file.c_str(), ImVec2(thumbnailSize, thumbnailSize)))
            {
                currentFileName_ = file;
                LoadFromFile(stageDataDir_ + currentFileName_);
            }

            // 右クリックメニュー
            if (ImGui::BeginPopupContextItem("FileContextMenu"))
            {
                // 読み込み
                if (ImGui::MenuItem("Load"))
                {
                    currentFileName_ = file;
                    LoadFromFile(stageDataDir_ + currentFileName_);
                }

                ImGui::EndPopup();
            }

            ImGui::TextWrapped("%s", file.c_str());
            ImGui::PopID();
        }
        ImGui::EndTable();
    }

    ImGui::End();
#endif
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
        itemJson["hStandMotion"] = data.standMotion.handle;
        itemJson["hStanceMotion"] = data.stanceMotion.handle;
        itemJson["hWalkMotion"] = data.walkMotion.handle;
        itemJson["hDashMotion"] = data.dashMotion.handle;
        itemJson["hAvoidFrontMotion"] = data.avoidFrontMotion.handle;
        itemJson["hAvoidBackMotion"] = data.avoidBackMotion.handle;
        itemJson["hAvoidLeftMotion"] = data.avoidLeftMotion.handle;
        itemJson["hAvoidRightMotion"] = data.avoidRightMotion.handle;

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

            data.standMotion.handle = itemJson.value("hStandMotion", 0);
            data.stanceMotion.handle = itemJson.value("hStanceMotion", 0);
            data.walkMotion.handle = itemJson.value("hWalkMotion", 0);
            data.dashMotion.handle = itemJson.value("hDashMotion", 0);
            data.avoidFrontMotion.handle = itemJson.value("hAvoidFrontMotion", 0);
            data.avoidBackMotion.handle = itemJson.value("hAvoidBackMotion", 0);
            data.avoidLeftMotion.handle = itemJson.value("hAvoidLeftMotion", 0);
            data.avoidRightMotion.handle = itemJson.value("hAvoidRightMotion", 0);

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
		initData.hStandMotion = data.standMotion.handle;
		initData.hStanceMotion = data.stanceMotion.handle;
		initData.hWalkMotion = data.walkMotion.handle;
		initData.hDashMotion = data.dashMotion.handle;
		initData.hAvoidFrontMotion = data.avoidFrontMotion.handle;
		initData.hAvoidBackMotion = data.avoidBackMotion.handle;
		initData.hAvoidLeftMotion = data.avoidLeftMotion.handle;
		initData.hAvoidRightMotion = data.avoidRightMotion.handle;

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