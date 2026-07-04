#pragma once
#include "../ComboTreeData/ComboTreeData.h"

class ComboTreeEditor;

class ComboTreeEditorClipboard
{
public:

	/// @brief コピーしたノードとリンクの情報をクリップボードに保存する
	/// @param sourceNodes 
	/// @param sourceLinks 
	void HandleCopy(const std::vector<ComboEditorNode>& sourceNodes, const std::vector<ComboEditorLink>& sourceLinks);

	/// @brief コピーしたノードとリンクの情報をエディタにペーストする
	/// @param editor 
	void HandlePaste(ComboTreeEditor& editor);


private:

	/// @brief コピーしたノードの情報を保持する変数
	std::vector<ComboEditorNode> clipboardNodes_;

	/// @brief コピーしたリンクの情報を保持する変数
	std::vector<ComboEditorLink> clipboardLinks_;
};

