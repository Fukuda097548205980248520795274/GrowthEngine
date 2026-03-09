#include "Parameter.h"
#include <Windows.h>
#include "Func/ConvertString/ConvertString.h"

/// @brief コンストラクタ
/// @param folderName 
Engine::Parameter::Parameter(const std::string& folderName) : folderName_(folderName) 
{
	// ディレクトリを掘る
	if (!CreateDirectory(ConvertString(directory_ + folderName).c_str(), nullptr))
	{
		if (GetLastError() != ERROR_ALREADY_EXISTS)
		{
			assert(false);
		}
	}
}

/// @brief ファイルがあるかどうか
/// @param fileName 
/// @return 
bool Engine::Parameter::IsFileFound(const std::string& fileName)
{
	// ファイルパス
	std::string filePath = directory_ + folderName_ + "/" + fileName + ".json";

	// 入力ファイルストリーム
	std::ifstream ifs;
	ifs.open(filePath);

	// ファイルが開けないとき
	if (!ifs.is_open())
		return false;

	return true;
}

/// @brief ファイルを作成する
/// @param fileName 
void Engine::Parameter::CreateRecordFile(const std::string& fileName)
{
	// ディレクトリがなければ作成する
	std::filesystem::path dir(directory_);
	if (!std::filesystem::exists(dir))
	{
		std::filesystem::create_directory(dir);
	}

	// ファイルパス
	std::string filePath = directory_ + folderName_ + "/" + fileName + ".json";

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