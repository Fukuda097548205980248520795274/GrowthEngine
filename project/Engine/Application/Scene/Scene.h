#pragma once
#include <string>

class SceneManager;
class GrowthEngine;

class Scene
{
public:

	/// @brief コンストラクタ
	/// @param sceneManager 
	Scene(SceneManager* sceneManager);

	/// @brief 仮想デストラクタ
	virtual ~Scene() = default;


	/// @brief 初期化
	virtual void Initialize() = 0;

	/// @brief 更新処理
	virtual void Update() = 0;

	/// @brief 描画処理
	virtual void Draw() = 0;

	/// @brief シーン遷移する
	/// @param sceneName 
	void Transition(const std::string& sceneName);


protected:

	/// @brief エンジン
	const GrowthEngine* engine_ = nullptr;


private:

	/// @brief シーンマネージャ
	SceneManager* sceneManager_ = nullptr;
};

