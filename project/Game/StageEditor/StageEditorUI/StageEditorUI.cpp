#include "StageEditorUI.h"
#include <numbers>
#include "../StageFileManager/StageFileManager.h"
#include "../StageSpawner/StageSpawner.h"
#include "../StageEditorHistory/StageEditorHistory.h"

#include "Scene/GameScene/GameScene.h"

#include "Entity/Character/Character.h"
#include "Entity/Weapon/Weapon.h"
#include "StageObject/StageObject.h"
#include "NavMesh/NavMesh.h"

/// @brief 初期化
void StageEditorUI::Initialize()
{
	// モーションマネージャのインスタンスを取得
	motionManager_ = MotionManager::GetInstance();

	// ビヘイビアツリーデータの名前を読み込む
    LoadBehaviorTreeNames();

    // 入力キーの初期化
    inputModelChange_ = std::make_unique<InputKey>("StageEditor_ModelChange", InputState::Trigger, DIK_TAB);
}

/// @brief 更新処理
void StageEditorUI::Update()
{
    // ブジェクト配置モード と ナビメッシュ編集モード を切り替え
    if (inputModelChange_->IsInput())
    {
        if (currentMode_ == EditorMode::ObjectPlacement)
        {
            currentMode_ = EditorMode::NavMeshEdit;
        }
        else
        {
            currentMode_ = EditorMode::ObjectPlacement;
        }
    }
}

/// @brief UIの描画
/// @param placementList 
/// @param currentFileName 
/// @param isPlaying 
/// @param navMesh 
/// @param canExtrude 
/// @param canBridge 
void StageEditorUI::DrawUI(std::vector<PlacementData>& placementList, std::string& currentFileName, bool& isPlaying, NavMesh* navMesh, bool canExtrude, bool canBridge)
{
#ifdef _DEVELOPMENT

    if (!ImGui::Begin("Stage Editor")) 
    {
        // ウィンドウが折りたたまれている場合は終了
        ImGui::End();
        return;
    }

    // ファイルが選択されていない場合は、警告を表示してUIの描画を終了する
    if (currentFileName.empty())
    {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "ステージファイルが選択されていません。");
        ImGui::Text("「ステージプロジェクトアセット」ウィンドウからステージを選択または作成してください。");
        ImGui::End();
        return;
    }

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, 8.0f));

    /*--------------
        操作方法
    --------------*/

    // ショートカットキーの説明
    ImGui::Text("ショートカットキー 一覧");

    // --- 保存の表示 ---
    ImGui::Text("Ctrl + S : 上書き保存");

    // --- Undo の表示切り替え ---
    if (history_->CanUndo())
    {
        ImGui::Text("Ctrl + Z : Undo");
    }
    else
    {
        ImGui::TextColored(ImVec4(0.2f, 0.2f, 0.2f, 1.0f), "Ctrl + Z : Undo (無効)");
    }

    // --- Redo の表示切り替え ---
    if (history_->CanRedo())
    {
        ImGui::Text("Ctrl + Y : Redo");
    }
    else
    {
        ImGui::TextColored(ImVec4(0.2f, 0.2f, 0.2f, 1.0f), "Ctrl + Y : Redo (無効)");
    }

	// --- コピーの表示切り替え ---
	if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(placementList.size()))
	{
		ImGui::Text("Ctrl + C : コピー");
	}
	else
	{
		ImGui::TextColored(ImVec4(0.2f, 0.2f, 0.2f, 1.0f), "Ctrl + C : コピー (リスト内で選択時のみ有効)");
	}

	// --- 貼り付けの表示切り替え ---
    if (hasCopiedData_)
    {
		ImGui::Text("Ctrl + V : 貼り付け");

	}
    else
    {
        ImGui::TextColored(ImVec4(0.2f, 0.2f, 0.2f, 1.0f), "Ctrl + V : 貼り付け (無効)");
    }

	// --- 複製の表示切り替え ---
	if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(placementList.size()))
	{
		ImGui::Text("Ctrl + D : 複製");
	}
	else
	{
		ImGui::TextColored(ImVec4(0.2f, 0.2f, 0.2f, 1.0f), "Ctrl + D : 複製 (リスト内で選択時のみ有効)");
	}

	// --- 削除の表示切り替え ---
	if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(placementList.size()))
	{
		ImGui::Text("Delete / Backspace キー : 削除");
	}
	else
	{
		ImGui::TextColored(ImVec4(0.2f, 0.2f, 0.2f, 1.0f), "Delete / Backspace キー : 削除 (リスト内で選択時のみ有効)");
	}

	// --- モード切り替えの表示 ---
	ImGui::Text("Tab キー : モード切替 (現在のモード : %s)", currentMode_ == EditorMode::ObjectPlacement ? "オブジェクト配置" : "ナビメッシュ編集");

    ImGui::Separator();

    if (currentMode_ == EditorMode::ObjectPlacement)
    {

    }
    else if (currentMode_ == EditorMode::NavMeshEdit)
    {
        // 操作方法の説明
        ImGui::Text("1 キー : 頂点 選択");
        ImGui::Text("2 キー : 辺 選択");
        ImGui::Text("3 キー : 面 選択");

        // --- 押し出し (Extrude) の表示切り替え ---
        if (canExtrude)
        {
            // 成功時: 緑色でショートカットを表示
            ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "E キー : 押し出し");
        }
        else
        {
            // 失敗時: 灰色で無効表示にし、右側にオレンジ色でアドバイスを表示
            ImGui::TextColored(ImVec4(0.2f, 0.2f, 0.2f, 1.0f), "E キー : 押し出し (無効)");
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.0f, 1.0f), "<- 辺を1つ選択してください ( 2キー で 辺選択 )");
        }

        // --- ブリッジ (Bridge) の表示切り替え ---
        if (canBridge)
        {
            // 成功時: 緑色でショートカットを表示
            ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "B キー : ブリッジ");
        }
        else
        {
            // 失敗時: 灰色で無効表示にし、右側にオレンジ色でアドバイスを表示
            ImGui::TextColored(ImVec4(0.2f, 0.2f, 0.2f, 1.0f), "B キー : ブリッジ (無効)");
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.0f, 1.0f), "<- 辺を2つ選択してください ( 2キー で 辺選択 )");
        }
        ImGui::Separator();
    }



    /*---------
        生成
    ---------*/

    // 現在編集中のファイル名を表示
    ImGui::Text("ステージ : %s", currentFileName.c_str());

	// 実行 / 停止 ボタン
    if (isPlaying)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f)); // STOPボタンは赤色
        if (ImGui::Button("停止", ImVec2(100, 30)))
        {
            isPlaying = false;
			Entity::SetUpdateEnabled(false);// すべての実体の更新を停止する
			StageObject::SetUpdateEnabled(false); // すべてのステージオブジェクトの更新を停止する

			// プレイモードを終了したら、ゲームシーンをリセットして初期状態に戻す
            scene_->Reset();

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
        if (ImGui::Button("実行", ImVec2(100, 30)))
        {
            isPlaying = true;
			Entity::SetUpdateEnabled(true); // すべての実体の更新を再開する
			StageObject::SetUpdateEnabled(true); // すべてのステージオブジェクトの更新を再開する
        }
        ImGui::PopStyleColor();
    }
    

    ImGuiIO& io = ImGui::GetIO();
    
	// Ctrl + S で上書き保存 (プレイ中は保存できないようにする)
    if (!isPlaying)
    {
        if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_S))
        {
            fileManager_->SaveToFile(currentFileName, placementList, navMesh);
            isDirty_ = false;
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

        if (!io.WantTextInput)
        {
            // Ctrl + C でコピー
            if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_C))
            {
                // 有効なオブジェクトが選択されている場合のみコピー
                if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(placementList.size()))
                {
                    copiedData_ = placementList[selectedIndex_];

                    // 別ファイルへのペーストに対応するため、実体へのポインタはリセットする
                    copiedData_.instancePtr = nullptr;
                    hasCopiedData_ = true;
                }
            }

            // Ctrl + V でペースト
            if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_V))
            {
                if (hasCopiedData_)
                {
                    // 現在の状態を履歴に保存して変更フラグを立てる
                    history_->SaveHistory(placementList);
                    isDirty_ = true;

                    // クリップボードからデータを複製
                    PlacementData newData = copiedData_;

                    // 実体をシーンに生成してリストに追加
                    spawner_->SpawnActualEntity(newData);
                    placementList.push_back(newData);

                    // ペーストしたオブジェクトを自動的に選択状態にする
                    selectedIndex_ = static_cast<int>(placementList.size()) - 1;
                }
            }

			// Ctrl + D で複製
			if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_D))
			{
				// 有効なオブジェクトが選択されている場合のみ複製
				if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(placementList.size()))
				{
					// 現在の状態を履歴に保存して変更フラグを立てる
					history_->SaveHistory(placementList);
					isDirty_ = true;

					// 選択中のオブジェクトのデータを複製
					PlacementData newData = placementList[selectedIndex_];

					// 複製したオブジェクトは少し位置をずらして生成する
					newData.position.x += 0.5f; 
					newData.position.z += 0.5f;
                    newData.instancePtr = nullptr;

					// 実体をシーンに生成してリストに追加
					spawner_->SpawnActualEntity(newData);
					placementList.push_back(newData);

					// 複製したオブジェクトを自動的に選択状態にする
					selectedIndex_ = static_cast<int>(placementList.size()) - 1;
				}
			}

			// Delete / Backspace キーで削除
            if (ImGui::IsKeyPressed(ImGuiKey_Delete) || ImGui::IsKeyPressed(ImGuiKey_Backspace))
            {
                if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(placementList.size()))
                {
                    // 現在の状態を履歴に保存して変更フラグを立てる
                    history_->SaveHistory(placementList);
                    isDirty_ = true;

                    // シーンから実体を削除してリストからも削除
                    spawner_->DeleteActualEntity(placementList[selectedIndex_]);
                    placementList.erase(placementList.begin() + selectedIndex_);

                    // 選択状態をリセット
                    selectedIndex_ = -1;
                }
            }
        }
    }

    ImGui::Separator();

	// モード切替の表示
    if (currentMode_ == EditorMode::ObjectPlacement)
    {
		// オブジェクト配置モードのUIを描画
        ImGui::Text("--- オブジェクト配置 ---");

        static PlacementData currentData;
        static bool isInitialized = false;
        if (!isInitialized)
        {
            currentData.category = EditCategory::Character;
            currentData.subType = 0;
            currentData.position = Vector3(0.0f, 0.0f, 0.0f);
			currentData.rotate_ = Vector3(0.0f, 0.0f, 0.0f);
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
        if (ImGui::Combo("大分類", &intCat, categoryNames, IM_ARRAYSIZE(categoryNames)))
        {
            currentData.category = static_cast<EditCategory>(intCat);
            currentData.subType = 0; // 大分類が変わったら小分類のリセット
        }

        ImGui::Separator();

        // 大分類に応じて、小分類のコンボボックスの中身を切り替える
        if (currentData.category == EditCategory::Character)
        {
            ImGui::Combo("キャラクターの種類", &currentData.subType, characterTagNames, IM_ARRAYSIZE(characterTagNames));

            // 位置
            ImGui::DragFloat3("生成位置", &currentData.position.x, 0.1f);

            // HP
            ImGui::DragInt("HP", &currentData.hp, 1, 0, 10000);

            // 回転
            ImGui::DragFloat("回転", &currentData.rotate_.x, 0.01f, -std::numbers::pi_v<float>, std::numbers::pi_v<float>);

            // もしNPCが選ばれていたら、モーションの選択UIも表示する
            MotionSelecter("立ちモーション", MotionType::Stand, currentData.standMotion);
            MotionSelecter("戦闘モーション", MotionType::Stance, currentData.stanceMotion);
            MotionSelecter("歩行モーション", MotionType::Walk, currentData.walkMotion);
            MotionSelecter("ダッシュモーション", MotionType::Dash, currentData.dashMotion);
            MotionSelecter("前方回避モーション", MotionType::Avoid, currentData.avoidFrontMotion);
            MotionSelecter("後方回避モーション", MotionType::Avoid, currentData.avoidBackMotion);
            MotionSelecter("左回避モーション", MotionType::Avoid, currentData.avoidLeftMotion);
            MotionSelecter("右回避モーション", MotionType::Avoid, currentData.avoidRightMotion);

            // プレイヤーと未選択以外　ビヘイビアツリーデータ
            if (currentData.subType != 0 && currentData.subType != 1)
            {
                ImGui::Separator();
                ImGui::Text("ビヘイビアツリーの設定");

                // 開発中にファイルを新規作成した際、エディタを再起動せずにリストを更新できるボタン
                if (ImGui::Button("ビヘイビアツリー一覧を更新"))
                {
                    LoadBehaviorTreeNames();
                }

                // プレビュー用の文字列（未設定の場合は "Select Behavior Tree..." と表示）
                std::string currentBtName = currentData.behaviorScriptName;
                const char* previewBtValue = currentBtName.empty() ? "ビヘイビアツリーを選択..." : currentBtName.c_str();

                // プルダウンメニュー（コンボボックス）の描画
                if (ImGui::BeginCombo("ビヘイビアツリー", previewBtValue))
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
            ImGui::Combo("オブジェクトの種類", &currentData.subType, stageObjectTagNames, IM_ARRAYSIZE(stageObjectTagNames));

            // 位置
            ImGui::DragFloat3("生成位置", &currentData.position.x, 0.1f);

			// 回転
			ImGui::DragFloat3("回転Y", &currentData.rotate_.y, 0.01f, -std::numbers::pi_v<float>, std::numbers::pi_v<float>);

            // 拡縮
            ImGui::DragFloat3("大きさ", &currentData.scale.x, 0.1f, 0.0f, 10000.0f);
        }
        else if (currentData.category == EditCategory::Weapon)
        {
            ImGui::Combo("武器の種類", &currentData.subType, weaponCategoryNames, IM_ARRAYSIZE(weaponCategoryNames));

            // 位置
            ImGui::DragFloat3("生成位置", &currentData.position.x, 0.1f);

            // 耐久力
            ImGui::DragInt("耐久力", &currentData.durability, 1, 1, 10000);

            // 攻撃力
            ImGui::DragFloat("攻撃力", &currentData.attackPower, 0.1f, 0.0f, 10000.0f);

            // 壊れない武器かどうか
            ImGui::Checkbox("壊れるかどうか", &currentData.isUnbreakable);
        }

        ImGui::Separator();

        // 生成ボタン
        if (ImGui::Button("オブジェクトを生成"))
        {
            // 新しいオブジェクトを生成する前に、現在の配置リストの状態を履歴に保存する
            history_->SaveHistory(placementList);
            isDirty_ = true;

            // 新しい配置データを初期化
            PlacementData newData;
            newData.category = currentData.category;
            newData.subType = currentData.subType;
            newData.position = currentData.position;
            newData.rotate_ = currentData.rotate_;
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

        ImGui::Separator();
    }
	else if (currentMode_ == EditorMode::NavMeshEdit)
    {
        // ナビメッシュ編集モードのUIを描画
        ImGui::Text("--- ナビゲーションメッシュ ---");

		// ナビメッシュの島を追加するボタン
        if (ImGui::Button("ナビメッシュの島を追加"))
        {
            NavPolygon poly;
            poly.id = navMesh->GenerateNewPolygonId();

            // 新しい島の頂点を設定（XZ平面での正方形、Yは0固定）
            float size = 2.0f;
            poly.vertices[0] = Vector3(-size, 0.0f, -size);
            poly.vertices[1] = Vector3(-size, 0.0f, size);
            poly.vertices[2] = Vector3(size, 0.0f, size);
            poly.vertices[3] = Vector3(size, 0.0f, -size);

            // 完全に独立した新しい島なので、隣接IDはすべて無し (-1) に設定
            poly.neighborIds.fill(-1);

            // ナビメッシュに新しい島を追加
            navMesh->AddPolygon(poly);

            isDirty_ = true; // 変更があったフラグを立てる
        }

        ImGui::Separator();
    }

    ImGui::PopStyleVar();


    ImGui::End();

#endif
}

/// @brief アセットウィンドウの描画
/// @param placementList 
/// @param currentFileName 
/// @param isPlaying 
/// @param isDirty 
/// @param navMesh 
void StageEditorUI::DrawAssetWindow(std::vector<PlacementData>& placementList, std::string& currentFileName, bool& isPlaying, NavMesh* navMesh)
{
#ifdef _DEVELOPMENT

    if (!ImGui::Begin("Stage Project Assets")) 
    {
        ImGui::End();
        return;
    }

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, 8.0f));

    // 新規ステージの作成
    if (ImGui::Button("新規ステージ"))
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
        ImGui::InputText("ファイル名", newFileName, 64);

        // 作成ボタン
        if (ImGui::Button("作成"))
        {
			// 入力されたファイル名に ".json" 拡張子を付加する
            std::string fileName = newFileName;
            if (fileName.find(".json") == std::string::npos) fileName += ".json";

			// 実行中に新しいステージを作成する場合は、先に実行を停止する
            if (isPlaying)
            {
                isPlaying = false;
                Entity::SetUpdateEnabled(false);
                StageObject::SetUpdateEnabled(false);
            }

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
				// 変更が保存されていない場合でも、新しいステージを作成する前にすべての実体を消去する
                scene_->Reset();

				// すでにファイルが存在している場合は上書き保存する
                for (auto& data : placementList) spawner_->DeleteActualEntity(data);
                placementList.clear();
                currentFileName = fileName;
                fileManager_->SaveToFile(currentFileName, placementList, navMesh);
            }

            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    ImGui::Separator();
    ImGui::Text("保存ステージ:");
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
				// 実行中なら実行を停止する
                if (isPlaying)
                {
                    isPlaying = false;
                    Entity::SetUpdateEnabled(false);
                    StageObject::SetUpdateEnabled(false);
                }

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
					// 変更が保存されていない場合でも、ファイルを切り替える前にすべての実体を消去する
                    scene_->Reset();

					// すでにファイルが存在している場合は上書き保存する
                    for (auto& data : placementList) spawner_->DeleteActualEntity(data);
                    placementList.clear();
                    currentFileName = file;
                    fileManager_->LoadFromFile(currentFileName, placementList, spawner_, navMesh);
                }
            }

            // 右クリックメニュー
            if (ImGui::BeginPopupContextItem("FileContextMenu"))
            {
                // 読み込み
                if (ImGui::MenuItem("読み込み"))
                {
					// 実行中なら実行を停止する
                    if (isPlaying)
                    {
                        isPlaying = false;
                        Entity::SetUpdateEnabled(false);
                        StageObject::SetUpdateEnabled(false);
                    }

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
                        // 変更が保存されていない場合でも、新しいステージを作成する前にすべての実体を消去する
                        scene_->Reset();

						// すでにファイルが存在している場合は上書き保存する
                        for (auto& data : placementList) spawner_->DeleteActualEntity(data);
                        placementList.clear();
                        currentFileName = file;
                        fileManager_->LoadFromFile(currentFileName, placementList, spawner_, navMesh);
                    }
                }

				// 上書き保存 (プレイ中は保存できないようにする)
                if (ImGui::MenuItem("上書き保存", nullptr, false, !isPlaying))
                {
                    // 右クリックした対象を現在のファイルとして設定し、上書き保存する
                    currentFileName = file;
                    fileManager_->SaveToFile(currentFileName, placementList, navMesh);
                    isDirty_ = false;
                }

				// コピー
                if (ImGui::MenuItem("コピー", nullptr, false, !isPlaying))
                {
                    fileToCopy = file;
                }

                ImGui::Separator();

                // 削除は赤文字で表示
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
                if (ImGui::MenuItem("削除", nullptr, false, !isPlaying))
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
        ImGui::Text("変更が保存されていません。\n現在のステージを保存しますか？");
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // 保存ボタンを押して別のスクリプトを触る
        if (ImGui::Button("保存", ImVec2(140, 0)))
        {
            if (!currentFileName.empty())
            {
                fileManager_->SaveToFile(currentFileName, placementList, navMesh);
            }
            isDirty_ = false;
            ExecutePendingAction(placementList, currentFileName, navMesh);
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        // キャンセル(破棄)ボタンを押して別のスクリプトを触る
        if (ImGui::Button("キャンセル", ImVec2(140, 0)))
        {
            isDirty_ = false; // 変更を破棄したとみなす
            ExecutePendingAction(placementList, currentFileName, navMesh);
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        // 戻るボタンで今のスクリプトを触り続ける
        if (ImGui::Button("戻る", ImVec2(120, 0)))
        {
            pendingAction_ = PendingAction::None;
            pendingFileName_ = "";
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }



    // コピー対象が設定されていればモーダルを開く
    if (!fileToCopy.empty() && !ImGui::IsPopupOpen("Copy Stage"))
    {
        ImGui::OpenPopup("Copy Stage");
    }

    // コピー実行用のモーダルウィンドウ
    if (ImGui::BeginPopupModal("Copy Stage", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("'%s' に コピー:", fileToCopy.c_str());
        static char copyFileName[64] = "";

        ImGui::InputText("新規ファイル名", copyFileName, 64);

        if (ImGui::Button("Copy", ImVec2(120, 0)))
        {
            std::string newFileNameStr = copyFileName;
            if (!newFileNameStr.empty())
            {
                if (newFileNameStr.find(".json") == std::string::npos) newFileNameStr += ".json";

                // std::filesystem を使ってファイルを直接コピー
                try 
                {
					// コピー元とコピー先のパスを作成
                    std::filesystem::path srcPath = "./Assets/Parameter/StageData/" + fileToCopy;
                    std::filesystem::path destPath = "./Assets/Parameter/StageData/" + newFileNameStr;
                    std::filesystem::copy_file(srcPath, destPath, std::filesystem::copy_options::overwrite_existing);
                }
                catch (const std::exception& e) 
                {
                    // エラー時の処理 (必要であれば)
                    (void)e;
                }
            }

            fileToCopy = ""; // 対象をクリア
            copyFileName[0] = '\0'; // 入力欄をクリア
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

		// キャンセル ボタン
        if (ImGui::Button("キャンセル", ImVec2(120, 0)))
        {
            fileToCopy = "";
            copyFileName[0] = '\0';
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

        ImGui::Text("'%s' を削除してもよろしいですか？", fileToDelete.c_str());
        ImGui::Separator();

        // はい ボタン
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
        if (ImGui::Button("削除", ImVec2(120, 0)))
        {
            // 物理的にファイルを削除
            std::filesystem::remove("./Assets/Parameter/StageData/" + fileToDelete);

            // もし現在開いているファイルを削除した場合は、エディタの情報をリセットする
            if (currentFileName == targetName)
            {
                // 変更が保存されていない場合でも、新しいステージを作成する前にすべての実体を消去する
                scene_->Reset();

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
        if (ImGui::Button("キャンセル", ImVec2(120, 0)))
        {
            fileToDelete = ""; // キャンセル時は対象をクリアするだけ
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    ImGui::PopStyleVar();

    ImGui::End();
#endif
}

/// @brief オブジェクトリストウィンドウの描画
/// @param placementList 
/// @param navMesh 
void StageEditorUI::DrawObjectListWindow(std::vector<PlacementData>& placementList, NavMesh* navMesh)
{
#ifdef _DEVELOPMENT

    if (!ImGui::Begin("Object List")) 
    {
        ImGui::End();
        return;
    }


    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, 8.0f));

    ImGui::Text("配置されたオブジェクト :");


	// 項目の追加や削除があった場合に、走査中のリストが変更されてバグるのを防止するためのフラグ
    bool listChanged = false;

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

        // 各アイテムごとに一意のID空間を作る（右クリックメニューのバッティング防止）
        ImGui::PushID(i);

        // 選択されたら selectedIndex_ を更新
        if (ImGui::Selectable(label.c_str(), selectedIndex_ == i))
        {
            selectedIndex_ = i;
        }

        // 各項目に対する右クリックコンテキストメニュー
        if (ImGui::BeginPopupContextItem("ObjectItemContextMenu"))
        {
            selectedIndex_ = i; // 右クリックしたアイテムを自動的に選択状態にする

			// オブジェクトのコピー 
            if (ImGui::MenuItem("コピー"))
            {
                copiedData_ = placementList[i];

                // 新しいファイルで生成し直すため、実体へのポインタはリセットする
                copiedData_.instancePtr = nullptr;
                hasCopiedData_ = true;
            }

            // オブジェクトの複製
            if (ImGui::MenuItem("複製"))
            {
                history_->SaveHistory(placementList);
                isDirty_ = true;

                // 選択中のデータを複製
                PlacementData newData = placementList[i];

                // 完全に重ならないように位置を少しずらす
                newData.position.x += 0.5f;
                newData.position.z += 0.5f;
                newData.instancePtr = nullptr; // 新しい実体を作るため初期化

                // 実体を生成して追加
                spawner_->SpawnActualEntity(newData);
                placementList.push_back(newData);

                // 複製したオブジェクトを選択状態にする
                selectedIndex_ = static_cast<int>(placementList.size()) - 1;
                listChanged = true;
            }

            ImGui::Separator();

            // オブジェクトの消去 (削除)
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
            if (ImGui::MenuItem("削除"))
            {
                history_->SaveHistory(placementList);
                isDirty_ = true;

                // ゲーム内実体の削除
                spawner_->DeleteActualEntity(placementList[i]);
                // リストから削除
                placementList.erase(placementList.begin() + i);

                selectedIndex_ = -1; // 選択状態をリセット
                listChanged = true;
            }
            ImGui::PopStyleColor();

            ImGui::EndPopup();
        }

        ImGui::PopID();

        // 項目が追加または削除されたら、安全のためにこのフレームの走査を終了する
        if (listChanged)
        {
            break;
        }
    }

	// アイテムの上以外で右クリックされたら、全体のコンテキストメニューを開く
    if (hasCopiedData_)
    {
		// 右クリックされた位置にアイテムがない場合は、全体のコンテキストメニューを開く
        if (ImGui::BeginPopupContextWindow("ObjectListPasteRegionMenu", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
        {
            if (ImGui::MenuItem("貼り付け"))
            {
                history_->SaveHistory(placementList);
                isDirty_ = true;

				// コピーしたデータを新しいオブジェクトとして生成
                PlacementData newData = copiedData_;
                spawner_->SpawnActualEntity(newData);
                placementList.push_back(newData);
                selectedIndex_ = static_cast<int>(placementList.size()) - 1;
                listChanged = true;
            }
            ImGui::EndPopup();
        }
    }

    ImGui::EndChild();


    ImGui::Separator();


    // 選択中のオブジェクトがある場合、編集UIを表示
    if (selectedIndex_ >= 0 && selectedIndex_ < placementList.size())
    {
        auto& target = placementList[selectedIndex_];
        ImGui::Text("--- 編集中のオブジェクト ---");

        // カテゴリごとの編集項目
        if (target.category == EditCategory::Character)
        {
			Character* charPtr = static_cast<Character*>(target.instancePtr);
			charPtr->DrawDebugUI(&target, placementList, history_, &isDirty_);

            // アニメーション変更フラグを初期化
            isChangeAnimation_ = false;

			// モーション選択UI
            MotionSelecter("待機モーション", MotionType::Stand, target.standMotion, placementList);
            MotionSelecter("戦闘モーション", MotionType::Stance, target.stanceMotion, placementList);
            MotionSelecter("歩行モーション", MotionType::Walk, target.walkMotion, placementList);
            MotionSelecter("ダッシュモーション", MotionType::Dash, target.dashMotion, placementList);
            MotionSelecter("前方回避モーション", MotionType::Avoid, target.avoidFrontMotion, placementList);
            MotionSelecter("後方回避モーション", MotionType::Avoid, target.avoidBackMotion, placementList);
            MotionSelecter("左回避モーション", MotionType::Avoid, target.avoidLeftMotion, placementList);
            MotionSelecter("右回避モーション", MotionType::Avoid, target.avoidRightMotion, placementList);

			// もしモーションのどれかが変更されたら、実際のキャラクターオブジェクトにアニメーションハンドルを更新する
            if (isChangeAnimation_)
            {
				Character::AnimationHandleData animData;
				animData.hStandMotion = motionManager_->GetMotion(MotionType::Stand, target.standMotion.name);
				animData.hStanceMotion = motionManager_->GetMotion(MotionType::Stance, target.stanceMotion.name);
				animData.hWalkMotion = motionManager_->GetMotion(MotionType::Walk, target.walkMotion.name);
				animData.hDashMotion = motionManager_->GetMotion(MotionType::Dash, target.dashMotion.name);
				animData.hAvoidFrontMotion = motionManager_->GetMotion(MotionType::Avoid, target.avoidFrontMotion.name);
				animData.hAvoidBackMotion = motionManager_->GetMotion(MotionType::Avoid, target.avoidBackMotion.name);
				animData.hAvoidLeftMotion = motionManager_->GetMotion(MotionType::Avoid, target.avoidLeftMotion.name);
				animData.hAvoidRightMotion = motionManager_->GetMotion(MotionType::Avoid, target.avoidRightMotion.name);
				
				charPtr->SetAnimationHandle(animData);

				isChangeAnimation_ = false; // フラグをリセット
            }

            // プレイヤーと未選択以外　ビヘイビアツリーデータ
            if (target.subType != 0 && target.subType != 1)
            {
                ImGui::Separator();
                ImGui::Text("ビヘイビアツリーの設定");

                // 開発中にファイルを新規作成した際、エディタを再起動せずにリストを更新できるボタン
                if (ImGui::Button("リストを更新"))
                {
                    LoadBehaviorTreeNames();
                }

                // プレビュー用の文字列（未設定の場合は "Select Behavior Tree..." と表示）
                std::string currentBtName = target.behaviorScriptName;
                const char* previewBtValue = currentBtName.empty() ? "ビヘイビアツリーを選択..." : currentBtName.c_str();

                // プルダウンメニュー（コンボボックス）の描画
                if (ImGui::BeginCombo("ビヘイビアツリー", previewBtValue))
                {
                    for (const auto& name : behaviorTreeNames_)
                    {
						// 現在のビヘイビアツリー名と同じものが選択されている状態にする
                        bool isSelected = (currentBtName == name);
                        if (ImGui::Selectable(name.c_str(), isSelected))
                        {
							// ビヘイビアツリーを変更する前に、現在の配置リストの状態を履歴に保存する
                            history_->SaveHistory(placementList);
                            isDirty_ = true;

							// 選択された名前を PlacementData の配列にコピーする
                            strcpy_s(target.behaviorScriptName, sizeof(target.behaviorScriptName), name.c_str());

							// 実際のキャラクターオブジェクトにビヘイビアツリーを更新する
                            charPtr->SetBehaviorTree(scene_->GetBehaviorTreeEditor()->CreateTree(target.behaviorScriptName, charPtr));
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
    }

    ImGui::PopStyleVar();

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
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "モーションがロードされていません");
    }
    else
    {
        // 現在選択されているモーション名をプレビュー用の文字列として設定
        const char* previewValue = motionConfig.name.empty() ? "モーションを選択..." : motionConfig.name.c_str();

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

/// @brief モーションの選択UIを表示する
/// @param motionType 
/// @param motionName 
/// @param placementList
void StageEditorUI::MotionSelecter(const char* label, MotionType motionType, MotionConfig& motionConfig, std::vector<PlacementData>& placementList)
{
    // 選択されたモーションタイプに応じたモーション名のリストをMotionManagerから取得
    std::vector<std::string> motionNames = MotionManager::GetInstance()->GetMotionNames(motionType);

    // モーション名のリストが空の場合はエラーメッセージを表示
    if (motionNames.empty())
    {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "モーションがロードされていません");
    }
    else
    {
        // 現在選択されているモーション名をプレビュー用の文字列として設定
        const char* previewValue = motionConfig.name.empty() ? "モーションを選択..." : motionConfig.name.c_str();

        // モーション名選択用のコンボボックスを描画
        if (ImGui::BeginCombo(label, previewValue))
        {
            for (const auto& name : motionNames)
            {
                // 現在のモーション名と同じものが選択されている状態にする
                bool isSelected = (motionConfig.name == name);
                if (ImGui::Selectable(name.c_str(), isSelected))
                {
					// モーションを変更する前に、現在の配置リストの状態を履歴に保存する
					history_->SaveHistory(placementList);
					isDirty_ = true;

                    motionConfig.name = name;
                    motionConfig.handle = motionManager_->GetMotion(motionType, motionConfig.name);

					isChangeAnimation_ = true; // モーションが変更されたフラグを立てる
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
/// @param navMesh 
void StageEditorUI::ExecutePendingAction(std::vector<PlacementData>& placementList, std::string& currentFileName, NavMesh* navMesh)
{
    if (pendingAction_ == PendingAction::Load)
    {
		// ファイルを切り替える前に、現在の配置リストに基づいてすべての実体を消去する
        scene_->Reset();

		// ファイルを切り替える前に、現在の配置リストに基づいてすべての実体を消去する
        for (auto& data : placementList) spawner_->DeleteActualEntity(data);
        placementList.clear();
        currentFileName = pendingFileName_;
        fileManager_->LoadFromFile(currentFileName, placementList, spawner_, navMesh);
    }
    else if (pendingAction_ == PendingAction::New)
    {
        // ファイルを切り替える前に、現在の配置リストに基づいてすべての実体を消去する
        scene_->Reset();

		// 新しいファイルを作成する前に、現在の配置リストに基づいてすべての実体を消去する
        for (auto& data : placementList) spawner_->DeleteActualEntity(data);
        placementList.clear();
        currentFileName = pendingFileName_;
        fileManager_->SaveToFile(currentFileName, placementList, navMesh);
    }

    // 実行後はリセット
    pendingAction_ = PendingAction::None;
    pendingFileName_ = "";
}