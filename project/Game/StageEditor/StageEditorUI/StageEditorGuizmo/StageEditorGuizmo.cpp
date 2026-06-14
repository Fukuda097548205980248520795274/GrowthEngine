#include "StageEditorGuizmo.h"
#include <numbers>

#include "StageEditor/StageEditorHistory/StageEditorHistory.h"

#include "Entity/Character/NPC/NPC.h"
#include "Entity/Character/Player/Player.h"
#include "Entity/Weapon/Weapon.h"

#include "StageObject/Floor/Floor.h"
#include "StageObject/Wall/Wall.h"
#include "StageObject/StaticEventTrigger/StaticEventTrigger.h"


/// @brief 更新と描画を行う
/// @param placementList 
/// @param selectedIndex 
/// @param isDirty 
/// @param history 
void StageEditorGuizmo::UpdateAndDraw(std::vector<PlacementData>& placementList, int selectedIndex, bool& isDirty, StageEditorHistory* history)
{
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

		// キーボード入力などで操作モードを切り替える例
		if (ImGui::IsKeyPressed(ImGuiKey_T)) currentOperation_ = ImGuizmo::TRANSLATE;
		if (ImGui::IsKeyPressed(ImGuiKey_R)) currentOperation_ = ImGuizmo::ROTATE;
		if (ImGui::IsKeyPressed(ImGuiKey_S)) currentOperation_ = ImGuizmo::SCALE;

		// ギズモの描画と操作
		ImGuizmo::Manipulate(&viewMatrix.m[0][0], &projectionMatrix.m[0][0], currentOperation_, currentMode_, &worldMatrix.m[0][0]);

		// ギズモを使ってオブジェクトを操作中かどうかをチェック
		if (ImGuizmo::IsUsing() && useSnap_)
		{
			float translation[3];
			float rotation[3];
			float scale[3];

			// ワールド行列からSRT成分を抽出
			ImGuizmo::DecomposeMatrixToComponents(&worldMatrix.m[0][0], translation, rotation, scale);

			// 回転はImGuizmoが度単位で返すので、ラジアンに変換するための定数
			constexpr float DEG2RAD = std::numbers::pi_v<float> / 180.0f;

			switch (currentOperation_)
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
		if (ImGuizmo::IsUsing() && !useSnap_)
		{
			// 配置データを更新
			history->SaveHistory(placementList);
			isDirty = true;
		}

		// ギズモを操作している間は、スナップ機能を有効にする例（オプション）
		useSnap_ = ImGuizmo::IsUsing();
	}
}