#pragma once
#include <vector>
#include "BehaviorTree/BehaviorTreeProjectManager/BehaviorTreeProjectManager.h"

class BehaviorTreeEditor;

class BehaviorTreeEditorClipboard
{
public:

	/// @brief コピーしたノードとリンクの情報をクリップボードに保存する
	/// @param sourceNodes 
	/// @param sourceLinks 
	void HandleCopy(const std::vector<EditorNode>& sourceNodes, const std::vector<EditorLink>& sourceLinks);

	/// @brief コピーしたノードとリンクの情報をエディタにペーストする
	/// @param editor 
	void HandlePaste(BehaviorTreeEditor& editor);


private:

	/// @brief コピーしたノードの情報を保持する変数
	std::vector<EditorNode> clipboardNodes_;

	/// @brief コピーしたリンクの情報を保持する変数
	std::vector<EditorLink> clipboardLinks_;
};

