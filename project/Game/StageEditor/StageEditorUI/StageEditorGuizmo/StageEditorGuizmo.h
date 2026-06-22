#pragma once
#include "GrowthEngine.h"
#include "StageEditor/StageData/StageData.h"

class StageEditorHistory;
class NavMesh;
class StageEditorNavMeshController;

class StageEditorGuizmo
{
public:

	/// @brief オブジェクトの変形を更新する
	/// @param placementList 
	/// @param selectedIndex 
	/// @param isDirty 
	/// @param history 
	void UpdateObject(std::vector<PlacementData>& placementList, int selectedIndex, bool& isDirty, StageEditorHistory* history);

	/// @brief ナビメッシュの変形を更新する
	/// @param navMesh 
	/// @param navMeshController 
	/// @param isDirty 
	/// @param history 
	void UpdateNavMesh(NavMesh* navMesh, StageEditorNavMeshController* navMeshController, bool& isDirty, StageEditorHistory* history);


private:

	// 現在の操作モード
	ImGuizmo::OPERATION currentOperation_ = ImGuizmo::TRANSLATE;

	// ローカルモードかワールドモードか
	ImGuizmo::MODE currentMode_ = ImGuizmo::LOCAL;

	// オブジェクトのスナップの使用フラグ
	bool useObjectSnap_ = false;

	// ナビメッシュのスナップの使用フラグ
	bool useNavMeshSnap_ = false;

	// エンジンのインスタンス
	const GrowthEngine* engine_ = GrowthEngine::GetInstance();
};

