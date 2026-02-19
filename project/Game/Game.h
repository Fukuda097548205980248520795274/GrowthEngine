#pragma once
#include "GrowthEngine.h"

class Game : public Framework
{
public:

	/// @brief コンストラクタ
	/// @param screenWidth 
	/// @param screenHeight 
	/// @param title 
	Game(int32_t screenWidth, int32_t screenHeight, const std::string& title)
		: Framework(screenWidth, screenHeight, title){}

	/// @brief 初期化
	void Initialize() override;

	/// @brief 更新処理
	void Update() override;

	/// @brief 描画処理
	void Draw() override;


private:

	/// @brief シーンマネージャ
	std::unique_ptr<SceneManager> sceneManager_ = nullptr;
};

