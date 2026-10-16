#pragma once

/// @brief ナビゲーションメッシュ
class NavMesh;

/// @brief ステージエディタナビゲーションメッシュ操作
class StageEditorNavMeshController;

/// @brief ステージエディタナビゲーションメッシュUI
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

