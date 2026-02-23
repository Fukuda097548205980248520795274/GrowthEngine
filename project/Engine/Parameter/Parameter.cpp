#include "Parameter.h"

/// @brief ファイルを作成する
/// @param groupName 
void Engine::Parameter::CreateRecordFile(const std::string& groupName)
{
	// ディレクトリがなければ作成する
	std::filesystem::path dir(directory_);
	if (!std::filesystem::exists(dir))
	{
		std::filesystem::create_directory(dir);
	}

	// ファイルパス
	std::string filePath = directory_ + groupName + ".json";

	// 入力ファイルストリーム
	std::ifstream ifs;
	ifs.open(filePath);

	// 既存のファイルを削除する
	if (ifs.is_open())
	{
		ifs.close();
		std::filesystem::remove(filePath);
	}

	// 新たにファイルを作成する
	std::ofstream ofs;
	ofs.open(filePath);
}