#pragma once

class NavMesh;
class StageEditorNavMeshController;

class StageEditorUINavMesh
{
public:

	/// @brief コンストラクタ
	/// @param navMesh 
	/// @param canExtrude 
	/// @param canBridge 
	/// @param isDirty 
	void DrawUI(NavMesh* navMesh, bool canExtrude, bool canBridge, StageEditorNavMeshController* navMeshController, bool& isDirty);
};

