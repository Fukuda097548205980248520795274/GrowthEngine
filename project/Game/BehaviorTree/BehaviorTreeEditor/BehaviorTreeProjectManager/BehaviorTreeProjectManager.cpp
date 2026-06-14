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

	// ディレクトリが存在しない場合は空のリストを返す
	if (!std::filesystem::exists(directoryPath_)) return files;

	for (const auto& entry : std::filesystem::directory_iterator(directoryPath_))
	{
		if (entry.path().extension() == ".json")
		{
			files.push_back(entry.path().stem().string()); // 拡張子なしのファイル名
		}
	}
	return files;
}

/// @brief プロジェクトファイルを削除する
/// @param fileName 
/// @return 
bool BehaviorTreeProjectManager::DeleteProjectFile(const std::string& fileName)
{
	std::string filePath = directoryPath_ + fileName + ".json";

	// ファイルが存在すれば削除する
	if (std::filesystem::exists(filePath))
	{
		return std::filesystem::remove(filePath);
	}

	return false;
}

/// @brief プロジェクトファイルをコピーする
/// @param sourceFileName 
/// @param destFileName 
/// @return 
bool BehaviorTreeProjectManager::CopyProjectFile(const std::string& sourceFileName, const std::string& destFileName)
{
	std::string srcPath = directoryPath_ + sourceFileName + ".json";
	std::string dstPath = directoryPath_ + destFileName + ".json";

	// コピー先が既に存在する場合は上書きするオプションを指定
	std::error_code ec;
	bool success = std::filesystem::copy_file(srcPath, dstPath, std::filesystem::copy_options::overwrite_existing, ec);

	return success;
}