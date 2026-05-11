#pragma once
#include "Parameter/Parameter.h"
#include "BehaviorTree/BehaviorTreeProjectManager/BehaviorTreeProjectManager.h"

class BehaviorTreeSetting : public Engine::Parameter
{
public:

	/// @brief コンストラクタ
    /// @param folderName 
    BehaviorTreeSetting(const std::string& folderName) : Parameter(folderName) {}

	/// @brief ツリー構造をファイルに保存する
    /// @param fileName 
    /// @param nodes 
    /// @param links 
    void SaveTree(const std::string& fileName, const std::vector<EditorNode>& nodes, const std::vector<EditorLink>& links);

	/// @brief ファイルからツリー構造を読み込む
	/// @param fileName 
	/// @param out_nodes 
	/// @param out_links 
	void LoadTree(const std::string& fileName, std::vector<EditorNode>& out_nodes, std::vector<EditorLink>& out_links);


    
    void RegisterGroupDataReflection(const std::string& fileName) override {}
    void SaveFile(const std::string& fileName) override {}
};

