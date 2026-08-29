#pragma once
#include "BehaviorTree/BehaviorTreeEditor/BehaviorTreeData/BehaviorTreeData.h"
#include "ComboTree/ComboTreeEditor/ComboTreeData/ComboTreeData.h"

class SharedTreeEditorClipboard
{
public:

	/// @brief クリップボードに入っているデータの種類
    enum class DataType
    {
        None,
        ComboTree,
        BehaviorTree
    };


public:

	/// @brief シングルトンインスタンスを取得する
    /// @return 
    static SharedTreeEditorClipboard& GetInstance();

	/// @brief クリップボードをクリアする
    void Clear();


public:

    // 現在クリップボードに入っているデータの種類
    DataType currentDataType = DataType::None;

    // コンボツリー用のバッファ
    std::vector<ComboEditorNode> comboNodes;

    // ビヘイビアツリー用のバッファ
    std::vector<EditorNode> behaviorNodes;

    // 攻撃アニメーション名
    std::string motionName;

    // GrabStrike用のターゲットアニメーション名
    std::string targetMotionName;
};

