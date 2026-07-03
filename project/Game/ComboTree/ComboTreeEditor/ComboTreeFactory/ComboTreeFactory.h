#pragma once
#include <string>
#include "ComboTree/ComboTree.h"

class ComboTreeFactory
{
public:

	/// @brief コンボツリーを生成する
	/// @param jsonFilePath 
	/// @param character 
	/// @return 
	static ComboTree CreateTree(const std::string& jsonFilePath, Character* character);
};

