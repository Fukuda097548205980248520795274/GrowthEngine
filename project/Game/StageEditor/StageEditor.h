#pragma once
#include "StageData/StageData.h"

class StageSerializer;

class StageEditor
{
public:

	/// @brief コンストラクタ
	/// @param stageData 
	StageEditor(StageData& stageData) : stageData_(stageData){}

	/// @brief エディター用UIの更新
	/// @param serializer 
	void UpdateEditorUI(StageSerializer* serializer);

private:

	/// @brief ステージデータ
	StageData& stageData_;

	/// @brief リサイズしているかどうか
	bool isResize_ = false;

	// 選択中のタイル
	StageData::Tile selectedTile_ = StageData::Tile::Wall;
};

