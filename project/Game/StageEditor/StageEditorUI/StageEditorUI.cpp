#include "StageEditorUI.h"
#include <numbers>
#include "../StageFileManager/StageFileManager.h"
#include "../StageSpawner/StageSpawner.h"
#include "../StageEditorHistory/StageEditorHistory.h"

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
    

    ImGuiIO& io = ImGui::GetIO();
    
	// Ctrl + S で上書き保存 (プレイ中は保存できないようにする)
    if (!isPlaying)
    {
        if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_S))
        {
            fileManager_->SaveToFile(currentFileName, placementList);
            isDirty_ = false;
        }
    }

	// Ctrl + Z でアンドゥ
    if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Z)) 
    {
        history_->Undo(placementList, spawner_);
        isDirty_ = true;
    }

	// Ctrl + Y でリドゥ
    if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Y)) 
    {
        history_->Redo(placementList, spawner_);
        isDirty_ = true;
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
		currentData.isUnbreakable = false;

        currentData.standMotion.name = "Standing";
        currentData.stanceMotion.name = "Fighter";
        currentData.walkMotion.name = "Walk";
        currentData.dashMotion.name = "Dash";
        currentData.avoidFrontMotion.name = "Front";
        currentData.avoidBackMotion.name = "Back";
        currentData.avoidLeftMotion.name = "Front";
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
        MotionSelecter("Standing Motion", MotionType::Stand, currentData.standMotion);
        MotionSelecter("Fighting Motion", MotionType::Stance, currentData.stanceMotion);
        MotionSelecter("Walking Motion", MotionType::Walk, currentData.walkMotion);
        MotionSelecter("Dashing Motion", MotionType::Dash, currentData.dashMotion);
        MotionSelecter("Avoiding Front Motion", MotionType::Avoid, currentData.avoidFrontMotion);
        MotionSelecter("Avoiding Back Motion", MotionType::Avoid, currentData.avoidBackMotion);
        MotionSelecter("Avoiding Left Motion", MotionType::Avoid, currentData.avoidLeftMotion);
        MotionSelecter("Avoiding Right Motion", MotionType::Avoid, currentData.avoidRightMotion);

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

		// 壊れない武器かどうか
		ImGui::Checkbox("Unbreakable", &currentData.isUnbreakable);
    }

    ImGui::Separator();

    // 生成ボタン
    if (ImGui::Button("Spawn Object"))
    {
		// 新しいオブジェクトを生成する前に、現在の配置リストの状態を履歴に保存する
        history_->SaveHistory(placementList);
        isDirty_ = true;

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
		newData.isUnbreakable = currentData.isUnbreakable;
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

    ImGui::End();

#endif
}

/// @brief アセットウィンドウの描画
/// @param placementList 
/// @param currentFileName 
/// @param isPlaying 
/// @param isDirty 
void StageEditorUI::DrawAssetWindow(std::vector<PlacementData>& placementList, std::string& currentFileName, bool& isPlaying)
{
#ifdef _DEVELOPMENT
    ImGui::Begin("Stage Project Assets");

    // 新規ステージの作成
    if (ImGui::Button("New Stage"))
    {
        ImGui::OpenPopup("New Stage Popup");
    }

    // 次に読み込むファイル名を保留するための変数
    bool requestSavePopup = false;

    // 新規ステージ作成のポップアップ
    if (ImGui::BeginPopup("New Stage Popup"))
    {
        // 新しいファイル名の入力
        static char newFileName[64] = "";
        ImGui::InputText("File Name", newFileName, 64);

        // 作成ボタン
        if (ImGui::Button("Create"))
        {
			// 入力されたファイル名に ".json" 拡張子を付加する
            std::string fileName = newFileName;
            if (fileName.find(".json") == std::string::npos) fileName += ".json";

			// 新しいステージを作成する前に、現在の配置リストに基づいてすべての実体を消去する
            if (isDirty_)
            {
                pendingAction_ = PendingAction::New;
                pendingFileName_ = fileName;

				// 変更が保存されていないことを警告するポップアップを開く
                requestSavePopup = true;
            }
            else
            {
				// すでにファイルが存在している場合は上書き保存する
                for (auto& data : placementList) spawner_->DeleteActualEntity(data);
                placementList.clear();
                currentFileName = fileName;
                fileManager_->SaveToFile(currentFileName, placementList);
            }

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
				// ファイルを切り替える前に、現在の配置リストに基づいてすべての実体を消去する
                if (isDirty_)
                {
                    pendingAction_ = PendingAction::Load;
                    pendingFileName_ = file;
                    
                    // 変更が保存されていないことを警告するポップアップを開く
                    requestSavePopup = true;
                }
                else
                {
					// すでにファイルが存在している場合は上書き保存する
                    for (auto& data : placementList) spawner_->DeleteActualEntity(data);
                    placementList.clear();
                    currentFileName = file;
                    fileManager_->LoadFromFile(currentFileName, placementList, spawner_);
                }
            }

            // 右クリックメニュー
            if (ImGui::BeginPopupContextItem("FileContextMenu"))
            {
                // 読み込み
                if (ImGui::MenuItem("Load"))
                {
					// ファイルを切り替える前に、現在の配置リストに基づいてすべての実体を消去する
                    if (isDirty_)
                    {
                        pendingAction_ = PendingAction::Load;
                        pendingFileName_ = file;
                        
                        // 変更が保存されていないことを警告するポップアップを開く
                        requestSavePopup = true;
                    }
                    else
                    {
						// すでにファイルが存在している場合は上書き保存する
                        for (auto& data : placementList) spawner_->DeleteActualEntity(data);
                        placementList.clear();
                        currentFileName = file;
                        fileManager_->LoadFromFile(currentFileName, placementList, spawner_);
                    }
                }

                if (ImGui::MenuItem("Save"), nullptr, false, !isPlaying)
                {
                    // 右クリックした対象を現在のファイルとして設定し、上書き保存する
                    currentFileName = file;
                    fileManager_->SaveToFile(currentFileName, placementList);
                    isDirty_ = false;
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


	// 保留中のアクションがある場合は、確認のポップアップを開く
    if (requestSavePopup)
    {
		ImGui::OpenPopup("Save Confirmation Popup");
    }

	// 保留中のアクションがある場合は、確認のポップアップを表示する
    if (ImGui::BeginPopupModal("Save Confirmation Popup", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        // 重要な操作なので、赤いテキストで警告を表示して強調する
        ImGui::Text("You have unsaved changes.\nDo you want to save the current tree before leaving?");
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // 保存ボタンを押して別のスクリプトを触る
        if (ImGui::Button("Save", ImVec2(140, 0)))
        {
            if (!currentFileName.empty())
            {
                fileManager_->SaveToFile(currentFileName, placementList);
            }
            isDirty_ = false;
            ExecutePendingAction(placementList, currentFileName);
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        // キャンセル(破棄)ボタンを押して別のスクリプトを触る
        if (ImGui::Button("Cancel", ImVec2(140, 0)))
        {
            isDirty_ = false; // 変更を破棄したとみなす
            ExecutePendingAction(placementList, currentFileName);
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        // 戻るボタンで今のスクリプトを触り続ける
        if (ImGui::Button("Back", ImVec2(120, 0)))
        {
            pendingAction_ = PendingAction::None;
            pendingFileName_ = "";
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
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
                // ファイルを切り替える前に、現在の配置リストに基づいてすべての実体を消去する
                for (auto& data : placementList)spawner_->DeleteActualEntity(data);
                placementList.clear();

                currentFileName = "";
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
/// @param isDirty 
void StageEditorUI::DrawObjectListWindow(std::vector<PlacementData>& placementList)
{
#ifdef _DEVELOPMENT
    ImGui::Begin("Object List");

    ImGui::Text("Placed Objects:");

    // オブジェクトのリスト表示
    ImGui::BeginChild("ObjectListRegion", ImVec2(0, 150), true);
    for (int i = 0; i < placementList.size(); ++i)
    {
        auto& data = placementList[i];

        // 表示用のラベルを作成 
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
			charPtr->DrawDebugUI(&target, placementList, history_, &isDirty_);

			// モーション選択UI
            MotionSelecter("Standing Motion", MotionType::Stand, target.standMotion);
            MotionSelecter("Fighting Motion", MotionType::Stance, target.stanceMotion);
            MotionSelecter("Walking Motion", MotionType::Walk, target.walkMotion);
            MotionSelecter("Dashing Motion", MotionType::Dash, target.dashMotion);
            MotionSelecter("Avoiding Front Motion", MotionType::Avoid, target.avoidFrontMotion);
            MotionSelecter("Avoiding Back Motion", MotionType::Avoid, target.avoidBackMotion);
            MotionSelecter("Avoiding Left Motion", MotionType::Avoid, target.avoidLeftMotion);
            MotionSelecter("Avoiding Right Motion", MotionType::Avoid, target.avoidRightMotion);
        }
        else if (target.category == EditCategory::Object)
        {
			StageObject* objPtr = static_cast<StageObject*>(target.instancePtr);
			objPtr->DrawDebugUI(&target, placementList, history_, &isDirty_);
        }
        else if (target.category == EditCategory::Weapon)
        {
			Weapon* weaponPtr = static_cast<Weapon*>(target.instancePtr);
			weaponPtr->DrawDebugUI(&target, placementList, history_, &isDirty_);
        }

        ImGui::Separator();

        // 削除ボタン
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
        if (ImGui::Button("Delete Object", ImVec2(120, 0)))
        {
			// 削除する前に、現在の配置リストの状態を履歴に保存する
            history_->SaveHistory(placementList);
            isDirty_ = true;

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
void StageEditorUI::MotionSelecter(const char* label, MotionType motionType, MotionConfig& motionConfig)
{
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
        const char* previewValue = motionConfig.name.empty() ? "Select Motion..." : motionConfig.name.c_str();

        // モーション名選択用のコンボボックスを描画
        if (ImGui::BeginCombo(label, previewValue))
        {
            for (const auto& name : motionNames)
            {
                // 現在のモーション名と同じものが選択されている状態にする
                bool isSelected = (motionConfig.name == name);
                if (ImGui::Selectable(name.c_str(), isSelected))
                {
                    motionConfig.name = name;
					motionConfig.handle = motionManager_->GetMotion(motionType, motionConfig.name);
                }
                if (isSelected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
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

/// @brief 保留中のアクションを実行する
/// @param placementList 
/// @param currentFileName 
void StageEditorUI::ExecutePendingAction(std::vector<PlacementData>& placementList, std::string& currentFileName)
{
    if (pendingAction_ == PendingAction::Load)
    {
		// ファイルを切り替える前に、現在の配置リストに基づいてすべての実体を消去する
        for (auto& data : placementList) spawner_->DeleteActualEntity(data);
        placementList.clear();
        currentFileName = pendingFileName_;
        fileManager_->LoadFromFile(currentFileName, placementList, spawner_);
    }
    else if (pendingAction_ == PendingAction::New)
    {
		// 新しいファイルを作成する前に、現在の配置リストに基づいてすべての実体を消去する
        for (auto& data : placementList) spawner_->DeleteActualEntity(data);
        placementList.clear();
        currentFileName = pendingFileName_;
        fileManager_->SaveToFile(currentFileName, placementList);
    }

    // 実行後はリセット
    pendingAction_ = PendingAction::None;
    pendingFileName_ = "";
}