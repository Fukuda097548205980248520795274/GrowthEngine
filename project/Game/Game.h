#pragma once
#include "ContextEngine/ContextEngine.h"

class Game
{
public:

	/// @brief コンストラクタ
	/// @param screenWidth 
	/// @param screenHeight 
	/// @param title 
	Game(int32_t screenWidth, int32_t screenHeight,const std::string& title);

	/// @brief デストラクタ
	~Game();

	/// @brief 実行
	/// @return 
	int Run();
};

