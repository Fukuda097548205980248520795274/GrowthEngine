#pragma once
#include "EngineObject/Scene/Scene.h"
#include <memory>
#include <string>

class GrowthEngine;

class SceneManager
{
public:

	/// @brief コンストラクタ
	/// @param sceneName 
	SceneManager(const std::string& sceneName);

	/// @brief 更新処理
	void Update();

	/// @brief 描画処理
	void Draw();

	/// @brief シーン遷移する
	/// @param sceneName 
	/// @param stageName 
	void Transition(const std::string& sceneName, const std::string& stageName = "") { prevSceneName_ = sceneName_; sceneName_ = sceneName; nextStageName_ = stageName; isTransition_ = true; }


	/// @brief 次のステージ名を取得する
	/// @return 
	std::string GetNextStageName() const { return nextStageName_; }

	/// @brief 前のシーン名を取得する
	/// @return 
	std::string GetPrevSceneName() const { return prevSceneName_; }

	/// @brief チュートリアルクリアフラグを取得する
	/// @return 
	bool IsTutorialCleared() const { return isTutorialCleared_; }

	/// @brief チュートリアルクリアフラグを設定する
	/// @param flag 
	void SetTutorialCleared(bool flag) { isTutorialCleared_ = flag; }


private:

	/// @brief エンジン
	GrowthEngine* engine_ = nullptr;

private:

	/// @brief シーン
	std::unique_ptr<Scene> scene_ = nullptr;

	// 遷移したいシーン名
	std::string sceneName_{};

	// 現在のシーン名
	std::string currentSceneName_{};

	/// @brief 前のシーン名
	std::string prevSceneName_{};

	// 遷移フラグ
	bool isTransition_ = true;


private:

	/// @brief 次のステージ名
	std::string nextStageName_{};

	/// @brief チュートリアルクリアフラグ
	bool isTutorialCleared_ = false;
};