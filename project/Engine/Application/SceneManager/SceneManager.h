#pragma once
#include "Application/Scene/Scene.h"
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

	/// @brief 遷移
	/// @param sceneName 
	void Transition(const std::string& sceneName) { sceneName_ = sceneName; isTransition_ = true; }


private:

	/// @brief エンジン
	const GrowthEngine* engine_ = nullptr;

private:

	/// @brief シーン
	std::unique_ptr<Scene> scene_ = nullptr;

	// 遷移したいシーン名
	std::string sceneName_{};

	// 遷移フラグ
	bool isTransition_ = true;
};