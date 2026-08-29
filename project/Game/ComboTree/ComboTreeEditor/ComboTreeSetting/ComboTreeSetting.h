#pragma once
#include "Parameter/Parameter.h"
#include "../ComboTreeData/ComboTreeData.h"

class ComboTreeSetting : public Engine::Parameter
{
public:

    /// @brief コンストラクタ
    /// @param folderName 
    ComboTreeSetting(const std::string& folderName) : Parameter(folderName) {}

    /// @brief ツリー構造をファイルに保存する
    /// @param fileName 
    /// @param nodes 
    /// @param links 
    void SaveTree(const std::string& fileName, const std::vector<ComboEditorNode>& nodes, const std::vector<ComboEditorLink>& links);

    /// @brief ファイルからツリー構造を読み込む
    /// @param fileName 
    /// @param outNodes 
    /// @param outLinks 
    void LoadTree(const std::string& fileName, std::vector<ComboEditorNode>& outNodes, std::vector<ComboEditorLink>& outLinks);



    void RegisterGroupDataReflection(const std::string& fileName) override {}
    void SaveFile(const std::string& fileName) override {}
};

