#include "SharedTreeEditorClipboard.h"

/// @brief シングルトンインスタンスを取得する
/// @return 
SharedTreeEditorClipboard& SharedTreeEditorClipboard::GetInstance()
{
    static SharedTreeEditorClipboard instance;
    return instance;
}

/// @brief クリップボードをクリアする
void SharedTreeEditorClipboard::Clear()
{
    currentDataType = DataType::None;
    comboNodes.clear();
    behaviorNodes.clear();
}