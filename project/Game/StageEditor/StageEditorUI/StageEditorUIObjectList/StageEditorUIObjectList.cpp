#include "StageEditorUIObjectList.h"
#include "StageEditor/StageSpawner/StageSpawner.h"
#include "StageEditor/StageEditorHistory/StageEditorHistory.h"
#include "BehaviorTree/BehaviorTreeEditor/BehaviorTreeEditor.h"
#include <numbers>

#include "Entity/Character/NPC/NPC.h"
#include "Entity/Character/Player/Player.h"
#include "Entity/Weapon/Weapon.h"

#include "StageObject/Floor/Floor.h"
#include "StageObject/Wall/Wall.h"
#include "StageObject/StaticEventTrigger/StaticEventTrigger.h"

/// @brief コンストラクタ
    /// @param spawner 
    /// @param history 
StageEditorUIObjectList::StageEditorUIObjectList(StageSpawner* spawner, StageEditorHistory* history, BehaviorTreeEditor* behaviorTreeEditor)
	: spawner_(spawner), history_(history), behaviorTreeEditor_(behaviorTreeEditor)
{
	// モーションマネージャのインスタンスを取得
	motionManager_ = MotionManager::GetInstance();
}

/// @brief 配置されたオブジェクトのリストを描画する
/// @param placementList 
/// @param selectedIndex 
/// @param isDirty 
/// @param navMesh 
void StageEditorUIObjectList::DrawWindow(std::vector<PlacementData>& placementList, int& selectedIndex, bool& isDirty,
    bool& hasCopiedData, PlacementData& copiedData, NavMesh* navMesh, const std::vector<std::string> behaviorTreeNames)
{
    if (!ImGui::Begin("Object List"))
    {
        ImGui::End();
        return;
    }


	// 選択されているオブジェクトがあれば、そのオブジェクトをギズモで操作できるようにする
    if (selectedIndex >= 0 && selectedIndex < placementList.size())
    {
        auto& data = placementList[selectedIndex];

        // カメラのビュー行列とプロジェクション行列を取得
        Matrix4x4 viewMatrix = engine_->GetCamera3DView();
        Matrix4x4 projectionMatrix = engine_->GetCamera3DProjection();

        // 現在のSRTからワールド行列を一度だけ生成する
        Quaternion rotateQ =
            ToQuaternion(data.rotate_.z, Vector3(0.0f, 0.0, 1.0f)).Normalize() *
            ToQuaternion(data.rotate_.y, Vector3(0.0f, 1.0, 0.0f)).Normalize() *
            ToQuaternion(data.rotate_.x, Vector3(1.0f, 0.0, 0.0f)).Normalize();

        // ワールド行列
        Matrix4x4 worldMatrix =
            Make3DScaleMatrix4x4(data.scale) *
            Make3DRotateMatrix4x4(rotateQ) *
            Make3DTranslateMatrix4x4(data.position);

        // ギズモの操作モード（移動・回転・拡縮）を切り替えるためのUI（ショートカットキー等と連動させると便利です）
        static ImGuizmo::OPERATION currentOperation = ImGuizmo::TRANSLATE;
        static ImGuizmo::MODE currentMode = ImGuizmo::LOCAL;
		static bool useSnap = false;

        // キーボード入力などで操作モードを切り替える例
        if (ImGui::IsKeyPressed(ImGuiKey_T)) currentOperation = ImGuizmo::TRANSLATE;
        if (ImGui::IsKeyPressed(ImGuiKey_R)) currentOperation = ImGuizmo::ROTATE;
        if (ImGui::IsKeyPressed(ImGuiKey_S)) currentOperation = ImGuizmo::SCALE;

        // ギズモの描画と操作
        ImGuizmo::Manipulate(&viewMatrix.m[0][0], &projectionMatrix.m[0][0], currentOperation, currentMode, &worldMatrix.m[0][0]);

        // ギズモを使ってオブジェクトを操作中かどうかをチェック
        if (ImGuizmo::IsUsing() && useSnap)
        {
            float translation[3];
            float rotation[3];
            float scale[3];

            // ワールド行列からSRT成分を抽出
            ImGuizmo::DecomposeMatrixToComponents(&worldMatrix.m[0][0], translation, rotation, scale);

			// 回転はImGuizmoが度単位で返すので、ラジアンに変換するための定数
            constexpr float DEG2RAD = std::numbers::pi_v<float> / 180.0f;

            switch (currentOperation)
            {
            case ImGuizmo::TRANSLATE:
                // 移動成分抽出
                data.position = Vector3(translation[0], translation[1], translation[2]);
                break;
            case ImGuizmo::ROTATE:
                // 回転成分抽出
                data.rotate_.x = rotation[0] * DEG2RAD;
                data.rotate_.y = rotation[1] * DEG2RAD;
                data.rotate_.z = rotation[2] * DEG2RAD;
                break;
            case ImGuizmo::SCALE:
                // 拡縮成分抽出
                data.scale = Vector3(scale[0], scale[1], scale[2]);
                break;
            }

			// ギズモで操作した結果をゲーム内の実体に反映させる
            if (data.category == EditCategory::Character || data.category == EditCategory::Weapon)
            {
                auto entityPtr = static_cast<Entity*>(data.instancePtr);
                entityPtr->SetPosition(data.position);
                entityPtr->SetRotation(data.rotate_);
                entityPtr->SetScale(data.scale);
            }
            else if (data.category == EditCategory::Object)
            {
				auto stageObjectPtr = static_cast<StageObject*>(data.instancePtr);
                stageObjectPtr->SetPosition(data.position);
                stageObjectPtr->SetRotation(data.rotate_);
                stageObjectPtr->SetScale(data.scale);
            }
        }
        else
        {
			// ギズモを操作していないときは、ゲーム内の実体の位置を配置データに反映させる
            if (data.category == EditCategory::Character || data.category == EditCategory::Weapon)
            {
                auto entityPtr = static_cast<Entity*>(data.instancePtr);
				data.position = entityPtr->GetWorldTransform()->translate_;
				data.rotate_ = entityPtr->GetWorldTransform()->rotate_;
				data.scale = entityPtr->GetWorldTransform()->scale_;
            }
            else if (data.category == EditCategory::Object)
            {
                auto stageObjectPtr = static_cast<StageObject*>(data.instancePtr);
				data.position = stageObjectPtr->GetWorldTransform()->translate_;
				data.rotate_ = stageObjectPtr->GetWorldTransform()->rotate_;
				data.scale = stageObjectPtr->GetWorldTransform()->scale_;
            }
        }

		// Guizmoを触った瞬間に配置データの変更を確定させる（スナップ機能を使用していない場合）
        if (ImGuizmo::IsUsing() && !useSnap)
        {
            // 配置データを更新
            history_->SaveHistory(placementList);
            isDirty = true;
        }

		// ギズモを操作している間は、スナップ機能を有効にする例（オプション）
        useSnap = ImGuizmo::IsUsing();
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
        if (data.name[0] == '\0')
        {
            if (data.category == EditCategory::Character) label += characterTagNames[data.subType];
            else if (data.category == EditCategory::Object) label += stageObjectTagNames[data.subType];
            else if (data.category == EditCategory::Weapon) label += weaponCategoryNames[data.subType];
        } else
        {
            label += data.name; // 名前が設定されていればそちらを優先して表示
        }

        // 各アイテムごとに一意のID空間を作る（右クリックメニューのバッティング防止）
        ImGui::PushID(i);

        // 選択されたら selectedIndex_ を更新
        if (ImGui::Selectable(label.c_str(), selectedIndex == i))
        {
            selectedIndex = i;
        }

        // 各項目に対する右クリックコンテキストメニュー
        if (ImGui::BeginPopupContextItem("ObjectItemContextMenu"))
        {
            selectedIndex = i; // 右クリックしたアイテムを自動的に選択状態にする

            // オブジェクトのコピー 
            if (ImGui::MenuItem("コピー"))
            {
                copiedData = placementList[i];

                // 新しいファイルで生成し直すため、実体へのポインタはリセットする
                copiedData.instancePtr = nullptr;
                hasCopiedData = true;
            }

            // オブジェクトの複製
            if (ImGui::MenuItem("複製"))
            {
                history_->SaveHistory(placementList);
                isDirty = true;

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
                selectedIndex = static_cast<int>(placementList.size()) - 1;
                listChanged = true;
            }

            ImGui::Separator();

            // オブジェクトの消去 (削除)
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
            if (ImGui::MenuItem("削除"))
            {
                history_->SaveHistory(placementList);
                isDirty = true;

                // ゲーム内実体の削除
                spawner_->DeleteActualEntity(placementList[i]);
                // リストから削除
                placementList.erase(placementList.begin() + i);

                selectedIndex = -1; // 選択状態をリセット
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
    if (hasCopiedData)
    {
        // 右クリックされた位置にアイテムがない場合は、全体のコンテキストメニューを開く
        if (ImGui::BeginPopupContextWindow("ObjectListPasteRegionMenu", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
        {
            if (ImGui::MenuItem("貼り付け"))
            {
                history_->SaveHistory(placementList);
                isDirty = true;

                // コピーしたデータを新しいオブジェクトとして生成
                PlacementData newData = copiedData;
                spawner_->SpawnActualEntity(newData);
                placementList.push_back(newData);
                selectedIndex = static_cast<int>(placementList.size()) - 1;
                listChanged = true;
            }
            ImGui::EndPopup();
        }
    }

    ImGui::EndChild();


    ImGui::Separator();


    // 選択中のオブジェクトがある場合、編集UIを表示
	if (selectedIndex >= 0 && selectedIndex < placementList.size())
    {
        auto& target = placementList[selectedIndex];
        ImGui::Text("--- 編集中のオブジェクト ---");

        // オブジェクト名の編集
        ImGui::InputText("オブジェクト名", target.name, sizeof(target.name));

        ImGui::Separator();

        // カテゴリごとの編集項目
        if (target.category == EditCategory::Character)
        {
            // 選択されたオブジェクトの実体をキャラクター型として扱う
            Character* charPtr = static_cast<Character*>(target.instancePtr);
            charPtr->DrawDebugUI(&target, placementList, history_, &isDirty);

            // アニメーション変更フラグを初期化
            isChangeAnimation_ = false;

            // モーション選択UI
            MotionSelecter("待機モーション", MotionType::Stand, target.standMotion, placementList, isDirty);
            MotionSelecter("戦闘モーション", MotionType::Stance, target.stanceMotion, placementList, isDirty);
            MotionSelecter("歩行モーション", MotionType::Walk, target.walkMotion, placementList, isDirty);
            MotionSelecter("ダッシュモーション", MotionType::Dash, target.dashMotion, placementList, isDirty);
            MotionSelecter("前方回避モーション", MotionType::Avoid, target.avoidFrontMotion, placementList, isDirty);
            MotionSelecter("後方回避モーション", MotionType::Avoid, target.avoidBackMotion, placementList, isDirty);
            MotionSelecter("左回避モーション", MotionType::Avoid, target.avoidLeftMotion, placementList, isDirty);
            MotionSelecter("右回避モーション", MotionType::Avoid, target.avoidRightMotion, placementList, isDirty);

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

                // プレビュー用の文字列（未設定の場合は "Select Behavior Tree..." と表示）
                std::string currentBtName = target.behaviorScriptName;
                const char* previewBtValue = currentBtName.empty() ? "ビヘイビアツリーを選択..." : currentBtName.c_str();

                // プルダウンメニュー（コンボボックス）の描画
                if (ImGui::BeginCombo("ビヘイビアツリー", previewBtValue))
                {
                    for (const auto& name : behaviorTreeNames)
                    {
                        // 現在のビヘイビアツリー名と同じものが選択されている状態にする
                        bool isSelected = (currentBtName == name);
                        if (ImGui::Selectable(name.c_str(), isSelected))
                        {
                            // ビヘイビアツリーを変更する前に、現在の配置リストの状態を履歴に保存する
                            history_->SaveHistory(placementList);
                            isDirty = true;

                            // 選択された名前を PlacementData の配列にコピーする
                            strcpy_s(target.behaviorScriptName, sizeof(target.behaviorScriptName), name.c_str());

                            // 実際のキャラクターオブジェクトにビヘイビアツリーを更新する
                            charPtr->SetBehaviorTree(behaviorTreeEditor_->CreateTree(target.behaviorScriptName, charPtr));
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
            if (target.subType == static_cast<int>(StageObject::StageObjectTag::Floor))
            {
                // フロアオブジェクトの場合、特定のUIを表示する
                Floor* floorPtr = static_cast<Floor*>(target.instancePtr);
                floorPtr->DrawDebugUI(&target, placementList, history_, &isDirty);
            } 
            else if (target.subType == static_cast<int>(StageObject::StageObjectTag::Wall))
            {
                // 壁オブジェクトの場合、特定のUIを表示する
                Wall* wallPtr = static_cast<Wall*>(target.instancePtr);
                wallPtr->DrawDebugUI(&target, placementList, history_, &isDirty);
            } 
            else if (target.subType == static_cast<int>(StageObject::StageObjectTag::StaticEventTrigger))
            {
                // イベントトリガーオブジェクトの場合、特定のUIを表示する
                StaticEventTrigger* eventTriggerPtr = static_cast<StaticEventTrigger*>(target.instancePtr);

                // イベントトリガーオブジェクトの場合、特定のUIを表示する
                if (eventTriggerPtr != nullptr)
                {
                    eventTriggerPtr->DrawDebugUI(&target, placementList, history_, &isDirty);

                    ImGui::Unindent(); // 見栄え調整用（必要に応じて）
                    ImGui::Separator();
                    ImGui::TextColored(ImVec4(0.2f, 0.8f, 1.0f, 1.0f), "--- イベントトリガー設定 ---");
                    ImGui::Indent();


                    const char* eventTypes[] = { "None (なし)", "敵生成" };
                    int currentType = target.eventType;

                    if (ImGui::Combo("イベントタイプ", &currentType, eventTypes, IM_ARRAYSIZE(eventTypes)))
                    {
                        // 変更前に履歴を保存
                        history_->SaveHistory(placementList);
                        isDirty = true;

                        // データを更新
                        target.eventType = currentType;
                        eventTriggerPtr->SetEventType(currentType);

                        // 配置されている実体を再生成して反映する
                        spawner_->SpawnActualEntity(target);
                    }

                    if (ImGui::TreeNode("イベントパラメータ"))
                    {


                        // イベントタイプに応じた追加のパラメータ編集UI
                        switch (static_cast<StaticEventTrigger::EventType>(target.eventType))
                        {
                            // オブジェクト生成イベント
                        case StaticEventTrigger::EventType::ObjectSpawn:

                            // オブジェクトのスポーン設定を保持する構造体のリスト
                            std::vector<PlacementData> objects;
                            bool isInitialEmpty = false;

                            // まずは既存のJSON文字列を解析して、敵のリストを構築する
                            try
                            {
                                if (strlen(target.eventStringParam) > 0)
                                {
                                    nlohmann::json j = nlohmann::json::parse(target.eventStringParam);
                                    if (j.is_array())
                                    {
                                        for (const auto& enemyData : j)
                                        {
                                            PlacementData spawnData;
											fromJson(enemyData, spawnData);

                                            objects.push_back(spawnData);
                                        }
                                    }
                                } else
                                {
                                    // 初期状態で空のパラメータだったことを記録
                                    isInitialEmpty = true;
                                }
                            }
                            // JSONの解析に失敗した場合は、エラーを無視して空のリストとして扱う
                            catch (...)
                            {
                                objects.clear();
                                isInitialEmpty = true;
                            }

                            // 初期状態で空だった場合、かつ解析の結果敵のリストが空になってしまった場合は、デフォルトの敵を1体追加しておく
                            if (isInitialEmpty && objects.empty())
                            {
                                PlacementData defaultObject;
                                defaultObject.position = target.position;
                                objects.push_back(defaultObject);
                            }


                            // GUI操作によるパラメータ変更があったかどうかを示すフラグ
                            bool isParamChanged = false;
                            int enemyToDelete = -1; // このフレームで削除する敵のインデックスを保持する変数

                            // 選択されている敵のインデックスを保持（staticにすることでフレーム間をまたいで記憶）
                            static int selectedEnemyIndex = 0;

                            // 敵を削除してインデックスが範囲外になった場合の安全対策
                            if (selectedEnemyIndex >= objects.size())
                            {
                                selectedEnemyIndex = objects.empty() ? -1 : static_cast<int>(objects.size()) - 1;
                            }

                            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "[生成する敵のリスト]");

                            // 敵のリスト表示と選択
                            ImGui::BeginChild("EnemyListChild", ImVec2(0, 150), true);
                            for (size_t i = 0; i < objects.size(); ++i)
                            {
                                auto& data = objects[i];

                                // 表示用のラベルを作成 
                                std::string label = "ID:" + std::to_string(i) + " ";
                                if (data.name[0] == '\0')
                                {
                                    if (data.category == EditCategory::Character) label += characterTagNames[data.subType];
                                    else if (data.category == EditCategory::Object) label += stageObjectTagNames[data.subType];
                                    else if (data.category == EditCategory::Weapon) label += weaponCategoryNames[data.subType];
                                } else
                                {
                                    label += data.name; // 名前が設定されていればそちらを優先して表示
                                }

                                // Selectableを使ってリスト表示
                                bool isSelected = (selectedEnemyIndex == static_cast<int>(i));
                                if (ImGui::Selectable(label.c_str(), isSelected))
                                {
                                    selectedEnemyIndex = static_cast<int>(i);
                                }
                            }
                            ImGui::EndChild();

                            // 選択されているオブジェクトのパラメータ編集
                            if (selectedEnemyIndex >= 0 && selectedEnemyIndex < static_cast<int>(objects.size()))
                            {
                                auto& object = objects[selectedEnemyIndex];

                                // 大分類の選択
                                int intCat = static_cast<int>(object.category);
                                if (ImGui::Combo("大分類", &intCat, categoryNames, IM_ARRAYSIZE(categoryNames)))
                                {
                                    object.category = static_cast<EditCategory>(intCat);
                                    object.subType = 0; // 大分類が変わったら小分類のリセット

                                    // パラメータ変更フラグを立てる
                                    isParamChanged = true;
                                }

                                ImGui::Separator();

                                // 大分類に応じて、小分類のコンボボックスの中身を切り替える
                                if (object.category == EditCategory::Character)
                                {
                                    if (ImGui::Combo("キャラクターの種類", &object.subType, characterTagNames, IM_ARRAYSIZE(characterTagNames)))isParamChanged = true;

                                    // 位置
                                    if (ImGui::DragFloat3("生成位置", &object.position.x, 0.1f))
                                    {
                                        isParamChanged = true;
                                    }

                                    // HP
                                    if (ImGui::DragInt("HP", &object.hp, 1, 0, 10000))
                                    {
                                        isParamChanged = true;
                                    }

                                    // 回転
                                    if (ImGui::DragFloat("回転", &object.rotate_.x, 0.01f, -std::numbers::pi_v<float>, std::numbers::pi_v<float>))
                                    {
                                        isParamChanged = true;
                                    }

                                    // もしNPCが選ばれていたら、モーションの選択UIも表示する
                                    if (MotionSelecter("立ちモーション", MotionType::Stand, object.standMotion))isParamChanged = true;
                                    if (MotionSelecter("戦闘モーション", MotionType::Stance, object.stanceMotion))isParamChanged = true;
                                    if (MotionSelecter("歩行モーション", MotionType::Walk, object.walkMotion))isParamChanged = true;
                                    if (MotionSelecter("ダッシュモーション", MotionType::Dash, object.dashMotion))isParamChanged = true;
                                    if (MotionSelecter("前方回避モーション", MotionType::Avoid, object.avoidFrontMotion))isParamChanged = true;
                                    if (MotionSelecter("後方回避モーション", MotionType::Avoid, object.avoidBackMotion))isParamChanged = true;
                                    if (MotionSelecter("左回避モーション", MotionType::Avoid, object.avoidLeftMotion))isParamChanged = true;
                                    if (MotionSelecter("右回避モーション", MotionType::Avoid, object.avoidRightMotion))isParamChanged = true;

                                    // プレイヤーと未選択以外　ビヘイビアツリーデータ
                                    if (object.subType != 0 && object.subType != 1)
                                    {
                                        ImGui::Separator();
                                        ImGui::Text("ビヘイビアツリーの設定");

                                        // プレビュー用の文字列（未設定の場合は "Select Behavior Tree..." と表示）
                                        std::string currentBtName = object.behaviorScriptName;
                                        const char* previewBtValue = currentBtName.empty() ? "ビヘイビアツリーを選択..." : currentBtName.c_str();

                                        // プルダウンメニュー（コンボボックス）の描画
                                        if (ImGui::BeginCombo("ビヘイビアツリー", previewBtValue))
                                        {
                                            for (const auto& name : behaviorTreeNames)
                                            {
                                                bool isSelected = (currentBtName == name);
                                                if (ImGui::Selectable(name.c_str(), isSelected))
                                                {
                                                    // 選択された名前を PlacementData の配列にコピーする
                                                    // ※Visual Studio環境なら strcpy_s を使用して安全にコピーします
                                                    strcpy_s(object.behaviorScriptName, sizeof(object.behaviorScriptName), name.c_str());

                                                    // パラメータ変更フラグを立てる
                                                    isParamChanged = true;
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
                                } else if (object.category == EditCategory::Object)
                                {
                                    if (ImGui::Combo("オブジェクトの種類", &object.subType, stageObjectTagNames, IM_ARRAYSIZE(stageObjectTagNames)))
                                    {
                                        isParamChanged = true;
                                    }

                                    // 床
                                    if (static_cast<StageObject::StageObjectTag>(object.subType) == StageObject::StageObjectTag::Floor)
                                    {
                                        // 位置
                                        if (ImGui::DragFloat3("生成位置", &object.position.x, 0.1f))
                                        {
                                            isParamChanged = true;
                                        }

                                        // 拡縮
                                        if (ImGui::DragFloat3("大きさ", &object.scale.x, 0.1f, 0.0f, 10000.0f))
                                        {
                                            isParamChanged = true;
                                        }
                                    } else if (static_cast<StageObject::StageObjectTag>(object.subType) == StageObject::StageObjectTag::Wall)
                                    {
                                        // 位置
                                        if (ImGui::DragFloat3("生成位置", &object.position.x, 0.1f))
                                        {
                                            isParamChanged = true;
                                        }

                                        // 回転
                                        if (ImGui::DragFloat("回転Y", &object.rotate_.y, 0.01f, -std::numbers::pi_v<float>, std::numbers::pi_v<float>))
                                        {
                                            isParamChanged = true;
                                        }

                                        // 拡縮
                                        if (ImGui::DragFloat3("大きさ", &object.scale.x, 0.1f, 0.0f, 10000.0f))
                                        {
                                            isParamChanged = true;
                                        }
                                    } else if (static_cast<StageObject::StageObjectTag>(object.subType) == StageObject::StageObjectTag::StaticEventTrigger)
                                    {
                                        // 位置
                                        if (ImGui::DragFloat3("生成位置", &object.position.x, 0.1f))
                                        {
                                            isParamChanged = true;
                                        }

                                        // 拡縮
                                        if (ImGui::DragFloat3("大きさ", &object.scale.x, 0.1f, 0.0f, 10000.0f))
                                        {
                                            isParamChanged = true;
                                        }

                                        //// イベントの種類
                                        //ImGui::Combo("イベントタイプ", &object.eventType, eventTypeNames, IM_ARRAYSIZE(eventTypeNames));
                                        //
                                        //// イベントの種類によって、パラメータの内容が異なる（今はすべて文字列パラメータとして扱う）
                                        //ImGui::InputText("イベントパラメータ", object.eventStringParam, sizeof(object.eventStringParam));
                                    }
                                } else if (object.category == EditCategory::Weapon)
                                {
                                    ImGui::Combo("武器の種類", &object.subType, weaponCategoryNames, IM_ARRAYSIZE(weaponCategoryNames));

                                    // 位置
                                    if (ImGui::DragFloat3("生成位置", &object.position.x, 0.1f))
                                    {
                                        isParamChanged = true;
                                    }

                                    // 耐久力
                                    if (ImGui::DragInt("耐久力", &object.durability, 1, 1, 10000))
                                    {
                                        isParamChanged = true;
                                    }

                                    // 攻撃力
                                    if (ImGui::DragFloat("攻撃力", &object.attackPower, 0.1f, 0.0f, 10000.0f))
                                    {
                                        isParamChanged = true;
                                    }

                                    // 壊れない武器かどうか
                                    if (ImGui::Checkbox("壊れるかどうか", &object.isUnbreakable))
                                    {
                                        isParamChanged = true;
                                    }
                                }

                                ImGui::Separator();
                            }



                            ImGui::Spacing();

                            // 個別の削除ボタン
                            if (ImGui::Button("この敵を削除"))
                            {
                                enemyToDelete = selectedEnemyIndex;
                            }


                            // 追加ボタン（全体のリストに対する操作）
                            if (ImGui::Button("敵を追加"))
                            {
                                PlacementData newEnemy;
                                if (!objects.empty())
                                {
                                    // 最後に選択していた敵のパラメータを引き継ぐ
                                    int copyIndex = (selectedEnemyIndex >= 0) ? selectedEnemyIndex : static_cast<int>(objects.size() - 1);
                                    newEnemy = objects[copyIndex];
                                    newEnemy.position.x += 0.5f; // 重ならないようにずらす
                                } else
                                {
                                    newEnemy.position = target.position;
                                }
                                objects.push_back(newEnemy);

                                // 追加した敵を自動で選択状態にする
                                selectedEnemyIndex = static_cast<int>(objects.size()) - 1;
                                isParamChanged = true;
                            }

                            // 敵の削除処理
                            if (enemyToDelete >= 0 && enemyToDelete < objects.size())
                            {
                                objects.erase(objects.begin() + enemyToDelete);
                                isParamChanged = true;
                            }

                            // GUI操作による変更があった場合、JSON配列を再構築して保存する
                            if (isParamChanged)
                            {
                                history_->SaveHistory(placementList);
                                isDirty = true;

                                nlohmann::json enemyArray = nlohmann::json::array();
                                for (const auto& cfg : objects)
                                {
                                    nlohmann::json spawnData;
									toJson(spawnData, cfg);

                                    enemyArray.push_back(spawnData);
                                }

                                std::string jsonStr = enemyArray.dump();
                                strcpy_s(target.eventStringParam, sizeof(target.eventStringParam), jsonStr.c_str());
                            }

                            break;
                        }


                        ImGui::TreePop();
                    }
                }

            }
        }else if (target.category == EditCategory::Weapon)
        {
            Weapon* weaponPtr = static_cast<Weapon*>(target.instancePtr);
            weaponPtr->DrawDebugUI(&target, placementList, history_, &isDirty);
        }
    }

    ImGui::PopStyleVar();

    ImGui::End();
}


/// @brief モーションの選択UIを表示する
/// @param motionType 
/// @param motionName 
bool StageEditorUIObjectList::MotionSelecter(const char* label, MotionType motionType, MotionConfig& motionConfig)
{
    // 選択されたモーションタイプに応じたモーション名のリストをMotionManagerから取得
    std::vector<std::string> motionNames = MotionManager::GetInstance()->GetMotionNames(motionType);

    bool isChanged = false; // モーションが変更されたかどうかを示すフラグ

    // モーション名のリストが空の場合はエラーメッセージを表示
    if (motionNames.empty())
    {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "モーションがロードされていません");
    } else
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

                    isChanged = true; // モーションが変更されたフラグを立てる
                }
                if (isSelected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
    }

    return isChanged;
}

/// @brief モーションの選択UIを表示する
/// @param motionType 
/// @param motionName 
/// @param placementList
void StageEditorUIObjectList::MotionSelecter(const char* label, MotionType motionType, MotionConfig& motionConfig, std::vector<PlacementData>& placementList, bool& isDirty)
{
    // 選択されたモーションタイプに応じたモーション名のリストをMotionManagerから取得
    std::vector<std::string> motionNames = MotionManager::GetInstance()->GetMotionNames(motionType);

    // モーション名のリストが空の場合はエラーメッセージを表示
    if (motionNames.empty())
    {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "モーションがロードされていません");
    } else
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
                    isDirty = true;

                    motionConfig.name = name;
                    motionConfig.handle = motionManager_->GetMotion(motionType, motionConfig.name);
                }
                if (isSelected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
    }
}
