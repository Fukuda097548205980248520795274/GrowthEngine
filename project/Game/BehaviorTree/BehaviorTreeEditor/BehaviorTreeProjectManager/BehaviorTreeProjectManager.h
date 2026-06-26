#pragma once
#include "../BehaviorTreeData/BehaviorTreeData.h"
#include <vector>
#include <string>

class BehaviorTreeProjectManager
{
public:

	/// @brief コンストラクタ
	BehaviorTreeProjectManager();

	/// @brief 保存されているツリーのファイル名一覧を返す
	/// @return 
	std::vector<std::string> GetFileList();

	/// @brief プロジェクトファイルを削除する
	/// @param fileName 
	/// @return 
	bool DeleteProjectFile(const std::string& fileName);

	/// @brief プロジェクトファイルをコピーする
	/// @param sourceFileName 
	/// @param destFileName 
	/// @return 
	bool CopyProjectFile(const std::string& sourceFileName, const std::string& destFileName);

private:

	// ツリー構造の保存先ディレクトリパス
	std::string directoryPath_;
};

