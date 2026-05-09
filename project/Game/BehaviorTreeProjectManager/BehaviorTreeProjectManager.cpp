#include "BehaviorTreeProjectManager.h"
#include <filesystem>

/// @brief コンストラクタ
BehaviorTreeProjectManager::BehaviorTreeProjectManager()
{
    directoryPath_ = "./Assets/Parameter/BehaviorTree/";

    // ディレクトリがなければ作成
    std::filesystem::create_directories(directoryPath_);
}

/// @brief 保存されているツリーのファイル名一覧を返す
/// @return 
std::vector<std::string> BehaviorTreeProjectManager::GetFileList()
{
    std::vector<std::string> files;
    for (const auto& entry : std::filesystem::directory_iterator(directoryPath_))
    {
        if (entry.path().extension() == ".json")
        {
            files.push_back(entry.path().stem().string()); // 拡張子なしのファイル名
        }
    }
    return files;
}