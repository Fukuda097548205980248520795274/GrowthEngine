#pragma once
#include <string>
#include "ComboTree/ComboTree.h"
#include "../ComboTreeData/ComboTreeData.h"

class ComboTreeFactory
{
public:

	/// @brief コンボツリーを生成する（エディタ上のノードとリンクから）
	/// @param editorNodes 
	/// @param editorLinks 
	/// @param character 
	/// @return 
	static std::unique_ptr<ComboTree> CreateTree(const std::vector<ComboEditorNode>& editorNodes, const std::vector<ComboEditorLink>& editorLinks,
		Character* character);
};

