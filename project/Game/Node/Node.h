#pragma once
#include "GrowthEngine.h"

enum class EditorNodeType;

class Node
{
public:

	/// @brief ノードの状態
	enum class State
	{
		// 成功
		Success,

		// 失敗
		Failure,

		// 実行中
		Running,

		None,
	};


public:

	/// @brief コンストラクタ
	Node();

	/// @brief コンストラクタ
	virtual ~Node() = default;

	/// @brief ノードの更新
	/// @return 
	State UpdateNode();

	/// @brief 実行
	/// @return ノードの状態
	virtual State Exec() = 0;

	/// @brief 中断処理
	virtual void Abort() {};


	/// @brief デバッグ状態のリセット（毎フレームのツリー更新の先頭で呼ぶ）
	virtual void ResetStatusRecursive() { lastState_ = State::None; }

	/// @brief デバッグ用の再帰描画処理
	/// @return 
	virtual void DrawDebuggerRecursive(float zoom = 1.0f);

	/// @brief デバッグ情報を設定する
	/// @param id 
	/// @param inPin 
	/// @param outPin 
	/// @param pos 
	/// @param name 
	/// @param type 
	void SetDebugInfo(int id, int inPin, int outPin, const Vector2& pos, const std::string& name, EditorNodeType type);

	/// @brief 最後の状態を取得する
	/// @return 
	State GetLastState() const { return lastState_; }

	/// @brief エディタ上のノードIDを取得する
	/// @return 
	int GetEditorNodeId() const { return editorNodeId_; }

	/// @brief 入力ピンIDを取得する
	/// @return 
	int GetInputPinId() const { return inputPinId_; }

	/// @brief 出力ピンIDを取得する
	/// @return 
	int GetOutputPinId() const { return outputPinId_; }


protected:

	/// @brief エンジン
	const GrowthEngine* engine_ = nullptr;


protected:

	// 最後の状態
	State lastState_ = State::None;

	// エディタ上のノードID
	int editorNodeId_ = -1;

	// 入力ピンID
	int inputPinId_ = -1;

	// 出力ピンID
	int outputPinId_ = -1;

	// ノードの位置
	Vector2 pos_ = { 0.0f, 0.0f };

	// ノードの名前
	std::string nodeName_ = "Unknown";

	// ノードのタイプ
	EditorNodeType type_;
};

