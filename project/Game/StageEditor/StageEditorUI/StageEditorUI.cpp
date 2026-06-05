#include "StageEditorUI.h"
#include <numbers>
#include "../StageFileManager/StageFileManager.h"
#include "../StageSpawner/StageSpawner.h"

#include "Entity/Character/Character.h"
#include "Entity/Weapon/Weapon.h"
#include "StageObject/StageObject.h"

/// @brief 初期化
void StageEditorUI::Initialize()
{
	// モーションマネージャのインスタンスを取得
	motionManager_ = MotionManager::GetInstance();

	// ビヘイビアツリーデータの名前を読み込む
    LoadBehaviorTreeNames();
}

/// @brief UIの描画
/// @param placementList 
/// @param currentFileName 
void StageEditorUI::DrawUI(std::vector<PlacementData>& placementList, std::string& currentFileName, bool& isPlaying)
{
#ifdef _DEVELOPMENT

    ImGui::Begin("Stage Editor");

    // ファイルが選択されていない場合は、警告を表示してUIの描画を終了する
    if (currentFileName.empty())
    {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "No Stage File Selected.");
        ImGui::Text("Please select or create a stage from 'Stage Project Assets' window.");
        ImGui::End();
        return;
    }


	// PLAY / STOP ボタン
    if (isPlaying)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f)); // STOPボタンは赤色
        if (ImGui::Button("STOP", ImVec2(100, 30)))
        {
            isPlaying = false;
			Entity::SetUpdateEnabled(false);// すべての実体の更新を停止する
			StageObject::SetUpdateEnabled(false); // すべてのステージオブジェクトの更新を停止する

			// プレイモードを終了したら、配置リストに基づいてすべての実体を再生成する
            for (auto& data : placementList)
            {
                spawner_->DeleteActualEntity(data);
                spawner_->SpawnActualEntity(data);
            }
        }
        ImGui::PopStyleColor();
    }
    else
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f)); // PLAYボタンは緑色
        if (ImGui::Button("PLAY", ImVec2(100, 30)))
        {
            isPlaying = true;
			Entity::SetUpdateEnabled(true); // すべての実体の更新を再開する
			StageObject::SetUpdateEnabled(true); // すべてのステージオブジェクトの更新を再開する
        }
        ImGui::PopStyleColor();
    }
    ImGui::Separator();



    // 現在編集中のファイル名を表示
    ImGui::Text("Current Stage: %s", currentFileName.c_str());
    
    
    if (!isPlaying)
    {
        // Ctrl + S が押されたら保存する処理
        ImGuiIO& io = ImGui::GetIO();
        if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_S))
        {
            fileManager_->SaveToFile(currentFileName, placementList);
        }
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

        // プレイヤーと未選択以外　ビヘイビアツリーデータ
		if (currentData.subType != 0 && currentData.subType != 1)
		{
            ImGui::Separator();
            ImGui::Text("Behavior Tree Settings");

            // 開発中にファイルを新規作成した際、エディタを再起動せずにリストを更新できるボタン
            if (ImGui::Button("Refresh BT List"))
            {
                LoadBehaviorTreeNames();
            }

            // プレビュー用の文字列（未設定の場合は "Select Behavior Tree..." と表示）
            std::string currentBtName = currentData.behaviorScriptName;
            const char* previewBtValue = currentBtName.empty() ? "Select Behavior Tree..." : currentBtName.c_str();

            // プルダウンメニュー（コンボボックス）の描画
            if (ImGui::BeginCombo("Behavior Tree", previewBtValue))
            {
                for (const auto& name : behaviorTreeNames_)
                {
                    bool isSelected = (currentBtName == name);
                    if (ImGui::Selectable(name.c_str(), isSelected))
                    {
                        // 選択された名前を PlacementData の配列にコピーする
                        // ※Visual Studio環境なら strcpy_s を使用して安全にコピーします
                        strcpy_s(currentData.behaviorScriptName, sizeof(currentData.behaviorScriptName), name.c_str());
                    }

                    // 選択中のアイテムにフォーカスを合わせる
                    if (isSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
		}
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
        spawner_->SpawnActualEntity(newData);
        placementList.push_back(newData);
        selectedIndex_ = static_cast<int>(placementList.size()) - 1;
    }

    ImGui::Separator();


    // 配置したオブジェクトのリストと編集UI
    if (selectedIndex_ >= 0 && selectedIndex_ < placementList.size())
    {
        auto& target = placementList[selectedIndex_];
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
            spawner_->DeleteActualEntity(target); // 古い実体を消去
            spawner_->SpawnActualEntity(target);  // 新しい設定（新タグ）で再生成
        }
        
        // 座標などの編集UI...
        ImGui::DragFloat3("Position", &target.position.x, 0.1f);
    }

    ImGui::End();

#endif
}

/// @brief アセットウィンドウの描画
/// @param placementList 
/// @param currentFileName 
/// @param isPlaying 
void StageEditorUI::DrawAssetWindow(std::vector<PlacementData>& placementList, std::string& currentFileName, bool& isPlaying)
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
            currentFileName = fileName;

            // 必要であればここで配置リストなどをクリアする
            placementList.clear();

            // 空のファイルを作成・保存
            fileManager_->SaveToFile(currentFileName, placementList);
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    ImGui::Separator();
    ImGui::Text("Saved Stages:");
    ImGui::Spacing();

    // フォルダ内のjsonファイルを取得
	std::vector<std::string> files = fileManager_->GetSavedStageFiles();

    // グリッドレイアウトの計算
    float thumbnailSize = 64.0f;
    float padding = 16.0f;
    float cellSize = thumbnailSize + padding;
    float panelWidth = ImGui::GetContentRegionAvail().x;
    int columnCount = std::max(1, static_cast<int>(panelWidth / cellSize));

    // ポップアップを開くための状態管理変数
    static std::string fileToCopy = "";
    static std::string fileToDelete = "";

    // テーブルを開始
    if (ImGui::BeginTable("StageAssetGrid", columnCount))
    {
        for (const auto& file : files)
        {
            ImGui::TableNextColumn();
            ImGui::PushID(file.c_str());

            // 選択中のファイルは背景色を変える
            bool isSelected = (currentFileName == file);
            if (isSelected)
            {
                ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(ImVec4(0.3f, 0.5f, 0.8f, 0.5f)));
            }

            // ファイル名をボタンとして表示
            if (ImGui::Button(file.c_str(), ImVec2(thumbnailSize, thumbnailSize)))
            {
                currentFileName = file;
                fileManager_->LoadFromFile(currentFileName, placementList, spawner_);
            }

            // 右クリックメニュー
            if (ImGui::BeginPopupContextItem("FileContextMenu"))
            {
                // 読み込み
                if (ImGui::MenuItem("Load"))
                {
                    currentFileName = file;
                    fileManager_->LoadFromFile(currentFileName, placementList, spawner_);
                }

                if (ImGui::MenuItem("Save"), nullptr, false, !isPlaying)
                {
                    // 右クリックした対象を現在のファイルとして設定し、上書き保存する
                    currentFileName = file;
                    fileManager_->SaveToFile(currentFileName, placementList);
                }

                ImGui::Separator();

                // 削除は赤文字で表示
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
                if (ImGui::MenuItem("Delete"), nullptr, false, !isPlaying)
                {
                    fileToDelete = file;
                }
                ImGui::PopStyleColor();


                ImGui::EndPopup();
            }

            ImGui::TextWrapped("%s", file.c_str());
            ImGui::PopID();
        }
        ImGui::EndTable();
    }



    // 削除対象が設定されていればモーダルを開く
    if (!fileToDelete.empty())
    {
        ImGui::OpenPopup("Delete Confirmation");
    }

    // モーダルウィンドウの描画
    if (ImGui::BeginPopupModal("Delete Confirmation", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        // パスからファイル名だけを抽出して表示
        std::string targetName = std::filesystem::path(fileToDelete).filename().string();

        ImGui::Text("Are you sure you want to delete '%s'?", fileToDelete.c_str());
        ImGui::Separator();

        // はい ボタン
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
        if (ImGui::Button("Delete", ImVec2(120, 0)))
        {
            // 物理的にファイルを削除
            std::filesystem::remove(fileToDelete);

            // もし現在開いているファイルを削除した場合は、エディタの情報をリセットする
            if (currentFileName == targetName)
            {
                currentFileName = "";
                placementList.clear(); // 配置情報をクリア
                selectedIndex_ = -1;
            }

            fileToDelete = ""; // 対象をクリア
            ImGui::CloseCurrentPopup();
        }
        ImGui::PopStyleColor();


        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();

        // キャンセル ボタン
        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            fileToDelete = ""; // キャンセル時は対象をクリアするだけ
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }


    ImGui::End();
#endif
}

/// @brief オブジェクトリストウィンドウの描画
/// @param placementList 
void StageEditorUI::DrawObjectListWindow(std::vector<PlacementData>& placementList)
{
#ifdef _DEVELOPMENT
    ImGui::Begin("Object List");

    ImGui::Text("Placed Objects:");

    // オブジェクトのリスト表示（スクロール可能な領域）
    ImGui::BeginChild("ObjectListRegion", ImVec2(0, 150), true);
    for (int i = 0; i < placementList.size(); ++i)
    {
        auto& data = placementList[i];

        // 表示用のラベルを作成 (例: "ID:0 Player")
        std::string label = "ID:" + std::to_string(i) + " ";
        if (data.category == EditCategory::Character) label += characterTagNames[data.subType];
        else if (data.category == EditCategory::Object) label += stageObjectTagNames[data.subType];
        else if (data.category == EditCategory::Weapon) label += weaponCategoryNames[data.subType];

        // 選択されたら selectedIndex_ を更新
        if (ImGui::Selectable(label.c_str(), selectedIndex_ == i))
        {
            selectedIndex_ = i;
        }
    }
    ImGui::EndChild();

    ImGui::Separator();

    // 選択中のオブジェクトがある場合、編集UIを表示
    if (selectedIndex_ >= 0 && selectedIndex_ < placementList.size())
    {
        auto& target = placementList[selectedIndex_];
        ImGui::Text("--- Edit Selected Object ---");

        // カテゴリごとの編集項目
        if (target.category == EditCategory::Character)
        {
			Character* charPtr = static_cast<Character*>(target.instancePtr);
			charPtr->DrawDebugUI();
        }
        else if (target.category == EditCategory::Object)
        {
			StageObject* objPtr = static_cast<StageObject*>(target.instancePtr);
			objPtr->DrawUI();
        }
        else if (target.category == EditCategory::Weapon)
        {
			Weapon* weaponPtr = static_cast<Weapon*>(target.instancePtr);
			weaponPtr->DrawDebugUI();
        }

        ImGui::Separator();

        // 削除ボタン
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
        if (ImGui::Button("Delete Object", ImVec2(120, 0)))
        {
            spawner_->DeleteActualEntity(target);
            placementList.erase(placementList.begin() + selectedIndex_);
            selectedIndex_ = -1; // 選択状態をリセット
        }
        ImGui::PopStyleColor();
    }

    ImGui::End();
#endif
}

/// @brief モーションの選択UIを表示する
/// @param motionType 
/// @param motionName 
void StageEditorUI::MotionSelecter(const char* label, MotionType& motionType, std::string& motionName)
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

/// @brief ビヘイビアツリーデータの名前を読み込む
void StageEditorUI::LoadBehaviorTreeNames()
{
    behaviorTreeNames_.clear();

	// ビヘイビアツリーのデータが保存されているフォルダのパス
    std::string directoryPath = "./Assets/Parameter/BehaviorTree/";

	// フォルダが存在しない場合は処理を終了
    if (!std::filesystem::exists(directoryPath))
        return;

    // フォルダ内のファイルを走査
    for (const auto& entry : std::filesystem::directory_iterator(directoryPath))
    {
        if (entry.is_regular_file())
        {
            // stem() を使うと、拡張子を除いたファイル名を取得できます
            behaviorTreeNames_.push_back(entry.path().stem().string());
        }
    }
}