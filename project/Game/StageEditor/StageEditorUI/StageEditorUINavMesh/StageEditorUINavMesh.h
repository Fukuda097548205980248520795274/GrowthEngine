#pragma once

class NavMesh;

class StageEditorUINavMesh
{
public:

	/// @brief コンストラクタ
    /// @param navMesh 
    /// @param canExtrude 
    /// @param canBridge 
    /// @param isDirty 
    void DrawUI(NavMesh* navMesh, bool canExtrude, bool canBridge, bool& isDirty);
};

