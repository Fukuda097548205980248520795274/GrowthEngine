#pragma once

class NavMesh;
class StageEditorNavMeshController;

class StageEditorUINavMeshInfo
{
public:

	/// @brief コンストラクタ
	StageEditorUINavMeshInfo() = default;

	/// @brief デストラクタ
	~StageEditorUINavMeshInfo() = default;

	/// @brief ウィンドウの描画処理
	/// @param navMesh 
	/// @param controller 
	void DrawWindow(NavMesh* navMesh, const StageEditorNavMeshController* controller);

private:

	// ウィンドウの開閉状態
	bool isOpen_ = true;

	// 選択されたポリゴンID（-1は未選択を表す）
	int targetPolygonId_ = -1;
};

