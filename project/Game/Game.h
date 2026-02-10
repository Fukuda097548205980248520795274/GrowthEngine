#pragma once
#include "GrowthEngine.h"

#include "Application/AudioObject/AudioObject.h"

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

	std::unique_ptr<AudioObject> object_ = nullptr;

	std::unique_ptr<PrimitiveStaticModel> model_ = nullptr;
	std::unique_ptr<PrimitiveStaticModel> plane_ = nullptr;

	std::unique_ptr<GameCamera> gameCamera_ = nullptr;

	std::unique_ptr<LightDirectional> light_ = nullptr;
};

